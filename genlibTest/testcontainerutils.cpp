// SPDX-FileCopyrightText: 2018 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "salalib/genlib/containerutils.hpp"

#include <vector>

TEST_CASE("Test binary search helper with container", "") {
    std::vector<int> testVec{1, 2, 4, 5};

    REQUIRE(*genlib::findBinary(testVec, 2) == 2);
    REQUIRE(genlib::findBinary(testVec, 3) == testVec.end());
    REQUIRE(genlib::findBinary(testVec, 6) == testVec.end());
    auto iter = genlib::findBinary(testVec, 2);
    *iter = 3;
    REQUIRE(*genlib::findBinary(testVec, 3) == 3);

    const std::vector<int> &constVec = testVec;
    REQUIRE(*genlib::findBinary(constVec, 3) == 3);
    REQUIRE(genlib::findBinary(constVec, 2) == testVec.end());
    REQUIRE(genlib::findBinary(constVec, 6) == testVec.end());
}
