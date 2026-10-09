// SPDX-FileCopyrightText: 2026 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-only
//
// Serial-vs-OpenMP equivalence test for the VGA modules.

#include "catch_amalgamated.hpp"

#include "salalib/latticemap.hpp"
#include "salalib/metagraph.hpp"
#include "salalib/vgamodules/vgaangular.hpp"
#include "salalib/vgamodules/vgaangularopenmp.hpp"
#include "salalib/vgamodules/vgametric.hpp"
#include "salalib/vgamodules/vgametricopenmp.hpp"
#include "salalib/vgamodules/vgavisualglobal.hpp"
#include "salalib/vgamodules/vgavisualglobalopenmp.hpp"

#if defined(_OPENMP)
#include <omp.h>
#endif

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

    using NamedCols = std::map<std::string, std::vector<double>>;

    // A 10x10 grid at 1.0 spacing with two offset walls laid along cell centres.
    //
    // Verified properties of this fixture, each of which matters:
    //
    //   * 105 points filled of the 121 grid positions.  The walls sit ON cell centres, so
    //     16 positions are blocked.
    //   * Angular Mean Depth ranges 0.34 .. 1.46, i.e. the topology is genuinely non-trivial.
    //     A fully open grid gives mean depth 0.0 at EVERY point -- a complete graph, which
    //     exercises the traversal while testing nothing about which pixel sees which.  That
    //     control was run; it is the degenerate case this fixture avoids.
    //   * The walls leave gaps top and bottom, so all 105 points stay mutually reachable.
    //     A disconnected component would make mean depth infinite and the comparison
    //     meaningless.
    //
    struct Fixture {
        MetaGraph metaGraph{"equivalence fixture"};
        std::unique_ptr<LatticeMap> map;

        Fixture() {
            metaGraph.region = Region4f(Point2f(0.0, 0.0), Point2f(10.0, 10.0));
            map = std::make_unique<LatticeMap>(metaGraph.region, "equivalence fixture");

            REQUIRE(map->setGrid(1.0, Point2f(0.0, 0.0)));

            std::vector<Line4f> walls{
                Line4f(Point2f(4.0, 0.0), Point2f(4.0, 7.0)),  // gap at the top
                Line4f(Point2f(7.0, 3.0), Point2f(7.0, 10.0)), // gap at the bottom
            };
            REQUIRE(map->blockLines(walls));

            const int fullFill = 0;
            REQUIRE(map->makePoints(Point2f(1.0, 1.0), fullFill, nullptr));
            REQUIRE(map->sparkGraph2(nullptr, false, -1.0));

            REQUIRE(map->getFilledPointCount() == 105);
        }
    };

    NamedCols collect(AnalysisResult &res, size_t rows) {
        NamedCols out;
        const auto &names = res.getAttributes();
        for (size_t c = 0; c < names.size(); c++) {
            std::vector<double> col;
            col.reserve(rows);
            for (size_t r = 0; r < rows; r++) {
                col.push_back(res.getValue(r, c));
            }
            out[names[c]] = std::move(col);
        }
        return out;
    }

    template <class MakeAnalysis> NamedCols runFresh(MakeAnalysis make) {
        Fixture fixture;
        auto analysis = make(*fixture.map);
        auto result = analysis.run(nullptr);
        return collect(result, fixture.map->getAttributeTable().getNumRows());
    }

    void requireEquivalent(const NamedCols &serial, const NamedCols &parallel, int threads) {
        INFO("threads requested = " << threads);

        std::vector<std::string> serialNames;
        std::vector<std::string> parallelNames;
        for (const auto &kv : serial) {
            serialNames.push_back(kv.first);
        }
        for (const auto &kv : parallel) {
            parallelNames.push_back(kv.first);
        }
        REQUIRE(serialNames == parallelNames);

        for (const auto &kv : serial) {
            const auto &name = kv.first;
            const auto &expected = kv.second;
            const auto &actual = parallel.at(name);
            INFO("column = " << name);
            REQUIRE(expected.size() == actual.size());
            for (size_t i = 0; i < expected.size(); i++) {
                INFO("row = " << i);
                REQUIRE(expected[i] == Catch::Approx(actual[i]).epsilon(1e-6).margin(1e-9));
            }
        }
    }

    void reportParallelCapability() {
#if !defined(_OPENMP)
        WARN("Built WITHOUT OpenMP: the OpenMP modules run serially, so these cases compare "
             "serial against serial and cannot detect a parallelism defect. Build with "
             "OpenMP enabled for this test to mean anything.");
#else
        INFO("omp_get_max_threads() = " << omp_get_max_threads());
        if (omp_get_max_threads() < 2) {
            WARN("Only one thread available. The requested thread counts are still "
                 "honoured (OpenMP oversubscribes), so the parallel paths do execute, but "
                 "true concurrency is limited.");
        }
#endif
    }

    constexpr double RADIUS = -1.0;
    const std::vector<int> THREAD_COUNTS{1, 2, 4};

} // namespace

TEST_CASE("VGAAngularOpenMP matches VGAAngular", "[vga][openmp]") {
    reportParallelCapability();
    const auto serial = runFresh([](LatticeMap &m) { return VGAAngular(m, RADIUS, false); });
    for (int threads : THREAD_COUNTS) {
        const auto parallel = runFresh([threads](LatticeMap &m) {
            return VGAAngularOpenMP(m, RADIUS, false, std::make_optional(threads), false);
        });
        requireEquivalent(serial, parallel, threads);
    }
}

TEST_CASE("VGAMetricOpenMP matches VGAMetric", "[vga][openmp]") {
    reportParallelCapability();
    const auto serial = runFresh([](LatticeMap &m) { return VGAMetric(m, RADIUS, false); });
    for (int threads : THREAD_COUNTS) {
        const auto parallel = runFresh([threads](LatticeMap &m) {
            return VGAMetricOpenMP(m, RADIUS, false, std::make_optional(threads), false);
        });
        requireEquivalent(serial, parallel, threads);
    }
}

TEST_CASE("VGAVisualGlobalOpenMP matches VGAVisualGlobal", "[vga][openmp]") {
    reportParallelCapability();
    const auto serial = runFresh([](LatticeMap &m) { return VGAVisualGlobal(m, RADIUS, false); });
    for (int threads : THREAD_COUNTS) {
        const auto parallel = runFresh([threads](LatticeMap &m) {
            return VGAVisualGlobalOpenMP(m, RADIUS, false, std::make_optional(threads), false);
        });
        requireEquivalent(serial, parallel, threads);
    }
}
