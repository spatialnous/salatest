// SPDX-FileCopyrightText: 2026 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "salalib/agents/agent.hpp"
#include "salalib/agents/agentprogram.hpp"

#include <cmath>
#include <vector>

TEST_CASE("binfromvec returns a bin in 0..31", "Guards the range contract") {
    // calcLoS and calcLoS2 index bins with their directionbin argument directly,
    // without a modulo, so binfromvec must never return 32.
    for (int i = 0; i < 20000; ++i) {
        const double a = (2.0 * M_PI * static_cast<double>(i)) / 20000.0;
        const int bin = binfromvec(Point2f(cos(a), sin(a)));
        REQUIRE(bin >= 0);
        REQUIRE(bin <= 31);
    }
}

TEST_CASE("binfromvec puts due east in bin 0, not bin 32", "The wrap case") {
    // Angles just below 2*pi round up past the end of the bin range. Before
    // normalisation this returned 32, which is bin 0 under another name: the
    // ahead-weighting in onWeightedLook compared against a value masked to
    // 0..31 and so could never match.
    REQUIRE(binfromvec(Point2f(1.0, 0.0)) == 0);
    REQUIRE(binfromvec(Point2f(1.0, -0.0)) == 0);
    REQUIRE(binfromvec(Point2f(1.0, -1e-12)) == 0);
    REQUIRE(binfromvec(Point2f(1.0, -0.05)) == 0);
}

TEST_CASE("binfromvec maps the cardinal directions", "Orientation and winding") {
    REQUIRE(binfromvec(Point2f(1.0, 0.0)) == 0);   // east
    REQUIRE(binfromvec(Point2f(0.0, 1.0)) == 8);   // north
    REQUIRE(binfromvec(Point2f(-1.0, 0.0)) == 16); // west
    REQUIRE(binfromvec(Point2f(0.0, -1.0)) == 24); // south
}

TEST_CASE("binfromvec walks the circle in order and wraps back to 0", "No gaps or repeats") {
    // Sweeping angle from 0 to 2*pi visits every bin exactly once in ascending
    // order, then returns to bin 0 -- bin 0 straddles angle zero, spanning
    // -1/32 to 1/32 of a turn, so it is entered at both ends of the sweep.
    // That final wrap is the case that used to return 32 instead of 0.
    std::vector<int> seen;
    int last = -1;
    for (int i = 0; i < 100000; ++i) {
        const double a = (2.0 * M_PI * static_cast<double>(i)) / 100000.0;
        const int bin = binfromvec(Point2f(cos(a), sin(a)));
        if (bin != last) {
            seen.push_back(bin);
            last = bin;
        }
    }
    REQUIRE(seen.size() == 33);
    for (int i = 0; i < 32; ++i) {
        REQUIRE(seen[static_cast<size_t>(i)] == i);
    }
    REQUIRE(seen.back() == 0); // wrapped, not 32
}

TEST_CASE("isValidSelType accepts every dispatchable look type", "Agent algorithm validation") {
    const int valid[] = {AgentProgram::SEL_STANDARD,
                         AgentProgram::SEL_WEIGHTED,
                         AgentProgram::SEL_LOS,
                         AgentProgram::SEL_LOS_OCC,
                         AgentProgram::SEL_OPTIC_FLOW2,
                         AgentProgram::SEL_LENGTH,
                         AgentProgram::SEL_OPTIC_FLOW,
                         AgentProgram::SEL_COMPARATIVE_LENGTH,
                         AgentProgram::SEL_COMPARATIVE_OPTIC_FLOW,
                         AgentProgram::SEL_OCC_ALL,
                         AgentProgram::SEL_OCC_BIN45,
                         AgentProgram::SEL_OCC_BIN60,
                         AgentProgram::SEL_OCC_STANDARD,
                         AgentProgram::SEL_OCC_WEIGHT_DIST,
                         AgentProgram::SEL_OCC_WEIGHT_ANG,
                         AgentProgram::SEL_OCC_WEIGHT_DIST_ANG,
                         AgentProgram::SEL_OCC_MEMORY};
    for (int selType : valid) {
        REQUIRE(AgentProgram::isValidSelType(selType));
    }
}

TEST_CASE("isValidSelType rejects masks and unknown values", "Agent algorithm validation") {
    // 0x8FFE is what depthmapQ sent for its default "Standard" combo entry: the
    // dialog had no branch for index 0, so it computed SEL_OCCLUSION + (0 - 2).
    // Nothing dispatched it, agents ended up with a NaN heading and the output
    // column came back empty.
    REQUIRE_FALSE(AgentProgram::isValidSelType(0x8FFE));

    // Bare masks are not selectable look types on their own.
    REQUIRE_FALSE(AgentProgram::isValidSelType(AgentProgram::SEL_TARGETTED));
    REQUIRE_FALSE(AgentProgram::isValidSelType(AgentProgram::SEL_GIBSONIAN));
    REQUIRE_FALSE(AgentProgram::isValidSelType(AgentProgram::SEL_GIBSONIAN2));
    REQUIRE_FALSE(AgentProgram::isValidSelType(AgentProgram::SEL_OCCLUSION));

    REQUIRE_FALSE(AgentProgram::isValidSelType(0));
    REQUIRE_FALSE(AgentProgram::isValidSelType(-1));
    REQUIRE_FALSE(AgentProgram::isValidSelType(0x9009));
}

TEST_CASE("selTypeToString formats as hex", "Diagnostics match the enum notation") {
    REQUIRE(AgentProgram::selTypeToString(0x8FFE) == "0x8FFE");
    REQUIRE(AgentProgram::selTypeToString(AgentProgram::SEL_STANDARD) == "0x1001");
}
