// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"
#include "catch_amalgamated.hpp"

#include "dmcli/axialparser.hpp"

TEST_CASE("Test mode and help") {
    AxialParser parser;
    REQUIRE(parser.getModeName() == "AXIAL");
    REQUIRE(
        parser.getHelp() ==
        "Mode options for Axial Analysis:\n"
        "  -xl <x>,<y> Calculate all lines map from this seed point (can be used more than once)\n"
        "  -xf Calculate fewest lines map from all lines map\n"
        "  -xa <radius/list of radii> run axial anlysis with specified radii\n"
        " All modes expect to find the required input in the in graph\n"
        " Any combination of flags above can be specified, they will always be run in the order "
        "-aa -af -au -ax\n"
        " Further flags for axial analysis are:\n"
        "   -xac Include choice (betweenness)\n"
        "   -xal Include local measures\n"
        "   -xar Include RA, RRA and total depth\n"
        "   -xaw <map attribute name> perform weighted analysis using this attribute\n"
        "\n");
}

TEST_CASE("Test Parsing Exceptions", "") {
    AxialParser parser;
    SECTION("No axial mode") {
        ArgumentHolder ah{"prog"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()), "No axial analysis mode present");
    }

    SECTION("Argument missing") {
        ArgumentHolder ah{"prog", "-xl"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()), "-xl requires an argument");
    }
}

TEST_CASE("Test mode parsing", "") {
    AxialParser parser;
    SECTION("All lines") {
        ArgumentHolder ah{"prog", "-xl", "1.2,1.5"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.runAllLines());
        REQUIRE(parser.getAllAxesRoots().size() == 1);
        REQUIRE(parser.getAllAxesRoots()[0].x == Catch::Approx(1.2));
        REQUIRE(parser.getAllAxesRoots()[0].y == Catch::Approx(1.5));
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE_FALSE(parser.runAnalysis());
    }
    SECTION("Fewest lines") {
        ArgumentHolder ah{"prog", "-xf"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.runAllLines());
        REQUIRE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE_FALSE(parser.runAnalysis());
    }
    SECTION("Analysis") {
        ArgumentHolder ah{"prog", "-xa", "n"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.runAllLines());
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE(parser.runAnalysis());
        REQUIRE_FALSE(parser.calculateRRA());
        REQUIRE_FALSE(parser.useChoice());
        REQUIRE_FALSE(parser.useLocal());
    }
    SECTION("Analysis -rra") {
        ArgumentHolder ah{"prog", "-xa", "n", "-xar"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.runAllLines());
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE(parser.runAnalysis());
        REQUIRE(parser.calculateRRA());
        REQUIRE_FALSE(parser.useChoice());
        REQUIRE_FALSE(parser.useLocal());
    }
    SECTION("Analysis + choice") {
        ArgumentHolder ah{"prog", "-xa", "n", "-xac"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.runAllLines());
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE(parser.runAnalysis());
        REQUIRE_FALSE(parser.calculateRRA());
        REQUIRE(parser.useChoice());
        REQUIRE_FALSE(parser.useLocal());
    }
    SECTION("Analysis + local") {
        ArgumentHolder ah{"prog", "-xa", "n", "-xal"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE_FALSE(parser.runAllLines());
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE(parser.runAnalysis());
        REQUIRE_FALSE(parser.calculateRRA());
        REQUIRE_FALSE(parser.useChoice());
        REQUIRE(parser.useLocal());
    }

    SECTION("Multiple") {
        ArgumentHolder ah{"prog", "-xl", "1.2,1.5", "-xa", "n", "-xl", "2.4,1.0"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.runAllLines());
        REQUIRE(parser.getAllAxesRoots().size() == 2);
        REQUIRE(parser.getAllAxesRoots()[0].x == Catch::Approx(1.2));
        REQUIRE(parser.getAllAxesRoots()[0].y == Catch::Approx(1.5));
        REQUIRE(parser.getAllAxesRoots()[1].x == Catch::Approx(2.4));
        REQUIRE(parser.getAllAxesRoots()[1].y == Catch::Approx(1.0));
        REQUIRE_FALSE(parser.runFewestLines());
        REQUIRE_FALSE(parser.runUnlink());
        REQUIRE(parser.runAnalysis());
    }
}
