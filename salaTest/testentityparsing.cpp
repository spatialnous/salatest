// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "salalib/entityparsing.hpp"
#include "salalib/genlib/line4f.hpp"

#include <math.h>
#include <ostream>
#include <sstream>
#include <vector>

TEST_CASE("Failing line parser", "") {
    {
        // header only has 3 elements
        std::stringstream stream;
        stream << "x1,y1,x2" << std::endl;
        REQUIRE_THROWS_WITH(EntityParsing::parseLines(stream, ','),
                            Catch::Matchers::ContainsSubstring(
                                "Badly formatted header (should contain x1, y1, x2 and y2)"));
    }

    {
        // header has y1 twice instead of y2
        std::stringstream stream;
        stream << "x1,y1,x2,y1" << std::endl;
        REQUIRE_THROWS_WITH(EntityParsing::parseLines(stream, ','),
                            Catch::Matchers::ContainsSubstring(
                                "Badly formatted header (should contain x1, y1, x2 and y2)"));
    }

    {
        // error parsing line
        std::stringstream stream;
        stream << "x1,y1,x2,y2" << std::endl;
        stream << "1.2,3.4,5.6" << std::endl;
        REQUIRE_THROWS_WITH(EntityParsing::parseLines(stream, ','),
                            Catch::Matchers::ContainsSubstring("Error parsing line"));
    }
}
TEST_CASE("Successful line parser", "") {
    const float epsilon = 0.001f;
    {
        std::stringstream stream;
        stream << "x1,y1,x2,y2" << std::endl;
        stream << "1.2,3.4,5.6,7.8" << std::endl;
        std::vector<Line4f> lines = EntityParsing::parseLines(stream, ',');
        REQUIRE(lines.size() == 1);
        REQUIRE(lines[0].start().x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(lines[0].start().y == Catch::Approx(3.4).epsilon(epsilon));
        REQUIRE(lines[0].end().x == Catch::Approx(5.6).epsilon(epsilon));
        REQUIRE(lines[0].end().y == Catch::Approx(7.8).epsilon(epsilon));
    }

    {
        std::stringstream stream;
        stream << "x1\ty1\tx2\ty2" << std::endl;
        stream << "1.2\t3.4\t5.6\t7.8" << std::endl;
        std::vector<Line4f> lines = EntityParsing::parseLines(stream, '\t');
        REQUIRE(lines.size() == 1);
        REQUIRE(lines[0].start().x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(lines[0].start().y == Catch::Approx(3.4).epsilon(epsilon));
        REQUIRE(lines[0].end().x == Catch::Approx(5.6).epsilon(epsilon));
        REQUIRE(lines[0].end().y == Catch::Approx(7.8).epsilon(epsilon));
    }

    {
        std::stringstream stream;
        stream << "x1\ty1\tx2\ty2" << std::endl;
        stream << "1.2\t3.4\t5.6\t7.8" << std::endl;
        stream << "0.1\t0.2\t0.3\t0.4" << std::endl;
        std::vector<Line4f> lines = EntityParsing::parseLines(stream, '\t');
        REQUIRE(lines.size() == 2);
        REQUIRE(lines[0].start().x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(lines[0].start().y == Catch::Approx(3.4).epsilon(epsilon));
        REQUIRE(lines[0].end().x == Catch::Approx(5.6).epsilon(epsilon));
        REQUIRE(lines[0].end().y == Catch::Approx(7.8).epsilon(epsilon));
        REQUIRE(lines[1].start().x == Catch::Approx(0.1).epsilon(epsilon));
        REQUIRE(lines[1].start().y == Catch::Approx(0.2).epsilon(epsilon));
        REQUIRE(lines[1].end().x == Catch::Approx(0.3).epsilon(epsilon));
        REQUIRE(lines[1].end().y == Catch::Approx(0.4).epsilon(epsilon));
    }
}

TEST_CASE("Failing point parser", "") {
    {
        // header only has 3 elements
        std::stringstream stream;
        stream << "x" << std::endl;
        REQUIRE_THROWS_WITH(
            EntityParsing::parsePoints(stream, ','),
            Catch::Matchers::ContainsSubstring("Badly formatted header (should contain x and y)"));
    }

    {
        // header has y1 twice instead of y2
        std::stringstream stream;
        stream << "x,x" << std::endl;
        REQUIRE_THROWS_WITH(
            EntityParsing::parsePoints(stream, ','),
            Catch::Matchers::ContainsSubstring("Badly formatted header (should contain x and y)"));
    }

    {
        // error parsing line
        std::stringstream stream;
        stream << "x,y" << std::endl;
        stream << "1.2" << std::endl;
        REQUIRE_THROWS_WITH(EntityParsing::parsePoints(stream, ','),
                            Catch::Matchers::ContainsSubstring("Error parsing line"));
    }
}
TEST_CASE("Successful point parser", "") {
    const float epsilon = 0.001f;
    {
        std::stringstream stream;
        stream << "x,y" << std::endl;
        stream << "1.2,3.4" << std::endl;
        std::vector<Point2f> points = EntityParsing::parsePoints(stream, ',');
        REQUIRE(points.size() == 1);
        REQUIRE(points[0].x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(points[0].y == Catch::Approx(3.4).epsilon(epsilon));
    }

    {
        std::stringstream stream;
        stream << "x\ty" << std::endl;
        stream << "1.2\t3.4" << std::endl;
        std::vector<Point2f> points = EntityParsing::parsePoints(stream, '\t');
        REQUIRE(points.size() == 1);
        REQUIRE(points[0].x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(points[0].y == Catch::Approx(3.4).epsilon(epsilon));
    }

    {
        std::stringstream stream;
        stream << "x\ty" << std::endl;
        stream << "1.2\t3.4" << std::endl;
        stream << "0.1\t0.2" << std::endl;
        std::vector<Point2f> points = EntityParsing::parsePoints(stream, '\t');
        REQUIRE(points.size() == 2);
        REQUIRE(points[0].x == Catch::Approx(1.2).epsilon(epsilon));
        REQUIRE(points[0].y == Catch::Approx(3.4).epsilon(epsilon));
        REQUIRE(points[1].x == Catch::Approx(0.1).epsilon(epsilon));
        REQUIRE(points[1].y == Catch::Approx(0.2).epsilon(epsilon));
    }
}

TEST_CASE("Test point parsing") {
    REQUIRE_THROWS_WITH(EntityParsing::parsePoint("foo", '|'),
                        Catch::Matchers::ContainsSubstring(
                            "Badly formatted point data, should be <number>|<number>, was foo"));
    auto point = EntityParsing::parsePoint("1.235|27.25", '|');
    REQUIRE(point.x == Catch::Approx(1.235));
    REQUIRE(point.y == Catch::Approx(27.25));

    point = EntityParsing::parsePoint("1.235|bar", '|');
    REQUIRE(point.x == Catch::Approx(1.235));
    REQUIRE(point.y == 0.0);
}

TEST_CASE("Successful Isovist parser") {
    const float epsilon = 0.0001f;
    {
        std::stringstream stream;
        stream << "x,y\n1.0,2.34\n0.5,9.2\n" << std::flush;
        auto result = EntityParsing::parseIsovists(stream, ',');
        REQUIRE(result.size() == 2);
        REQUIRE(result[0].getLocation().x == Catch::Approx(1.0).epsilon(epsilon));
        REQUIRE(result[0].getLocation().y == Catch::Approx(2.34).epsilon(epsilon));
        REQUIRE(result[0].getAngle() == Catch::Approx(0.0).epsilon(epsilon));
        REQUIRE(result[0].getViewAngle() == 0.0);
        REQUIRE(result[1].getLocation().x == Catch::Approx(0.5).epsilon(epsilon));
        REQUIRE(result[1].getLocation().y == Catch::Approx(9.2).epsilon(epsilon));
        REQUIRE(result[1].getAngle() == Catch::Approx(0.0).epsilon(epsilon));
        REQUIRE(result[1].getViewAngle() == 0.0);
    }
    {
        std::stringstream stream;
        stream << "x,y,angle,viewAngle\n1.0,2.34,90,90\n0.5,9.2,180,270\n" << std::flush;
        auto result = EntityParsing::parseIsovists(stream, ',');
        REQUIRE(result.size() == 2);
        REQUIRE(result[0].getLocation().x == Catch::Approx(1.0).epsilon(epsilon));
        REQUIRE(result[0].getLocation().y == Catch::Approx(2.34).epsilon(epsilon));
        REQUIRE(result[0].getAngle() == Catch::Approx(M_PI / 2.0).epsilon(epsilon));
        REQUIRE(result[0].getViewAngle() == Catch::Approx(M_PI / 2.0).epsilon(epsilon));
        REQUIRE(result[1].getLocation().x == Catch::Approx(0.5).epsilon(epsilon));
        REQUIRE(result[1].getLocation().y == Catch::Approx(9.2).epsilon(epsilon));
        REQUIRE(result[1].getAngle() == Catch::Approx(M_PI).epsilon(epsilon));
        REQUIRE(result[1].getViewAngle() == Catch::Approx(M_PI * 1.5).epsilon(epsilon));
    }
}

TEST_CASE("Failing Isovist parser") {
    {
        std::stringstream stream;
        stream << "x,angle,viewAngle\n" << std::flush;
        REQUIRE_THROWS_WITH(EntityParsing::parseIsovists(stream, ','),
                            Catch::Matchers::ContainsSubstring(
                                "Badly formatted header (should contain x and y, might "
                                "also have angle and viewangle for partial isovists)"));
    }

    {
        std::stringstream stream;
        stream << "x,y,angle,viewAngle\n1.0,1.0,270" << std::flush;
        REQUIRE_THROWS_WITH(EntityParsing::parseIsovists(stream, ','),
                            Catch::Matchers::ContainsSubstring("Error parsing line: 1.0,1.0,270"));
    }
}

TEST_CASE("Parsing single isovist") {
    SECTION("Success full") {
        auto result = EntityParsing::parseIsovist("1,1");
        REQUIRE(result.getLocation().x == Catch::Approx(1.0));
        REQUIRE(result.getLocation().y == Catch::Approx(1.0));
        REQUIRE(result.getAngle() == 0.0);
        REQUIRE(result.getViewAngle() == 0.0);
    }

    SECTION("Success partial isovist") {
        auto result = EntityParsing::parseIsovist("1,1,27,90");
        REQUIRE(result.getLocation().x == Catch::Approx(1.0));
        REQUIRE(result.getLocation().y == Catch::Approx(1.0));
        REQUIRE(result.getAngle() == Catch::Approx(0.4712388));
        REQUIRE(result.getViewAngle() == Catch::Approx(M_PI / 2.0));
    }

    SECTION("Failed bad string") {
        REQUIRE_THROWS_WITH(EntityParsing::parseIsovist("1,1,27"),
                            Catch::Matchers::ContainsSubstring(
                                "Failed to parse '1,1,27' to an isovist definition"));
    }
}
