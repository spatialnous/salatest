// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/metagraph.h"
#include "salalib/shapemapgroupdata.h"

#include "catch_amalgamated.hpp"

TEST_CASE("Test getVisibleLines", "") {
    const float epsilon = 0.001f;

    // create a new MetaGraph
    std::unique_ptr<MetaGraph> mgraph(new MetaGraph());

    Point2f visibleLineStart(0, 0);
    Point2f visibleLineEnd(2, 4);
    Point2f hiddenLineStart(1, 1);
    Point2f hiddenLineEnd(3, 5);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;

    // push a ShapeMap in the SpacePixelFile
    spacePixels.emplace_back("Visible ShapeMap");

    // add a line to the first ShapeMap
    spacePixels.back().makeLineShape(Line4f(visibleLineStart, visibleLineEnd));

    // push a ShapeMap in the SpacePixelFile
    spacePixels.emplace_back("Hidden ShapeMap");

    // add a line to the second ShapeMap
    spacePixels.back().makeLineShape(Line4f(hiddenLineStart, hiddenLineEnd));

    SECTION("Get lines") {

        const std::vector<SimpleLine> &visibleLines0 = spacePixels[0].getAllShapesAsSimpleLines();
        const std::vector<SimpleLine> &visibleLines1 = spacePixels[1].getAllShapesAsSimpleLines();

        REQUIRE(visibleLines0.size() == 1);
        REQUIRE(visibleLines1.size() == 1);
        REQUIRE(visibleLines0[0].start().x == Catch::Approx(visibleLineStart.x).epsilon(epsilon));
        REQUIRE(visibleLines0[0].start().y == Catch::Approx(visibleLineStart.y).epsilon(epsilon));
        REQUIRE(visibleLines0[0].end().x == Catch::Approx(visibleLineEnd.x).epsilon(epsilon));
        REQUIRE(visibleLines0[0].end().y == Catch::Approx(visibleLineEnd.y).epsilon(epsilon));
        REQUIRE(visibleLines1[0].start().x == Catch::Approx(hiddenLineStart.x).epsilon(epsilon));
        REQUIRE(visibleLines1[0].start().y == Catch::Approx(hiddenLineStart.y).epsilon(epsilon));
        REQUIRE(visibleLines1[0].end().x == Catch::Approx(hiddenLineEnd.x).epsilon(epsilon));
        REQUIRE(visibleLines1[0].end().y == Catch::Approx(hiddenLineEnd.y).epsilon(epsilon));
    }
}
