// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/isovistutils.h"
#include "salalib/salashape.h"

#include "salalib/genlib/comm.h"
#include "salalib/genlib/line4f.h"

#include "catch_amalgamated.hpp"

TEST_CASE("Simple Isovist") {

    const float epsilon = 0.001f;

    // simple plan for isovist. dot as the origin
    //  _ _ _ _
    // |     . |
    // |    _ _|
    // |   |
    // |_ _|

    std::vector<Line4f> planLines = {
        Line4f(Point2f(1, 1), Point2f(1, 3)), //
        Line4f(Point2f(1, 3), Point2f(3, 3)), //
        Line4f(Point2f(3, 3), Point2f(3, 2)), //
        Line4f(Point2f(3, 2), Point2f(2, 2)), //
        Line4f(Point2f(2, 2), Point2f(2, 1)), //
        Line4f(Point2f(2, 1), Point2f(1, 1))  //
    };

    Point2f isovistOrigin(2.5, 2.5);

    ShapeMap shapeMap("Test ShapeMap");

    for (Line4f &line : planLines) {
        shapeMap.makeLineShape(line);
    }

    ShapeMap isovistMap("Isovists");

    SECTION("With a communicator") {
        std::unique_ptr<Communicator> comm(new ICommunicator);
        IsovistUtils::createIsovistInMap(comm.get(), planLines, shapeMap.getRegion(), isovistMap,
                                         isovistOrigin, 0, 0);
    }
    SECTION("Without a communicator") {
        IsovistUtils::createIsovistInMap(nullptr, planLines, shapeMap.getRegion(), isovistMap,
                                         isovistOrigin, 0, 0);
    }

    SalaShape &isovist = isovistMap.getAllShapes().begin()->second;

    REQUIRE(isovist.isClosed());
    REQUIRE(isovist.isPolygon());

    // TODO: The current implementation generates a polygon of 8 points, potentially
    // because it takes them directly from the isovist gaps. This isovist only really
    // needs 5 points so it might make sense to run some sort of optimisation right
    // after generating the isovists

    REQUIRE(isovist.points.size() == 8);

    size_t i = 0;
    REQUIRE(isovist.points[i].x == Catch::Approx(3.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(3.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(2.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(3.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(1.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(3.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(1.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(1.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(2.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(2.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(2.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(2.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(3.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(2.0).epsilon(epsilon));

    i++;
    REQUIRE(isovist.points[i].x == Catch::Approx(3.0).epsilon(epsilon));
    REQUIRE(isovist.points[i].y == Catch::Approx(2.5).epsilon(epsilon));
}
