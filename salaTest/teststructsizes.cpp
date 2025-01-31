// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/axialpolygons.h"

#include "catch_amalgamated.hpp"

/**
 * This seems a bit silly, but this is a list of structs that are serialised by just dumping the
 * memory content into a stream, so the size/layout of these must be the same across all platforms
 * to ensure reading writing of graph files.
 */
TEST_CASE("Enforce struct sizes") {
    REQUIRE(sizeof(RadialKey) == 16);
    REQUIRE(sizeof(RadialLine) == 64);
    REQUIRE(sizeof(PolyConnector) == 56);
    REQUIRE(sizeof(Region4f) == 32);
    REQUIRE(sizeof(Line4f) == 40);
}
