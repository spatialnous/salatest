// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"

#include "dmpcli/segmentshortestpathparser.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("SegmentShortestPathParser", "Error cases") {
    SECTION("Missing argument to -sspo") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspd", "0,0", "-sspo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sspo requires an argument"));
    }

    SECTION("Missing argument to -sspd") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspo", "0,0", "-sspd"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sspd requires an argument"));
    }

    SECTION("Missing argument to -sspt") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspo", "0,0", "-sspd", "0,0", "-sspt"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sspt requires an argument"));
    }

    SECTION("rubbish input to -sspo") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspd", "0,0", "-sspo", "foo"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring(
                "Invalid origin point provided (foo). Should only contain digits dots and commas"));
    }

    SECTION("rubbish input to -sspd") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspo", "0,0", "-sspd", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "Invalid destination point provided (foo). Should only "
                                "contain digits dots and commas"));
    }

    SECTION("rubbish input to -sspt") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog", "-sspo", "0,0", "-sspd", "0,0", "-sspt", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Invalid step type: foo"));
    }

    SECTION("Neiter points nor point file provided") {
        SegmentShortestPathParser parser;
        ArgumentHolder ah{"prog"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("Both -sspo and -sspd must be provided"));
    }
}

TEST_CASE("Successful SegmentShortestPathParser", "Read successfully") {
    SegmentShortestPathParser parser;
    double originX = 1.0;
    double originY = 2.0;
    double destinationX = 1.1;
    double destinationY = 1.2;

    SECTION("Read from commandline") {
        std::stringstream originStream;
        originStream << originX << "," << originY << std::flush;
        std::stringstream destinationStream;
        destinationStream << destinationX << "," << destinationY << std::flush;

        ArgumentHolder ah{
            "prog",  "-sspo",      originStream.str(), "-sspd", destinationStream.str(),
            "-sspt", "topological"};
        parser.parse(ah.argc(), ah.argv());
    }

    auto originPoint = parser.getShortestPathOrigin();
    auto destinationPoint = parser.getShortestPathDestination();
    REQUIRE(originPoint.x == Catch::Approx(originX));
    REQUIRE(originPoint.y == Catch::Approx(originY));
    REQUIRE(destinationPoint.x == Catch::Approx(destinationX));
    REQUIRE(destinationPoint.y == Catch::Approx(destinationY));
}
