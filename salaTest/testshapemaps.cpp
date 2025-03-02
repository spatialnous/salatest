// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/mapconverter.hpp"
#include "salalib/shapemap.hpp"

#include "salalib/genlib/line4f.hpp"

#include "catch_amalgamated.hpp"

#include <iostream>
#include <sstream>

TEST_CASE("Test ShapeMap::copy()") {

    std::unique_ptr<ShapeMap> shapeMap(new ShapeMap("Test ShapeMap"));

    // Barnsbury
    shapeMap->makeLineShape(Line4f(Point2f(984, -1684), Point2f(1346, -1523)));
    shapeMap->makeLineShape(Line4f(Point2f(1306, -1497), Point2f(1379, -1746)));
    shapeMap->makeLineShape(Line4f(Point2f(1257, -1772), Point2f(1494, -1712)));
    shapeMap->makeLineShape(Line4f(Point2f(1200, -1581), Point2f(1271, -1779)));
    shapeMap->makeLineShape(Line4f(Point2f(1174, -1684), Point2f(1537, -1588)));
    shapeMap->makeLineShape(Line4f(Point2f(1223, -1898), Point2f(1269, -1760)));
    shapeMap->makeLineShape(Line4f(Point2f(946, -1386), Point2f(1032, -1976)));
    shapeMap->makeLineShape(Line4f(Point2f(1350, -1896), Point2f(1372, -1692)));
    shapeMap->makeLineShape(Line4f(Point2f(1117, -1979), Point2f(1120, -1594)));
    shapeMap->makeLineShape(Line4f(Point2f(988, -1876), Point2f(1261, -1867)));
    shapeMap->makeLineShape(Line4f(Point2f(1353, -1804), Point2f(1354, -2090)));
    shapeMap->makeLineShape(Line4f(Point2f(1202, -2146), Point2f(1232, -1856)));
    shapeMap->makeLineShape(Line4f(Point2f(1167, -1874), Point2f(1359, -1850)));
    shapeMap->makeLineShape(Line4f(Point2f(1170, -1756), Point2f(1377, -1702)));
    shapeMap->makeLineShape(Line4f(Point2f(988, -1715), Point2f(1218, -1613)));
    shapeMap->makeLineShape(Line4f(Point2f(1006, -2177), Point2f(1027, -1900)));
    shapeMap->makeLineShape(Line4f(Point2f(1236, -1847), Point2f(1271, -2058)));
    shapeMap->makeLineShape(Line4f(Point2f(988, -1966), Point2f(1150, -1977)));
    shapeMap->makeLineShape(Line4f(Point2f(1111, -1795), Point2f(1263, -1797)));
    shapeMap->makeLineShape(Line4f(Point2f(973, -1752), Point2f(1199, -1753)));
    shapeMap->makeLineShape(Line4f(Point2f(1149, -1715), Point2f(1367, -1670)));
    shapeMap->makeLineShape(Line4f(Point2f(1048, -1648), Point2f(1061, -1722)));
    shapeMap->makeLineShape(Line4f(Point2f(1218, -1644), Point2f(1344, -1600)));
    shapeMap->makeLineShape(Line4f(Point2f(1128, -1970), Point2f(1279, -2046)));
    shapeMap->makeLineShape(Line4f(Point2f(1283, -1542), Point2f(1297, -1589)));
    shapeMap->makeLineShape(Line4f(Point2f(1254, -2044), Point2f(1359, -2012)));
    shapeMap->makeLineShape(Line4f(Point2f(1287, -1759), Point2f(1321, -1812)));
    shapeMap->makeLineShape(Line4f(Point2f(1287, -1819), Point2f(1300, -1755)));
    shapeMap->makeLineShape(Line4f(Point2f(1296, -1949), Point2f(1365, -1921)));
    shapeMap->makeLineShape(Line4f(Point2f(1329, -1748), Point2f(1337, -1792)));
    shapeMap->makeLineShape(Line4f(Point2f(1065, -1818), Point2f(1069, -1977)));
    shapeMap->makeLineShape(Line4f(Point2f(1184, -1671), Point2f(1195, -1714)));
    shapeMap->makeLineShape(Line4f(Point2f(1155, -1670), Point2f(1196, -1684)));
    shapeMap->makeLineShape(Line4f(Point2f(1298, -1863), Point2f(1319, -1801)));
    shapeMap->makeLineShape(Line4f(Point2f(1283, -1804), Point2f(1317, -1859)));
    shapeMap->makeLineShape(Line4f(Point2f(1190, -1824), Point2f(1198, -1883)));
    shapeMap->makeLineShape(Line4f(Point2f(1205, -1835), Point2f(1256, -1841)));
    shapeMap->makeLineShape(Line4f(Point2f(1171, -1860), Point2f(1175, -1884)));
    shapeMap->makeLineShape(Line4f(Point2f(1179, -1744), Point2f(1195, -1801)));
    shapeMap->makeLineShape(Line4f(Point2f(1344, -1782), Point2f(1368, -1793)));
    shapeMap->makeLineShape(Line4f(Point2f(1060, -1941), Point2f(1123, -1936)));
    shapeMap->makeLineShape(Line4f(Point2f(1151, -1887), Point2f(1160, -1826)));
    shapeMap->makeLineShape(Line4f(Point2f(1299, -1917), Point2f(1360, -1941)));
    shapeMap->makeLineShape(Line4f(Point2f(1147, -1669), Point2f(1162, -1776)));
    shapeMap->makeLineShape(Line4f(Point2f(1304, -1818), Point2f(1308, -1927)));
    shapeMap->makeLineShape(Line4f(Point2f(1244, -1924), Point2f(1315, -1949)));
    shapeMap->makeLineShape(Line4f(Point2f(1274, -1853), Point2f(1276, -1877)));
    shapeMap->makeLineShape(Line4f(Point2f(1054, -1703), Point2f(1091, -1755)));
    shapeMap->makeLineShape(Line4f(Point2f(1026, -1755), Point2f(1065, -1703)));
    shapeMap->makeLineShape(Line4f(Point2f(1220, -1732), Point2f(1229, -1762)));
    shapeMap->makeLineShape(Line4f(Point2f(1245, -1939), Point2f(1317, -1918)));
    shapeMap->makeLineShape(Line4f(Point2f(1306, -1937), Point2f(1308, -2036)));
    shapeMap->makeLineShape(Line4f(Point2f(1032, -1731), Point2f(1048, -1830)));
    shapeMap->makeLineShape(Line4f(Point2f(1079, -1827), Point2f(1080, -1726)));
    shapeMap->makeLineShape(Line4f(Point2f(1330, -1789), Point2f(1350, -1783)));
    shapeMap->makeLineShape(Line4f(Point2f(1044, -1824), Point2f(1085, -1822)));
    shapeMap->makeLineShape(Line4f(Point2f(1154, -1829), Point2f(1197, -1829)));
    shapeMap->makeLineShape(Line4f(Point2f(1199, -1766), Point2f(1234, -1757)));

    auto shapeGraph = MapConverter::convertDataToAxial(nullptr, "aa", *shapeMap);

    std::unique_ptr<ShapeGraph> newShapeGraph(new ShapeGraph("New ShapeMap"));

    newShapeGraph->copy(*shapeGraph, ShapeMap::COPY_ALL);
    REQUIRE(newShapeGraph->getAllShapes() == shapeGraph->getAllShapes());

    REQUIRE(newShapeGraph->getConnections().size() == shapeGraph->getConnections().size());
    for (size_t i = 0; i < shapeGraph->getConnections().size(); i++) {
        REQUIRE(newShapeGraph->getConnections()[i].connections ==
                shapeGraph->getConnections()[i].connections);
    }

    Point2f p1(0982.8, -1620.3);
    Point2f p2(1217.1, -1977.3);
    Region4f region(p1, p1);
    auto selSet = shapeGraph->getShapesInRegion(region);
    REQUIRE(selSet.size() == 1);
    REQUIRE(selSet.begin()->first == 6);
    shapeGraph->linkShapes(p2, selSet.begin()->first);
    REQUIRE(shapeGraph->getLinks().size() == 1);

    auto newSelSet = newShapeGraph->getShapesInRegion(region);
    REQUIRE(newSelSet.size() == 1);
    REQUIRE(newSelSet.begin()->first == 6);
    newShapeGraph->linkShapes(p2, newSelSet.begin()->first);
    REQUIRE(newShapeGraph->getLinks().size() == 1);
}

