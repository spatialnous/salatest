// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"

#include "dmcli/linkparser.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("LINK args invalid", "") {
    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "LINK", "-lf"};
        LinkParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-lf requires an argument"));
    }

    {
        ArgumentHolder ah{"prog", "-f", "infile", "-o", "outfile", "-m", "LINK", "-lnk"};
        LinkParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-lnk requires an argument"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile",     "-o",  "outfile",   "-m",
                          "LINK", "-lf", "linksfile1", "-lf", "linksfile2"};
        LinkParser p;
        REQUIRE_THROWS_WITH(
            p.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-lf can only be used once at the moment"));
    }

    {
        ArgumentHolder ah{"prog", "-f",  "infile",     "-o",   "outfile", "-m",
                          "LINK", "-lf", "linksfile1", "-lnk", "0,0,0,0"};
        LinkParser p;
        REQUIRE_THROWS_WITH(
            p.parse(ah.argc(), ah.argv()),
            Catch::Matchers::ContainsSubstring("-lf can not be used in conjunction with -lnk"));
    }

    {
        ArgumentHolder ah{"prog", "-f",   "infile", "-o",      "outfile",
                          "-m",   "LINK", "-lnk",   "LaLaLaLa"};
        LinkParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Invalid link provided"));
    }

    {
        ArgumentHolder ah{"prog", "-f",   "infile",         "-o", "outfile", "-m",
                          "LINK", "-lnk", "1.2;3.4;5.6;7.8"};
        LinkParser p;
        REQUIRE_THROWS_WITH(p.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("Invalid link provided"));
    }
}

TEST_CASE("LINK args valid", "valid") {
    {
        ArgumentHolder ah{"prog", "-f",   "infile",         "-o", "outfile", "-m",
                          "LINK", "-lnk", "1.2,3.4,5.6,7.8"};
        LinkParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getManualLinks().size() == 1);
        REQUIRE(cmdP.getManualLinks()[0] == "1.2,3.4,5.6,7.8");
        REQUIRE(cmdP.getLinkMode() == cmdP.LinkMode::LINK);
    }

    {
        ArgumentHolder ah{"prog", "-f",   "infile",          "-o",   "outfile",        "-m",
                          "LINK", "-lnk", "1.2,3.4,5.6,7.8", "-lnk", "0.1,0.2,0.3,0.4"};
        LinkParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getManualLinks().size() == 2);
        REQUIRE(cmdP.getManualLinks()[0] == "1.2,3.4,5.6,7.8");
        REQUIRE(cmdP.getManualLinks()[1] == "0.1,0.2,0.3,0.4");
        REQUIRE(cmdP.getLinkMode() == cmdP.LinkMode::LINK);
    }

    {
        ArgumentHolder ah{"prog",   "-f",   "infile",          "-o",   "outfile",         "-m",
                          "LINK",   "-lnk", "1.2,3.4,5.6,7.8", "-lnk", "0.1,0.2,0.3,0.4", "-lm",
                          "unlink", "-lmt", "latticemaps"};
        LinkParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getManualLinks().size() == 2);
        REQUIRE(cmdP.getManualLinks()[0] == "1.2,3.4,5.6,7.8");
        REQUIRE(cmdP.getManualLinks()[1] == "0.1,0.2,0.3,0.4");
        REQUIRE(cmdP.getLinkMode() == cmdP.LinkMode::UNLINK);
    }
}
