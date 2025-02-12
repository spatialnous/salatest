// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/gridproperties.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("TestGridProperties", "Test the calculations of grid properties") {
    double maxDimension = 4.583;
    GridProperties gp(maxDimension);
    REQUIRE(gp.getDefault() == Catch::Approx(0.04));
    REQUIRE(gp.getMax() == Catch::Approx(0.8));
    REQUIRE(gp.getMin() == Catch::Approx(0.004));
}
