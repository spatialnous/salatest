// SPDX-FileCopyrightText: 2018 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/genlib/containerutils.hpp"

#include "catch_amalgamated.hpp"

#include <vector>

TEST_CASE("Test binary search helper with container", "") {
    std::vector<int> testVec{1, 2, 4, 5};

    REQUIRE(*depthmapX::findBinary(testVec, 2) == 2);
    REQUIRE(depthmapX::findBinary(testVec, 3) == testVec.end());
    REQUIRE(depthmapX::findBinary(testVec, 6) == testVec.end());
    auto iter = depthmapX::findBinary(testVec, 2);
    *iter = 3;
    REQUIRE(*depthmapX::findBinary(testVec, 3) == 3);

    const std::vector<int> &constVec = testVec;
    REQUIRE(*depthmapX::findBinary(constVec, 3) == 3);
    REQUIRE(depthmapX::findBinary(constVec, 2) == testVec.end());
    REQUIRE(depthmapX::findBinary(constVec, 6) == testVec.end());
}
