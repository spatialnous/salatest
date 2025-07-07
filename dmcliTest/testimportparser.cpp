// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"

#include "dmcli/importparser.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("Import args valid", "valid") {
    {
        ArgumentHolder ah{"prog", "-f",     "infile", "-o",        "outfile",
                          "-m",   "IMPORT", "-if",    "importfile"};
        ImportParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getFilesToImport().size() == 1);
        REQUIRE(cmdP.getFilesToImport()[0] == "importfile");
    }
    {
        ArgumentHolder ah{"prog",   "-f",  "infile",      "-o",  "outfile",    "-m",
                          "IMPORT", "-if", "importfile1", "-if", "importfile2"};
        ImportParser cmdP;
        cmdP.parse(ah.argc(), ah.argv());
        REQUIRE(cmdP.getFilesToImport().size() == 2);
        REQUIRE(cmdP.getFilesToImport()[0] == "importfile1");
        REQUIRE(cmdP.getFilesToImport()[1] == "importfile2");
    }
}
