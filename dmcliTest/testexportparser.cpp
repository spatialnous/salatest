// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"
#include "catch_amalgamated.hpp"

#include "dmcli/exportparser.hpp"

TEST_CASE("ExportParser Fail", "Parsing errors") {
    // missing arguments

    SECTION("Missing argument to -em") {
        ExportParser parser;
        ArgumentHolder ah{"prog", "-em"};
        REQUIRE_THROWS_WITH(parser.parse(ah.argc(), ah.argv()),
                            Catch::Matchers::ContainsSubstring("-em requires an argument"));
    }
}

TEST_CASE("ExportParser Success", "Read successfully") {
    ExportParser parser;

    SECTION("Correctly parse mode latticemap-connections-csv") {
        ArgumentHolder ah{"prog", "-em", "latticemap-connections-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::LATTICEMAP_CONNECTIONS_CSV);
    }

    SECTION("Correctly parse mode latticemap-data-csv") {
        ArgumentHolder ah{"prog", "-em", "latticemap-data-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::LATTICEMAP_DATA_CSV);
    }

    SECTION("Correctly parse mode latticemap-links-csv") {
        ArgumentHolder ah{"prog", "-em", "latticemap-links-csv"};
        parser.parse(ah.argc(), ah.argv());
        REQUIRE(parser.getExportMode() == ExportParser::LATTICEMAP_LINKS_CSV);
    }
}
