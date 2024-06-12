// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/geometrygenerators.h"

#include "genlib/p2dpoly.h"

#include "catch.hpp"

TEST_CASE("Test disk triangles generation", "") {
    const float EPSILON = 0.001f;
    size_t sides = 8;
    float radius = 2;

    std::vector<Point2f> expected{Point2f(0, 0),  Point2f(1.41421, 1.41421),
                                  Point2f(0, 2),  Point2f(0, 0),
                                  Point2f(2, 0),  Point2f(1.41421, 1.41421),
                                  Point2f(0, 0),  Point2f(1.41421, -1.41421),
                                  Point2f(2, 0),  Point2f(0, 0),
                                  Point2f(0, -2), Point2f(1.41421, -1.41421),
                                  Point2f(0, 0),  Point2f(-1.41421, -1.41421),
                                  Point2f(0, -2), Point2f(0, 0),
                                  Point2f(-2, 0), Point2f(-1.41421, -1.41421),
                                  Point2f(0, 0),  Point2f(-1.41421, 1.41421),
                                  Point2f(-2, 0), Point2f(0, 0),
                                  Point2f(0, 2),  Point2f(-1.41421, 1.41421)};

    std::vector<Point2f> diskTriangles = GeometryGenerators::generateDiskTriangles(sides, radius);

    REQUIRE(diskTriangles.size() == expected.size());
    for (size_t i = 0; i < diskTriangles.size(); i++) {
        REQUIRE(diskTriangles[i].x == Approx(expected[i].x).epsilon(EPSILON));
        REQUIRE(diskTriangles[i].y == Approx(expected[i].y).epsilon(EPSILON));
    }

    std::vector<Point2f> offsets{Point2f(1, 2), Point2f(3, -4), Point2f(-5, -6), Point2f(-7, 8)};

    std::vector<Point2f> multiDiskTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(sides, radius, offsets);

    REQUIRE(multiDiskTriangles.size() == expected.size() * offsets.size());

    for (size_t i = 0; i < multiDiskTriangles.size(); i++) {
        REQUIRE(multiDiskTriangles[i].x ==
                Approx(expected[i % (sides * 3)].x + offsets[i / (sides * 3)].x).epsilon(EPSILON));
        REQUIRE(multiDiskTriangles[i].y ==
                Approx(expected[i % (sides * 3)].y + offsets[i / (sides * 3)].y).epsilon(EPSILON));
    }
}

TEST_CASE("Test circle perimeter line generation", "") {
    const float EPSILON = 0.001f;
    size_t sides = 8;
    float radius = 2;

    std::vector<SimpleLine> expected{SimpleLine(Point2f(1.41421, 1.41421), Point2f(0, 2)),
                                     SimpleLine(Point2f(2, 0), Point2f(1.41421, 1.41421)),
                                     SimpleLine(Point2f(1.41421, -1.41421), Point2f(2, 0)),
                                     SimpleLine(Point2f(0, -2), Point2f(1.41421, -1.41421)),
                                     SimpleLine(Point2f(-1.41421, -1.41421), Point2f(0, -2)),
                                     SimpleLine(Point2f(-2, 0), Point2f(-1.41421, -1.41421)),
                                     SimpleLine(Point2f(-1.41421, 1.41421), Point2f(-2, 0)),
                                     SimpleLine(Point2f(0, 2), Point2f(-1.41421, 1.41421))};

    std::vector<SimpleLine> circleLines = GeometryGenerators::generateCircleLines(sides, radius);

    REQUIRE(circleLines.size() == expected.size());
    for (size_t i = 0; i < circleLines.size(); i++) {
        REQUIRE(circleLines[i].start().x == Approx(expected[i].start().x).epsilon(EPSILON));
        REQUIRE(circleLines[i].start().y == Approx(expected[i].start().y).epsilon(EPSILON));
        REQUIRE(circleLines[i].end().x == Approx(expected[i].end().x).epsilon(EPSILON));
        REQUIRE(circleLines[i].end().y == Approx(expected[i].end().y).epsilon(EPSILON));
    }

    std::vector<Point2f> offsets{Point2f(1, 2), Point2f(3, -4), Point2f(-5, -6), Point2f(-7, 8)};

    std::vector<SimpleLine> multiCircleLines =
        GeometryGenerators::generateMultipleCircleLines(sides, radius, offsets);

    REQUIRE(multiCircleLines.size() == expected.size() * offsets.size());

    for (size_t i = 0; i < multiCircleLines.size(); i++) {
        REQUIRE(multiCircleLines[i].start().x ==
                Approx(expected[i % sides].start().x + offsets[i / sides].x).epsilon(EPSILON));
        REQUIRE(multiCircleLines[i].start().y ==
                Approx(expected[i % sides].start().y + offsets[i / sides].y).epsilon(EPSILON));
        REQUIRE(multiCircleLines[i].end().x ==
                Approx(expected[i % sides].end().x + offsets[i / sides].x).epsilon(EPSILON));
        REQUIRE(multiCircleLines[i].end().y ==
                Approx(expected[i % sides].end().y + offsets[i / sides].y).epsilon(EPSILON));
    }
}
