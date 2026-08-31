// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/geometrygenerators.hpp"

#include "salalib/genlib/point2f.hpp"

#include "catch_amalgamated.hpp"

#include <cstddef>
#include <vector>

TEST_CASE("Test disk triangles generation", "") {
    const float epsilon = 0.001f;
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
        REQUIRE_THAT(diskTriangles[i].x,
                     Catch::Matchers::WithinAbs(static_cast<float>(expected[i].x), epsilon));
        REQUIRE_THAT(diskTriangles[i].y,
                     Catch::Matchers::WithinAbs(static_cast<float>(expected[i].y), epsilon));
    }

    std::vector<Point2f> offsets{Point2f(1, 2), Point2f(3, -4), Point2f(-5, -6), Point2f(-7, 8)};

    std::vector<Point2f> multiDiskTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(sides, radius, offsets);

    REQUIRE(multiDiskTriangles.size() == expected.size() * offsets.size());

    for (size_t i = 0; i < multiDiskTriangles.size(); i++) {
        REQUIRE(multiDiskTriangles[i].x ==
                Catch::Approx(expected[i % (sides * 3)].x + offsets[i / (sides * 3)].x)
                    .epsilon(epsilon));
        REQUIRE(multiDiskTriangles[i].y ==
                Catch::Approx(expected[i % (sides * 3)].y + offsets[i / (sides * 3)].y)
                    .epsilon(epsilon));
    }
}

TEST_CASE("Test circle perimeter line generation", "") {
    const float epsilon = 0.001f;
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
        REQUIRE_THAT(
            circleLines[i].start().x,
            Catch::Matchers::WithinAbs(static_cast<float>(expected[i].start().x), epsilon));
        REQUIRE_THAT(
            circleLines[i].start().y,
            Catch::Matchers::WithinAbs(static_cast<float>(expected[i].start().y), epsilon));
        REQUIRE_THAT(circleLines[i].end().x,
                     Catch::Matchers::WithinAbs(static_cast<float>(expected[i].end().x), epsilon));
        REQUIRE_THAT(circleLines[i].end().y,
                     Catch::Matchers::WithinAbs(static_cast<float>(expected[i].end().y), epsilon));
    }

    std::vector<Point2f> offsets{Point2f(1, 2), Point2f(3, -4), Point2f(-5, -6), Point2f(-7, 8)};

    std::vector<SimpleLine> multiCircleLines =
        GeometryGenerators::generateMultipleCircleLines(sides, radius, offsets);

    REQUIRE(multiCircleLines.size() == expected.size() * offsets.size());

    for (size_t i = 0; i < multiCircleLines.size(); i++) {
        REQUIRE(
            multiCircleLines[i].start().x ==
            Catch::Approx(expected[i % sides].start().x + offsets[i / sides].x).epsilon(epsilon));
        REQUIRE(
            multiCircleLines[i].start().y ==
            Catch::Approx(expected[i % sides].start().y + offsets[i / sides].y).epsilon(epsilon));
        REQUIRE(multiCircleLines[i].end().x ==
                Catch::Approx(expected[i % sides].end().x + offsets[i / sides].x).epsilon(epsilon));
        REQUIRE(multiCircleLines[i].end().y ==
                Catch::Approx(expected[i % sides].end().y + offsets[i / sides].y).epsilon(epsilon));
    }
}
