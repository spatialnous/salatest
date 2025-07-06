// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"

#include "dmpcli/vgaparser.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("VGA args invalid", "") {
    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "VGA", "-vm"};
        VgaParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-vm requires an argument"));
    }

    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "VGA", "-vm", "foo"};
        VgaParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Invalid VGA mode: foo"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile",     "-o",  "outfile", "-m",
                          "VGA",  "-vm", "visibility", "-vm", "metric"};
        VgaParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-vm can only be used once"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile", "-o",         "outfile",
                          "-m",   "VGA", "-vm",    "visibility", "-vg"};
        VgaParser p;
        REQUIRE_THROWS_WITH(
            p.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring(
                "Global measures in VGA/visibility analysis require a radius, use -vr <radius>"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile",     "-o",  "outfile", "-m",
                          "VGA",  "-vm", "visibility", "-vg", "-vr"};
        VgaParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-vr requires an argument"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile",     "-o",  "outfile", "-m",
                          "VGA",  "-vm", "visibility", "-vg", "-vr",     "foo"};
        VgaParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring(
                                "Radius must be a positive integer number or n, got foo"));
    }

    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "VGA", "-vm", "metric"};
        VgaParser p;
        REQUIRE_THROWS_WITH(
            p.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("Metric vga requires a radius, use -vr <radius>"));
    }
}

TEST_CASE("VGA args valid", "valid") {
    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "VGA"};
        VgaParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getVgaMode() == VgaParser::VgaMode::ISOVIST);
    }

    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "VGA", "-vm", "isovist"};
        VgaParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getVgaMode() == VgaParser::VgaMode::ISOVIST);
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile", "-o",        "outfile",
                          "-m",   "VGA", "-vm",    "visibility"};
        VgaParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getVgaMode() == VgaParser::VgaMode::VISBILITY);
        REQUIRE_FALSE(cmdP.localMeasures());
        REQUIRE_FALSE(cmdP.globalMeasures());
        REQUIRE(cmdP.getRadius().empty());
    }

    {
        ArgumentHolder ah{"prog", "-f",         "infile", "-o",  "outfile", "-m", "VGA",
                          "-vm",  "visibility", "-vl",    "-vg", "-vr",     "4"};
        VgaParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getVgaMode() == VgaParser::VgaMode::VISBILITY);
        REQUIRE(cmdP.globalMeasures());
        REQUIRE(cmdP.localMeasures());
        REQUIRE(cmdP.getRadius() == "4");
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile", "-o",        "outfile",
                          "-m",   "VGA", "-vm",    "thruvision"};
        VgaParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getVgaMode() == VgaParser::VgaMode::THRU_VISION);
    }
}
