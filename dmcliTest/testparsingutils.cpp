// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "dmcli/parsingutils.hpp"

#include <string>
#include <vector>

TEST_CASE("AxialRadiusParsing success") {
    std::string testString = "5,1,n";
    auto result = dmcli::parseRadiusList(testString);
    std::vector<double> expectedResult = {1.0, 5.0, -1.0};
    REQUIRE(result == expectedResult);
}

TEST_CASE("AxialRadiusParsing failure") {
    REQUIRE_THROWS_WITH(dmcli::parseRadiusList("5,1.1"),
                        Catch::Matchers::ContainsSubstring("Found non integer radius 1.1"));
    REQUIRE_THROWS_WITH(
        dmcli::parseRadiusList("5,foo"),
        Catch::Matchers::ContainsSubstring("Found either 0 or unparsable radius foo"));
    REQUIRE_THROWS_WITH(
        dmcli::parseRadiusList("5,0"),
        Catch::Matchers::ContainsSubstring("Found either 0 or unparsable radius 0"));
    REQUIRE_THROWS_WITH(
        dmcli::parseRadiusList("5,-1"),
        Catch::Matchers::ContainsSubstring("Radius must be either n or a positive integer"));
}