TEST_CASE("Testing ShapeMap::getAllShapes variants") {
    const float epsilon = 0.001f;
    Point2f line0Start(0, 1);
    Point2f line0End(3, 2);
    Point2f line1Start(1, 1);
    Point2f line1End(1, -1);

    std::unique_ptr<ShapeMap> shapeMap(new ShapeMap("Test ShapeMap"));

    shapeMap->makeLineShape(Line4f(line0Start, line0End));
    shapeMap->makeLineShape(Line4f(line1Start, line1End));

    std::vector<Point2f> polyVertices;
    polyVertices.push_back(Point2f(-1, -1));
    polyVertices.push_back(Point2f(2, -1));
    polyVertices.push_back(Point2f(0, 0));

    shapeMap->makePolyShape(polyVertices, false, false);

    SECTION("ShapeMap::getAllShapesAsLines") {
        std::vector<SimpleLine> lines = shapeMap->getAllShapesAsSimpleLines();

        REQUIRE(lines.size() == 5);

        REQUIRE(lines[0].start().x == Catch::Approx(line0Start.x).epsilon(epsilon));
        REQUIRE(lines[0].start().y == Catch::Approx(line0Start.y).epsilon(epsilon));
        REQUIRE(lines[0].end().x == Catch::Approx(line0End.x).epsilon(epsilon));
        REQUIRE(lines[0].end().y == Catch::Approx(line0End.y).epsilon(epsilon));

        REQUIRE(lines[1].start().x == Catch::Approx(line1Start.x).epsilon(epsilon));
        REQUIRE(lines[1].start().y == Catch::Approx(line1Start.y).epsilon(epsilon));
        REQUIRE(lines[1].end().x == Catch::Approx(line1End.x).epsilon(epsilon));
        REQUIRE(lines[1].end().y == Catch::Approx(line1End.y).epsilon(epsilon));

        REQUIRE(lines[2].start().x == Catch::Approx(polyVertices[0].x).epsilon(epsilon));
        REQUIRE(lines[2].start().y == Catch::Approx(polyVertices[0].y).epsilon(epsilon));
        REQUIRE(lines[2].end().x == Catch::Approx(polyVertices[1].x).epsilon(epsilon));
        REQUIRE(lines[2].end().y == Catch::Approx(polyVertices[1].y).epsilon(epsilon));

        REQUIRE(lines[3].start().x == Catch::Approx(polyVertices[1].x).epsilon(epsilon));
        REQUIRE(lines[3].start().y == Catch::Approx(polyVertices[1].y).epsilon(epsilon));
        REQUIRE(lines[3].end().x == Catch::Approx(polyVertices[2].x).epsilon(epsilon));
        REQUIRE(lines[3].end().y == Catch::Approx(polyVertices[2].y).epsilon(epsilon));

        REQUIRE(lines[4].start().x == Catch::Approx(polyVertices[2].x).epsilon(epsilon));
        REQUIRE(lines[4].start().y == Catch::Approx(polyVertices[2].y).epsilon(epsilon));
        REQUIRE(lines[4].end().x == Catch::Approx(polyVertices[0].x).epsilon(epsilon));
        REQUIRE(lines[4].end().y == Catch::Approx(polyVertices[0].y).epsilon(epsilon));
    }
    SECTION("ShapeMap::getAllLinesWithColour") {
        std::vector<std::pair<SimpleLine, PafColor>> colouredLines =
            shapeMap->getAllSimpleLinesWithColour(std::set<int>());

        REQUIRE(colouredLines.size() == 2);

        REQUIRE(colouredLines[0].first.start().x ==
                Catch::Approx(std::min(line0Start.x, line0End.x)).epsilon(epsilon));
        REQUIRE(colouredLines[0].first.start().y ==
                Catch::Approx(std::min(line0Start.y, line0End.y)).epsilon(epsilon));
        REQUIRE(colouredLines[0].first.end().x ==
                Catch::Approx(std::max(line0Start.x, line0End.x)).epsilon(epsilon));
        REQUIRE(colouredLines[0].first.end().y ==
                Catch::Approx(std::max(line0Start.y, line0End.y)).epsilon(epsilon));
        REQUIRE(colouredLines[0].second.redf() == Catch::Approx(0.2f).epsilon(epsilon));
        REQUIRE(colouredLines[0].second.greenf() == Catch::Approx(0.2f).epsilon(epsilon));
        REQUIRE(colouredLines[0].second.bluef() == Catch::Approx(0.86667f).epsilon(epsilon));

        REQUIRE(colouredLines[1].first.start().x == Catch::Approx(line1Start.x).epsilon(epsilon));
        REQUIRE(colouredLines[1].first.start().y == Catch::Approx(line1Start.y).epsilon(epsilon));
        REQUIRE(colouredLines[1].first.end().x == Catch::Approx(line1End.x).epsilon(epsilon));
        REQUIRE(colouredLines[1].first.end().y == Catch::Approx(line1End.y).epsilon(epsilon));
        REQUIRE(colouredLines[1].second.redf() == Catch::Approx(0.13333f).epsilon(epsilon));
        REQUIRE(colouredLines[1].second.greenf() == Catch::Approx(0.86667f).epsilon(epsilon));
        REQUIRE(colouredLines[1].second.bluef() == Catch::Approx(0.53333f).epsilon(epsilon));
    }
    SECTION("ShapeMap::getAllPolygonsWithColour") {
        std::vector<std::pair<std::vector<Point2f>, PafColor>> colouredPolygons =
            shapeMap->getAllPolygonsWithColour(std::set<int>());

        REQUIRE(colouredPolygons.size() == 1);

        auto iter = colouredPolygons.begin();
        const std::vector<Point2f> vertices = iter->first;
        const PafColor colour = iter->second;

        REQUIRE(vertices[0].x == Catch::Approx(polyVertices[0].x).epsilon(epsilon));
        REQUIRE(vertices[0].y == Catch::Approx(polyVertices[0].y).epsilon(epsilon));
        REQUIRE(vertices[1].x == Catch::Approx(polyVertices[1].x).epsilon(epsilon));
        REQUIRE(vertices[1].y == Catch::Approx(polyVertices[1].y).epsilon(epsilon));
        REQUIRE(vertices[2].x == Catch::Approx(polyVertices[2].x).epsilon(epsilon));
        REQUIRE(vertices[2].y == Catch::Approx(polyVertices[2].y).epsilon(epsilon));
        REQUIRE(colour.redf() == Catch::Approx(0.86667f).epsilon(epsilon));
        REQUIRE(colour.greenf() == Catch::Approx(0.2f).epsilon(epsilon));
        REQUIRE(colour.bluef() == Catch::Approx(0.2f).epsilon(epsilon));
    }
}
