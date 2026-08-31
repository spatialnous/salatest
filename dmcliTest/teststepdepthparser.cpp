// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"
#include "selfcleaningfile.hpp"

#include "dmcli/stepdepthparser.hpp"

#include "catch_amalgamated.hpp"

#include <fstream>
#include <ostream>
#include <sstream>

TEST_CASE("StepDepthParserFail", "Error cases") {
    SECTION("Missing argument to -sdp") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdp"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sdp requires an argument"));
    }

    SECTION("Missing argument to -sdf") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdf"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sdf requires an argument"));
    }

    SECTION("Missing argument to -sdt") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdt"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-sdt requires an argument"));
    }

    SECTION("rubbish input to -sdp") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdp", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "Invalid step depth point provided (foo). Should only "
                                "contain digits dots and commas"));
    }

    SECTION("rubbish input to -sdt") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdt", "foo"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Invalid step type: foo"));
    }

    SECTION("Non-existing file provided") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog", "-sdf", "foo.csv"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("Failed to load file foo.csv, error"));
    }

    SECTION("Neiter points nor point file provided") {
        StepDepthParser parser;
        ArgumentHolder ah{"prog"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("Either -sdp or -sdf must be given"));
    }

    SECTION("Points and pointfile provided") {
        StepDepthParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\t2\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-sdp", "0.1,5.2", "-sdf", "testpoints.csv"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-sdf cannot be used together with -sdp"));
    }

    SECTION("Pointfile and points provided") {
        StepDepthParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\t2\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-sdf", "testpoints.csv", "-sdp", "0.1,5.2"};
        REQUIRE_THROWS_WITH(
            parser.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-sdp cannot be used together with -sdf"));
    }

    SECTION("Malformed pointfile") {
        StepDepthParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-sdf", "testpoints.csv"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Error parsing line: 1"));
    }

    SECTION("Malformed point arg") {
        StepDepthParser parser;
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f("testpoints.csv");
            f << "x\ty\n1\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-sdp", "0.1"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Error parsing line: 0.1"));
    }
}

TEST_CASE("StepDepthParserSuccess", "Read successfully") {
    StepDepthParser parser;
    double x1 = 1.0;
    double y1 = 2.0;
    double x2 = 1.1;
    double y2 = 1.2;

    SECTION("Read from commandline") {
        std::stringstream p1;
        p1 << x1 << "," << y1 << std::flush;
        std::stringstream p2;
        p2 << x2 << "," << y2 << std::flush;

        ArgumentHolder ah{"prog", "-sdp", p1.str(), "-sdp", p2.str(), "-sdt", "visual"};
        parser.parse(ah.argc(), ah.argv());
    }

    SECTION("Read from file") {
        SelfCleaningFile scf("testpoints.csv");
        {
            std::ofstream f(scf.Filename().c_str());
            f << "x\ty\n" << x1 << "\t" << y1 << "\n" << x2 << "\t" << y2 << "\n" << std::flush;
        }
        ArgumentHolder ah{"prog", "-sdf", scf.Filename(), "-sdt", "visual"};
        parser.parse(ah.argc(), ah.argv());
    }

    auto points = parser.getStepDepthPoints();
    REQUIRE(points.size() == 2);
    REQUIRE(points[0].x == Catch::Approx(x1));
    REQUIRE(points[0].y == Catch::Approx(y1));
    REQUIRE(points[1].x == Catch::Approx(x2));
    REQUIRE(points[1].y == Catch::Approx(y2));
}
