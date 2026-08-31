// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/mapconverter.hpp"
#include "salalib/shapegraph.hpp"
#include "salalib/shapemap.hpp"

#include "salalib/genlib/line4f.hpp"

#include "catch_amalgamated.hpp"
#include "salalib/shapemapgroupdata.hpp"

#include <memory>
#include <sstream>
#include <utility>
#include <vector>

TEST_CASE("Testing ShapeGraph::writeAxialConnections") {

    Point2f line1Start(0, 0);
    Point2f line1End(3, 0);
    Point2f line2Start(1, 1);
    Point2f line2End(1, -1);
    Point2f line3Start(2, 1);
    Point2f line3End(2, -2);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");

    spacePixels.back().makeLineShape(Line4f(line1Start, line1End));
    spacePixels.back().makeLineShape(Line4f(line2Start, line2End));
    spacePixels.back().makeLineShape(Line4f(line3Start, line3End));

    auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);

    auto shapegraph = MapConverter::convertDrawingToAxial(nullptr, "Test axial", drawingMapRefs);

    REQUIRE(shapegraph->getConnections().size() == 3);
    REQUIRE(shapegraph->getConnections()[0].connections.size() == 2);
    REQUIRE(shapegraph->getConnections()[1].connections.size() == 1);
    REQUIRE(shapegraph->getConnections()[2].connections.size() == 1);

    SECTION("writeAxialConnectionsAsDotGraph") {
        std::stringstream stream;
        shapegraph->writeAxialConnectionsAsDotGraph(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{
            "strict graph {", "    0 -- 1", "    0 -- 2", "    1 -- 0", "    2 -- 0", "}", ""};
        REQUIRE(lines == expected);
    }
    SECTION("writeAxialConnectionsAsPairsCSV") {
        std::stringstream stream;
        shapegraph->writeAxialConnectionsAsPairsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{"refA,refB", "0,1", "0,2", "1,0", "2,0"};
        REQUIRE(lines == expected);
    }
}
TEST_CASE("Testing ShapeGraph::writeSegmentConnections") {
    // As we are converting the drawing directly to segments
    // the lines need to touch, not cross

    Point2f line1Start(1, 1);
    Point2f line1End(1, 0);
    Point2f line2Start(1, 0);
    Point2f line2End(2, 0);
    Point2f line3Start(2, 0);
    Point2f line3End(2, 2);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);
    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");

    spacePixels.back().makeLineShape(Line4f(line1Start, line1End));
    spacePixels.back().makeLineShape(Line4f(line2Start, line2End));
    spacePixels.back().makeLineShape(Line4f(line3Start, line3End));

    auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);
    auto shapegraph =
        MapConverter::convertDrawingToSegment(nullptr, "Test segment", drawingMapRefs);

    SECTION("writeSegmentConnectionsAsPairsCSV") {
        std::stringstream stream;
        shapegraph->writeSegmentConnectionsAsPairsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> lines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            lines.push_back(line);
        }
        std::vector<std::string> expected{"refA,refB,ss_weight,for_back,dir", "0,1,1,0,1",
                                          "1,2,1,0,1", "1,0,1,1,-1", "2,1,1,1,-1"};
        REQUIRE(lines == expected);
    }
}

// While the linking functionality is placed in the ShapeMap,
// (for example the variables m_links and m_unlinks) it can
// only be used through ShapeGraph because it starts with
// m_hasgraph = true. Ideally the linking functionality should
// move to the ShapeGraph

TEST_CASE("Testing ShapeMap::getAllLinkLines and ShapeMap::getAllUnlinkPoints()") {
    const float epsilon = 0.001f;
    const double toleranceA = 1e-9;
    Point2f line0Start(0.522, 0.424);
    Point2f line0End(0.709, 1.098);
    Point2f line1Start(0.897, 1.123);
    Point2f line1End(1.122, 0.421);
    Point2f line2Start(1.073, 0.386);
    Point2f line2End(1.269, 1.196);

    std::unique_ptr<ShapeGraph> shapeGraph(new ShapeGraph("Test ShapeMap"));

    shapeGraph->makeLineShape(Line4f(line0Start, line0End));
    shapeGraph->makeLineShape(Line4f(line1Start, line1End));
    shapeGraph->makeLineShape(Line4f(line2Start, line2End));

    shapeGraph->makeShapeConnections();

    shapeGraph->linkShapes(0, 1);
    shapeGraph->unlinkShapes(1, 2);

    std::vector<SimpleLine> linkLines = shapeGraph->getAllLinkLines();

    REQUIRE(linkLines.size() == 1);

    REQUIRE(linkLines[0].start().x ==
            Catch::Approx((line0Start.x + line0End.x) * 0.5).epsilon(epsilon));
    REQUIRE(linkLines[0].start().y ==
            Catch::Approx((line0Start.y + line0End.y) * 0.5).epsilon(epsilon));
    REQUIRE(linkLines[0].end().x ==
            Catch::Approx((line1Start.x + line1End.x) * 0.5).epsilon(epsilon));
    REQUIRE(linkLines[0].end().y ==
            Catch::Approx((line1Start.y + line1End.y) * 0.5).epsilon(epsilon));

    std::vector<Point2f> unlinkPoints = shapeGraph->getAllUnlinkPoints();

    REQUIRE(unlinkPoints.size() == 1);

    Point2f intersection =
        Line4f(line1Start, line1End).intersection_point(Line4f(line2Start, line2End), toleranceA);

    REQUIRE(unlinkPoints[0].x == Catch::Approx(intersection.x).epsilon(epsilon));
    REQUIRE(unlinkPoints[0].y == Catch::Approx(intersection.y).epsilon(epsilon));
}
