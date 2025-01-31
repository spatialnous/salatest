// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/sparksieve2.h"

#include "catch_amalgamated.hpp"

#include <map>

TEST_CASE("One block garbage") {
    Point2f centre(1, 1);
    sparkSieve2 sieve(centre);
    std::vector<Line4f> lines;
    // these lines get turned into "blocks" based by a tanify function based on q and the centre
    // given above. Given q=4 and centre 1,1 this line will be from 0.625 to something bigger than 1
    lines.push_back(Line4f(Point2f(0.5, 0.2), Point2f(0.5, 0.7)));
    sieve.block(lines, 4);
    sieve.collectgarbage();
    REQUIRE(sieve.gaps.size() == 1);
    REQUIRE(sieve.gaps.begin()->start == 0);
    REQUIRE(sieve.gaps.begin()->end == Catch::Approx(0.625));
}

TEST_CASE("Shift start and end") {
    Point2f centre(1, 1);
    sparkSieve2 sieve(centre);
    std::vector<Line4f> lines;
    // .625 -> > 1
    lines.push_back(Line4f(Point2f(0.5, 0.2), Point2f(0.5, 0.7)));
    // < 0 -> 0.55555557
    lines.push_back(Line4f(Point2f(0.5, 0.1), Point2f(1.1, 0.9)));
    sieve.block(lines, 4);
    sieve.collectgarbage();
    REQUIRE(sieve.gaps.size() == 1);
    REQUIRE(sieve.gaps.begin()->start == Catch::Approx(0.55555555555));
    REQUIRE(sieve.gaps.begin()->end == Catch::Approx(0.625));
}

TEST_CASE("delete gap") {
    Point2f centre(1, 1);
    sparkSieve2 sieve(centre);
    std::vector<Line4f> lines;
    // < 0 -> > 1 the block covers the whole gap
    lines.push_back(Line4f(Point2f(1.1, 0.2), Point2f(0.5, 0.7)));
    sieve.block(lines, 4);
    sieve.collectgarbage();
    REQUIRE(sieve.gaps.empty());
}

TEST_CASE("add gap") {
    Point2f centre(1, 1);
    sparkSieve2 sieve(centre);
    std::vector<Line4f> lines;
    // 0.55555 -> .625 the block splits the gap
    lines.push_back(Line4f(Point2f(0.5, 0.2), Point2f(0.5, 0.1)));
    //  0.71428571 -> > 1
    lines.push_back(Line4f(Point2f(0.5, 0.3), Point2f(0.5, 0.7)));
    sieve.block(lines, 4);
    sieve.collectgarbage();
    REQUIRE(sieve.gaps.size() == 2);
    auto iter = sieve.gaps.begin();
    REQUIRE(iter->start == 0);
    REQUIRE(iter->end == Catch::Approx(0.55555555555));
    iter++;
    REQUIRE(iter->start == Catch::Approx(0.625));
    REQUIRE(iter->end == Catch::Approx(0.71428571));
}
