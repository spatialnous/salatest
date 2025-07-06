// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dmpcli/radiusconverter.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("ConvertForMetric", "") {
    RadiusConverter converter;
    REQUIRE(converter.ConvertForMetric("2") == Catch::Approx(2.0));
    REQUIRE(converter.ConvertForMetric("n") == Catch::Approx(-1.0));
    REQUIRE(converter.ConvertForMetric("2.6") == Catch::Approx(2.6));
    REQUIRE(converter.ConvertForMetric("2.3e12") == Catch::Approx(2.3e12));
    REQUIRE_THROWS_WITH(
        converter.ConvertForMetric("-1"),
        Catch::Matchers::ContainsSubstring(
            "Radius for metric vga must be n for the whole range or a positive number. Got -1"));
    REQUIRE_THROWS_WITH(
        converter.ConvertForMetric("foo"),
        Catch::Matchers::ContainsSubstring(
            "Radius for metric vga must be n for the whole range or a positive number. Got foo"));
    REQUIRE_THROWS_WITH(converter.ConvertForMetric("NaN"),
                        Catch::Matchers::ContainsSubstring("Radius NaN?! Really?"));
    REQUIRE_THROWS_WITH(converter.ConvertForMetric("INFINITY"),
                        Catch::Matchers::ContainsSubstring("Radius inf?! Who are you kidding?"));
}
