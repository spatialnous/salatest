// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "argumentholder.hpp"

#include "catch_amalgamated.hpp"

#include <cstring>

TEST_CASE("Test ArgumentHolder", "Constructor") {
    ArgumentHolder ah{"foo", "bar"};
    REQUIRE(ah.argc() == 2);
    REQUIRE(std::strcmp(ah.argv()[0], "foo") == 0);
    REQUIRE(std::strcmp(ah.argv()[1], "bar") == 0);
}
