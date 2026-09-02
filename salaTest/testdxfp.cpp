// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "salalib/genlib/comm.hpp"
#include "salalib/genlib/point2f.hpp"
#include "salalib/parsers/dxfp.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

TEST_CASE("DXF Parsing (lines)") {
    const float epsilon = 0.001f;
    Point2f lineStart(-1, -2);
    Point2f lineEnd(3, 4);
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << lineStart.x << "\n"
           << "20\n"
           << lineStart.y << "\n"
           << "30\n0\n"
           << "11\n"
           << lineEnd.x << "\n"
           << "21\n"
           << lineEnd.y << "\n"
           << "31\n0\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numLines() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().x ==
            Catch::Approx(lineStart.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().y ==
            Catch::Approx(lineStart.y).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().x ==
            Catch::Approx(lineEnd.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().y ==
            Catch::Approx(lineEnd.y).epsilon(epsilon));
}

TEST_CASE("DXF Parsing (arcs)") {
    const float epsilon = 0.001f;
    Point2f centre(1, -2);
    float radius = 3;
    float startAngle = 45;
    float endAngle = 67;
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nARC\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << centre.x << "\n"
           << "20\n"
           << centre.y << "\n"
           << "30\n0\n"
           << "40\n"
           << radius << "\n"
           << "50\n"
           << startAngle << "\n"
           << "51\n"
           << endAngle << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numArcs() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getArc(0).getCentre().x ==
            Catch::Approx(centre.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getArc(0).getCentre().y ==
            Catch::Approx(centre.y).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getArc(0).getRadius() ==
            Catch::Approx(radius).epsilon(epsilon));
    // arc start angle not publicly accessible
    // arc end angle not publicly accessible
}

TEST_CASE("DXF Parsing (circles)") {
    const float epsilon = 0.001f;
    Point2f centre(1, -2);
    float radius = 3;
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nCIRCLE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << centre.x << "\n"
           << "20\n"
           << centre.y << "\n"
           << "30\n0\n"
           << "40\n"
           << radius << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numCircles() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getCircle(0).getCentre().x ==
            Catch::Approx(centre.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getCircle(0).getCentre().y ==
            Catch::Approx(centre.y).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getCircle(0).getRadius() ==
            Catch::Approx(radius).epsilon(epsilon));
}

TEST_CASE("DXF Parsing (points)") {
    const float epsilon = 0.001f;
    Point2f point(1, -2);
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nPOINT\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << point.x << "\n"
           << "20\n"
           << point.y << "\n"
           << "30\n0\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numPoints() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getPoint(0).x ==
            Catch::Approx(point.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getPoint(0).y ==
            Catch::Approx(point.y).epsilon(epsilon));
}

TEST_CASE("DXF Parsing (lwpolyline)") {
    const float epsilon = 0.001f;
    Point2f point1(-1, -2);
    Point2f point2(3, 4);
    Point2f point3(-5, 6);
    Point2f point4(7, -8);
    std::string layer("0");

    int closed = 0;

    SECTION("open polyline") { closed = 0; }
    SECTION("closed polyline") { closed = 1; }

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nLWPOLYLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << point1.x << "\n"
           << "20\n"
           << point1.y << "\n"
           << "30\n0\n"
           << "10\n"
           << point2.x << "\n"
           << "20\n"
           << point2.y << "\n"
           << "30\n0\n"
           << "10\n"
           << point3.x << "\n"
           << "20\n"
           << point3.y << "\n"
           << "30\n0\n"
           << "10\n"
           << point4.x << "\n"
           << "20\n"
           << point4.y << "\n"
           << "30\n0\n"
           << "70\n"
           << closed << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numPolyLines() == 1);
    DxfPolyLine polyline = dxfParser.getLayer(layer.c_str())->getPolyLine(0);
    REQUIRE(polyline.numVertices() == 4);
    REQUIRE(polyline.getVertex(0).x == Catch::Approx(point1.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(0).y == Catch::Approx(point1.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).x == Catch::Approx(point2.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).y == Catch::Approx(point2.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(2).x == Catch::Approx(point3.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(2).y == Catch::Approx(point3.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(3).x == Catch::Approx(point4.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(3).y == Catch::Approx(point4.y).epsilon(epsilon));
    REQUIRE((polyline.getAttributes() & polyline.CLOSED) == closed);
}

TEST_CASE("DXF Parsing (polyline)") {
    const float epsilon = 0.001f;
    Point2f point1(-1, -2);
    Point2f point2(3, 4);
    Point2f point3(-5, 6);
    Point2f point4(7, -8);
    std::string layer("0");

    int closed = 0;

    SECTION("open polyline") { closed = 0; }
    SECTION("closed polyline") { closed = 1; }

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nPOLYLINE\n"
           << "8\n"
           << layer << "\n"
           << "70\n"
           << closed << "\n"
           << "0\nVERTEX\n"
           << "10\n"
           << point1.x << "\n"
           << "20\n"
           << point1.y << "\n"
           << "30\n0\n"
           << "0\nVERTEX\n"
           << "10\n"
           << point2.x << "\n"
           << "20\n"
           << point2.y << "\n"
           << "30\n0\n"
           << "0\nVERTEX\n"
           << "10\n"
           << point3.x << "\n"
           << "20\n"
           << point3.y << "\n"
           << "30\n0\n"
           << "0\nVERTEX\n"
           << "10\n"
           << point4.x << "\n"
           << "20\n"
           << point4.y << "\n"
           << "30\n0\n"
           << "0\nSEQEND\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numPolyLines() == 1);
    DxfPolyLine polyline = dxfParser.getLayer(layer.c_str())->getPolyLine(0);
    REQUIRE(polyline.numVertices() == 4);
    REQUIRE(polyline.getVertex(0).x == Catch::Approx(point1.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(0).y == Catch::Approx(point1.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).x == Catch::Approx(point2.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).y == Catch::Approx(point2.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(2).x == Catch::Approx(point3.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(2).y == Catch::Approx(point3.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(3).x == Catch::Approx(point4.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(3).y == Catch::Approx(point4.y).epsilon(epsilon));
    REQUIRE((polyline.getAttributes() & polyline.CLOSED) == closed);
}

TEST_CASE("DXF Parsing (spline)") {
    const float epsilon = 0.001f;

    std::vector<double> weights = {0.0, 0.0, 0.0, 0.0, 2.72, 5.15, 6.93, 6.93, 6.93, 6.93};

    std::vector<Point2f> controlPoints = {Point2f(32.80, 27.09), Point2f(31.58, 26.71),
                                          Point2f(29.28, 25.98), Point2f(30.69, 30.55),
                                          Point2f(31.35, 28.67), Point2f(31.64, 27.87)};

    std::vector<Point2f> fitPoints = {
        Point2f(32.80, 27.09),
        Point2f(30.08, 27.09),
        Point2f(30.75, 29.42),
        Point2f(31.64, 27.87),
    };
    std::string layer("0");

    int closed = 0;

    SECTION("open spline") { closed = 0; }
    SECTION("closed spline") { closed = 1; }

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nSPLINE\n"
           << "8\n"
           << layer << "\n"
           << "70\n"
           << closed << "\n"
           << "72\n"
           << (controlPoints.size() + fitPoints.size()) << "\n"
           << "73\n"
           << controlPoints.size() << "\n";

    std::vector<double>::iterator weightsIter = weights.begin(), weightsEnd = weights.end();
    for (; weightsIter != weightsEnd; ++weightsIter) {
        stream << "40\n" << *weightsIter << "\n";
    }

    std::vector<Point2f>::iterator cPointsIter = controlPoints.begin(),
                                   cPointsEnd = controlPoints.end();
    for (; cPointsIter != cPointsEnd; ++cPointsIter) {
        Point2f point = *cPointsIter;
        stream << "10\n"
               << point.x << "\n"
               << "20\n"
               << point.y << "\n"
               << "30\n0\n";
    }

    std::vector<Point2f>::iterator fPointsIter = fitPoints.begin(), fPointsEnd = fitPoints.end();
    for (; fPointsIter != fPointsEnd; ++fPointsIter) {
        Point2f point = *fPointsIter;
        stream << "11\n"
               << point.x << "\n"
               << "21\n"
               << point.y << "\n"
               << "31\n0\n";
    }

    stream << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numSplines() == 1);
    DxfSpline spline = dxfParser.getLayer(layer.c_str())->getSpline(0);
    REQUIRE(spline.numVertices() == controlPoints.size());
    for (size_t i = 0; i < controlPoints.size(); i++) {
        REQUIRE(spline.getVertex(i).x == Catch::Approx(controlPoints[i].x).epsilon(epsilon));
        REQUIRE(spline.getVertex(i).y == Catch::Approx(controlPoints[i].y).epsilon(epsilon));
    }
    REQUIRE((spline.getAttributes() & spline.CLOSED) == closed);
}
TEST_CASE("DXF Parsing (zero-length line)") {
    // parser skips zero-length lines

    Point2f lineStart(1, 2);
    Point2f lineEnd(1, 2);
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << lineStart.x << "\n"
           << "20\n"
           << lineStart.y << "\n"
           << "30\n0\n"
           << "11\n"
           << lineEnd.x << "\n"
           << "21\n"
           << lineEnd.y << "\n"
           << "31\n0\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numLines() == 0);
}

TEST_CASE("DXF Parsing (zero-length lwpolyline)") {
    // parser does not skip zero-length polylines

    const float epsilon = 0.001f;
    Point2f point1(1, 2);
    Point2f point2(1, 2);
    std::string layer("0");

    int closed = 0;

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nLWPOLYLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << point1.x << "\n"
           << "20\n"
           << point1.y << "\n"
           << "30\n0\n"
           << "10\n"
           << point2.x << "\n"
           << "20\n"
           << point2.y << "\n"
           << "30\n0\n"
           << "70\n"
           << closed << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numPolyLines() == 1);
    DxfPolyLine polyline = dxfParser.getLayer(layer.c_str())->getPolyLine(0);
    REQUIRE(polyline.numVertices() == 2);
    REQUIRE(polyline.getVertex(0).x == Catch::Approx(point1.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(0).y == Catch::Approx(point1.y).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).x == Catch::Approx(point2.x).epsilon(epsilon));
    REQUIRE(polyline.getVertex(1).y == Catch::Approx(point2.y).epsilon(epsilon));
    REQUIRE((polyline.getAttributes() & polyline.CLOSED) == closed);
}

TEST_CASE("DXF Parsing (block)") {
    const float epsilon = 0.001f;
    Point2f lineStart(-1, -2);
    Point2f lineEnd(3, 4);
    std::string block("bl");
    Point2f blockTranslation(5, -6);
    Point2f blockScale(1, 1);
    double blockRotation = 0;
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nBLOCKS\n"
           << "0\nBLOCK\n"
           << "2\n"
           << block << "\n"
           << "8\n"
           << layer << "\n"
           << "0\nLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << lineStart.x << "\n"
           << "20\n"
           << lineStart.y << "\n"
           << "30\n0\n"
           << "11\n"
           << lineEnd.x << "\n"
           << "21\n"
           << lineEnd.y << "\n"
           << "31\n0\n"
           << "0\nENDBLK\n"
           << "0\nENDSEC\n"
           << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nINSERT\n"
           << "8\n"
           << layer << "\n"
           << "2\n"
           << block << "\n"
           << "10\n"
           << blockTranslation.x << "\n"
           << "20\n"
           << blockTranslation.y << "\n"
           << "30\n"
           << 0 << "\n"
           << "41\n"
           << blockScale.x << "\n"
           << "42\n"
           << blockScale.y << "\n"
           << "43\n"
           << 0 << "\n"
           << "50\n"
           << blockRotation << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numLines() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().x ==
            Catch::Approx(lineStart.x + blockTranslation.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().y ==
            Catch::Approx(lineStart.y + blockTranslation.y).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().x ==
            Catch::Approx(lineEnd.x + blockTranslation.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().y ==
            Catch::Approx(lineEnd.y + blockTranslation.y).epsilon(epsilon));
}

TEST_CASE("DXF Parsing (deeper blocks)") {
    const float epsilon = 0.001f;
    Point2f lineStart(-1, -2);
    Point2f lineEnd(3, 4);
    std::string block("bl");
    std::string blockInternal("bli");
    Point2f blockTranslation(5, -6);
    Point2f blockScale(1, 1);
    double blockRotation = 0;
    std::string layer("0");

    std::stringstream stream;

    stream << "0\nSECTION\n"
           << "2\nBLOCKS\n"
           << "0\nBLOCK\n"
           << "2\n"
           << block << "\n"
           << "8\n"
           << layer << "\n"

           << "0\nINSERT\n"
           << "8\n"
           << layer << "\n"
           << "2\n"
           << blockInternal << "\n"
           << "10\n"
           << blockTranslation.x << "\n"
           << "20\n"
           << blockTranslation.y << "\n"
           << "30\n"
           << 0 << "\n"
           << "41\n"
           << blockScale.x << "\n"
           << "42\n"
           << blockScale.y << "\n"
           << "43\n"
           << 0 << "\n"
           << "50\n"
           << blockRotation << "\n"

           << "0\nENDBLK\n"
           << "0\nBLOCK\n"
           << "2\n"
           << blockInternal << "\n"
           << "8\n"
           << layer << "\n"
           << "0\nLINE\n"
           << "8\n"
           << layer << "\n"
           << "10\n"
           << lineStart.x << "\n"
           << "20\n"
           << lineStart.y << "\n"
           << "30\n0\n"
           << "11\n"
           << lineEnd.x << "\n"
           << "21\n"
           << lineEnd.y << "\n"
           << "31\n0\n"
           << "0\nENDBLK\n"
           << "0\nENDSEC\n"
           << "0\nSECTION\n"
           << "2\nENTITIES\n"
           << "0\nINSERT\n"
           << "8\n"
           << layer << "\n"
           << "2\n"
           << block << "\n"
           << "10\n"
           << blockTranslation.x << "\n"
           << "20\n"
           << blockTranslation.y << "\n"
           << "30\n"
           << 0 << "\n"
           << "41\n"
           << blockScale.x << "\n"
           << "42\n"
           << blockScale.y << "\n"
           << "43\n"
           << 0 << "\n"
           << "50\n"
           << blockRotation << "\n"
           << "0\nENDSEC\n"
           << "0\nEOF\n";

    DxfParser dxfParser;
    dxfParser.open(stream);
    REQUIRE(dxfParser.numLayers() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->numLines() == 1);
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().x ==
            Catch::Approx(lineStart.x + 2 * blockTranslation.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getStart().y ==
            Catch::Approx(lineStart.y + 2 * blockTranslation.y).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().x ==
            Catch::Approx(lineEnd.x + 2 * blockTranslation.x).epsilon(epsilon));
    REQUIRE(dxfParser.getLayer(layer.c_str())->getLine(0).getEnd().y ==
            Catch::Approx(lineEnd.y + 2 * blockTranslation.y).epsilon(epsilon));
}
