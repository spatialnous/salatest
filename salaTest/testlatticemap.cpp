// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "comm.hpp"

#include "salalib/metagraph.hpp"
#include "salalib/shapemapgroupdata.hpp"
#include "salalib/vgamodules/vgametricdepth.hpp"

#include "catch_amalgamated.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

TEST_CASE("Test MetaGraph construction", "") {
    const float epsilon = 0.001f;
    Point2f offset(0, 0); // seems that this is always set to 0,0

    // create a new MetaGraph
    // The LatticeMap needs the region variable from this
    // object as a definition of the area the grid needs to cover
    MetaGraph metaGraph("Test MetaGraph");

    SECTION("Construct a plain MetaGraph without underlying geometry") {
        Point2f bottomLeft(0, 0);
        Point2f topRight(2, 4);

        // set region to the bounds
        metaGraph.region = Region4f(bottomLeft, topRight);

        // check if the bounds are set correctly
        REQUIRE(metaGraph.region.bottomLeft.x == Catch::Approx(bottomLeft.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.bottomLeft.y == Catch::Approx(bottomLeft.y).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.x == Catch::Approx(topRight.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.y == Catch::Approx(topRight.y).epsilon(epsilon));
    }

    SECTION("Construct a MetaGraph using underlying geometry") {
        Point2f lineStart(0, 0);
        Point2f lineEnd(2, 4);

        Point2f bottomLeft(std::min(lineStart.x, lineEnd.x), std::min(lineStart.y, lineEnd.y));
        Point2f topRight(std::max(lineStart.x, lineEnd.x), std::max(lineStart.y, lineEnd.y));

        // sala does not bundle the maps in the MetaGraph anymore,
        // except when reading a file (in a temporary container).
        // instead rely on the same vector of pairs construct
        std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

        auto &spacePixelFileData = drawingFiles.back().first;
        spacePixelFileData.name = "Test MetaGraph";
        auto &spacePixels = drawingFiles.back().second;

        // push a ShapeMap in the SpacePixelFile
        spacePixels.emplace_back("Test ShapeMap");

        auto &newShapeMap = spacePixels.back();
        // add a line to the ShapeMap
        newShapeMap.makeLineShape(Line4f(lineStart, lineEnd));

        // check if the ShapeMap bounds are set correctly
        REQUIRE(newShapeMap.getRegion().bottomLeft.x ==
                Catch::Approx(bottomLeft.x).epsilon(epsilon));
        REQUIRE(newShapeMap.getRegion().bottomLeft.y ==
                Catch::Approx(bottomLeft.y).epsilon(epsilon));
        REQUIRE(newShapeMap.getRegion().topRight.x == Catch::Approx(topRight.x).epsilon(epsilon));
        REQUIRE(newShapeMap.getRegion().topRight.y == Catch::Approx(topRight.y).epsilon(epsilon));

        // MetaGraph and SpacePixelFile do not automatically grow
        // their region when new shapemaps/files are added to them
        // therefore we have to do this externally
        spacePixelFileData.region = newShapeMap.getRegion();

        // check if the SpacePixelFile bounds are set correctly
        REQUIRE(spacePixelFileData.region.bottomLeft.x ==
                Catch::Approx(bottomLeft.x).epsilon(epsilon));
        REQUIRE(spacePixelFileData.region.bottomLeft.y ==
                Catch::Approx(bottomLeft.y).epsilon(epsilon));
        REQUIRE(spacePixelFileData.region.topRight.x == Catch::Approx(topRight.x).epsilon(epsilon));
        REQUIRE(spacePixelFileData.region.topRight.y == Catch::Approx(topRight.y).epsilon(epsilon));

        metaGraph.region =
            Region4f(spacePixelFileData.region.bottomLeft, spacePixelFileData.region.topRight);

        // check if the MetaGraph bounds are set correctly
        REQUIRE(metaGraph.region.bottomLeft.x == Catch::Approx(bottomLeft.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.bottomLeft.y == Catch::Approx(bottomLeft.y).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.x == Catch::Approx(topRight.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.y == Catch::Approx(topRight.y).epsilon(epsilon));
    }

    // construct a sample lattice map
    LatticeMap latticeMap(metaGraph.region, "Test LatticeMap");
}

TEST_CASE("Test grid filling", "") {
    const float epsilon = 0.001f;
    double spacing = 0.5;
    Point2f offset(0, 0); // seems that this is always set to 0,0

    // create a new MetaGraph
    // The LatticeMap needs the region variable from this
    // object as a definition of the area the grid needs to cover
    MetaGraph metaGraph("Test MetaGraph");

    // Construct a plain MetaGraph without underlying geometry
    {
        Point2f bottomLeft(0, 0);
        Point2f topRight(2, 4);

        // set region to the bounds
        metaGraph.region = Region4f(bottomLeft, topRight);

        // check if the bounds are set correctly
        REQUIRE(metaGraph.region.bottomLeft.x == Catch::Approx(bottomLeft.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.bottomLeft.y == Catch::Approx(bottomLeft.y).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.x == Catch::Approx(topRight.x).epsilon(epsilon));
        REQUIRE(metaGraph.region.topRight.y == Catch::Approx(topRight.y).epsilon(epsilon));
    }

    // construct a sample lattice map
    LatticeMap latticeMap(metaGraph.region, "Test LatticeMap");

    // set the grid

    // create the grid with bounds as set above
    bool gridIsSet = latticeMap.setGrid(spacing, offset);

    // check if the grid was set
    REQUIRE(gridIsSet);

    // check if the spacing is correct
    REQUIRE(spacing == latticeMap.getSpacing());

    // fill the grid

    // seems like fill_type is actually connected to the
    // QDepthmapView class which is a GUI class (depthmapview.h)
    // TODO Disentangle GUI enum from latticeMap.makePoints
    int fillType = 0; // = QDepthmapView::FULLFILL

    Point2f gridBottomLeft = latticeMap.getRegion().bottomLeft;

    SECTION("Check if the points are made when fill selection in a cell") {
        // Check if the points are made (grid filled) when
        // the selected position is certainly in a cell
        // This calculation should make the point directly
        // at the centre of a central cell
        Point2f midPoint(
            gridBottomLeft.x +
                spacing * (floor(static_cast<double>(latticeMap.getCols()) * 0.5) + 0.5),
            gridBottomLeft.y +
                spacing * (floor(static_cast<double>(latticeMap.getRows()) * 0.5) + 0.5));
        std::vector<Line4f> lines;
        latticeMap.blockLines(lines);
        bool pointsMade = latticeMap.makePoints(midPoint, fillType);
        REQUIRE(pointsMade);
    }

    SECTION("Check if the points are made when fill selection between cells") {
        // Check if the points are made (grid filled) when
        // the selected position is certainly between cells
        // This calculation should make the point directly
        // at the edge of a central cell
        Point2f midPoint(
            gridBottomLeft.x + spacing * (floor(static_cast<double>(latticeMap.getCols()) * 0.5)),
            gridBottomLeft.y + spacing * (floor(static_cast<double>(latticeMap.getRows()) * 0.5)));
        std::vector<Line4f> lines;
        latticeMap.blockLines(lines);
        bool pointsMade = latticeMap.makePoints(midPoint, fillType);
        REQUIRE(pointsMade);
    }
}

// LatticeMap::setGrid is quite convoluted with various parameters
// affecting the result, such as the limits of the region to be
// covered (bottomLeft, topRight), the spacing and the location
// of the plan in space. For example every grid created will be
// in relation to the origin (0,0), no matter where the region
// is and the current pixel can always be calculated as if the
// origin always falls in the centre of a cell.

TEST_CASE("Quirks in grid creation - Origin always at 0", "") {

    double spacing = 0.5;
    const float epsilon = 0.001f;
    Point2f offset(0, 0); // seems that this is always set to 0,0

    Point2f bottomLeft(0, 0);
    Point2f topRight(0, 0);

    SECTION("Region from origin to positive x, positive y quadrant") {
        spacing = 0.5;
        bottomLeft.x = 0;
        bottomLeft.y = 0;
        topRight.x = 1;
        topRight.y = 1;
    }

    SECTION("Region away from origin to positive x, positive y quadrant") {
        spacing = 0.5;
        bottomLeft.x = 1;
        bottomLeft.y = 1;
        topRight.x = 2;
        topRight.y = 2;
    }

    SECTION("Region from origin to negative x, negative y quadrant") {
        spacing = 0.5;
        bottomLeft.x = -1;
        bottomLeft.y = -1;
        topRight.x = 0;
        topRight.y = 0;
    }

    SECTION("Region in all quadrants") {
        spacing = 0.5;
        bottomLeft.x = -1;
        bottomLeft.y = -1;
        topRight.x = 1;
        topRight.y = 1;
    }

    SECTION("Region in positive x, positive y quadrant, non-rectangular") {
        spacing = 0.5;
        bottomLeft.x = 1;
        bottomLeft.y = 2;
        topRight.x = 3;
        topRight.y = 4;
    }

    SECTION("Region in positive x, positive y quadrant, floating-point limits") {
        spacing = 0.5;
        bottomLeft.x = 1.1;
        bottomLeft.y = 2.2;
        topRight.x = 3.3;
        topRight.y = 4.4;
    }

    SECTION("Region in positive x, positive y quadrant, floating-point limits") {
        spacing = 0.5;
        bottomLeft.x = 0.1;
        bottomLeft.y = 0.2;
        topRight.x = 0.3;
        topRight.y = 0.4;
    }

    SECTION("Region in negative x, negative y quadrant, floating-point limits") {
        spacing = 0.5;
        bottomLeft.x = -0.4;
        bottomLeft.y = -0.3;
        topRight.x = -0.2;
        topRight.y = -0.1;
    }

    SECTION("Region in all quadrants, floating-point limits") {
        spacing = 0.5;
        bottomLeft.x = -1.1;
        bottomLeft.y = -2.2;
        topRight.x = 3.3;
        topRight.y = 4.4;
    }

    SECTION("Region in all quadrants, floating-point limits, smaller spacing") {
        spacing = 0.25;
        bottomLeft.x = 1.1;
        bottomLeft.y = 2.2;
        topRight.x = 3.3;
        topRight.y = 4.4;
    }

    MetaGraph metaGraph("Test MetaGraph");
    metaGraph.region = Region4f(bottomLeft, topRight);
    LatticeMap latticeMap(metaGraph.region, "Test LatticeMap");
    bool gridIsSet = latticeMap.setGrid(spacing, offset);

    REQUIRE(gridIsSet);

    int bottomLeftPixelIndexX = static_cast<int>(floor(bottomLeft.x / spacing - 0.5)) + 1;
    int bottomLeftPixelIndexY = static_cast<int>(floor(bottomLeft.y / spacing - 0.5)) + 1;

    auto topRightPixelIndexX = static_cast<int>(floor(topRight.x / spacing - 0.5)) + 1;
    auto topRightPixelIndexY = static_cast<int>(floor(topRight.y / spacing - 0.5)) + 1;

    int numCellsX = topRightPixelIndexX - bottomLeftPixelIndexX + 1;
    int numCellsY = topRightPixelIndexY - bottomLeftPixelIndexY + 1;

    // check if the size of the grid is as expected
    REQUIRE(static_cast<int>(latticeMap.getCols()) == numCellsX);
    REQUIRE(static_cast<int>(latticeMap.getRows()) == numCellsY);

    Point2f gridBottomLeft(bottomLeftPixelIndexX * spacing - 0.5 * spacing,
                           bottomLeftPixelIndexY * spacing - 0.5 * spacing);

    // check if the bottom-left corner of the bottom-left pixel is as expected
    REQUIRE(latticeMap.getRegion().bottomLeft.x ==
            Catch::Approx(gridBottomLeft.x).epsilon(epsilon));
    REQUIRE(latticeMap.getRegion().bottomLeft.y ==
            Catch::Approx(gridBottomLeft.y).epsilon(epsilon));

    Point2f midPoint(gridBottomLeft.x + spacing * (floor(numCellsX * 0.5) + 0.5),
                     gridBottomLeft.y + spacing * (floor(numCellsY * 0.5) + 0.5));

    int fillType = 0; // = QDepthmapView::FULLFILL
    std::vector<Line4f> lines;
    latticeMap.blockLines(lines);
    bool pointsMade = latticeMap.makePoints(midPoint, fillType);

    // check if the grid is filled
    REQUIRE(pointsMade);
}

TEST_CASE("Test LatticeMap connections output", "") {
    double spacing = 0.5;
    Point2f offset(0, 0); // seems that this is always set to 0,0

    MetaGraph metaGraph("Test MetaGraph");

    double rectSize = 1.5;

    Point2f line0Start(0, 0);
    Point2f line0End(0, rectSize);
    Point2f line1Start(0, rectSize);
    Point2f line1End(rectSize, rectSize);
    Point2f line2Start(rectSize, rectSize);
    Point2f line2End(rectSize, 0);
    Point2f line3Start(rectSize, 0);
    Point2f line3End(0, 0);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;

    spacePixels.emplace_back("Test ShapeMap");
    spacePixels.back().makeLineShape(Line4f(line0Start, line0End));
    spacePixels.back().makeLineShape(Line4f(line1Start, line1End));
    spacePixels.back().makeLineShape(Line4f(line2Start, line2End));
    spacePixels.back().makeLineShape(Line4f(line3Start, line3End));
    spacePixelFileData.region = spacePixels.back().getRegion();
    metaGraph.region =
        Region4f(spacePixelFileData.region.bottomLeft, spacePixelFileData.region.topRight);
    LatticeMap latticeMap(metaGraph.region, "Test LatticeMap");

    Point2f gridBottomLeft = latticeMap.getRegion().bottomLeft;

    Point2f midPoint(gridBottomLeft.x +
                         spacing * (floor(static_cast<double>(latticeMap.getCols()) * 0.5) + 0.5),
                     gridBottomLeft.y +
                         spacing * (floor(static_cast<double>(latticeMap.getRows()) * 0.5) + 0.5));

    int fillType = 0; // = QDepthmapView::FULLFILL
    bool gridIsSet = latticeMap.setGrid(spacing, offset);

    REQUIRE(gridIsSet);

    std::vector<Line4f> lines = spacePixels.back().getAllShapesAsLines();
    latticeMap.blockLines(lines);
    bool pointsMade = latticeMap.makePoints(midPoint, fillType);

    REQUIRE(pointsMade);

    bool boundaryGraph = false;
    double maxDist = -1;
    // a communicator is required in order to create the connections between the pixels
    std::unique_ptr<Communicator> comm(new ICommunicator());

    bool graphMade = latticeMap.sparkGraph2(comm.get(), boundaryGraph, maxDist);

    REQUIRE(graphMade);

    SECTION("LatticeMap::outputLinksAsCSV") {
        std::stringstream stream;
        latticeMap.mergePixels(65537, 131074);
        latticeMap.mergePixels(131073, 65538);
        latticeMap.outputLinksAsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> streamLines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            streamLines.push_back(line);
        }
        std::vector<std::string> expected{"RefFrom,RefTo", "65537,131074", "65538,131073"};
        REQUIRE(streamLines == expected);
    }

    SECTION("LatticeMap::outputConnectionsAsCSV") {
        std::stringstream stream;
        latticeMap.outputConnectionsAsCSV(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> streamLines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            streamLines.push_back(line);
        }
        std::vector<std::string> expected{"RefFrom,RefTo", "65537,131073", "65537,131074",
                                          "65537,65538",   "65538,131074", "65538,131073",
                                          "131073,131074"};
        REQUIRE(streamLines == expected);
    }

    SECTION("LatticeMap::outputConnections") {
        std::stringstream stream;
        latticeMap.outputConnections(stream);

        REQUIRE(stream.good());
        char line[1000];
        std::vector<std::string> streamLines;
        while (!stream.eof()) {
            stream.getline(line, 1000);
            streamLines.push_back(line);
        }
        std::vector<std::string> expected{"#graph v1.0",
                                          "node {",
                                          "  ref    65537",
                                          "  origin 0.5 0.5 0",
                                          "  connections [",
                                          "    131073,",
                                          "    131074,",
                                          "    65538,",
                                          "  ]",
                                          "}",
                                          "node {",
                                          "  ref    65538",
                                          "  origin 0.5 1 0",
                                          "  connections [",
                                          "    131074,",
                                          "    65537,",
                                          "    131073,",
                                          "  ]",
                                          "}",
                                          "node {",
                                          "  ref    131073",
                                          "  origin 1 0.5 0",
                                          "  connections [",
                                          "    131074,",
                                          "    65538,",
                                          "    65537,",
                                          "  ]",
                                          "}",
                                          "node {",
                                          "  ref    131074",
                                          "  origin 1 1 0",
                                          "  connections [",
                                          "    65538,",
                                          "    65537,",
                                          "    131073,",
                                          "  ]",
                                          "}",
                                          ""};
        REQUIRE(streamLines == expected);
    }
}
TEST_CASE("Direct LatticeMap linking - fully filled grid (no geometry)", "") {
    double spacing = 0.5;
    Point2f offset(0, 0); // seems that this is always set to 0,0
    Point2f bottomLeft(0, 0);
    Point2f topRight(2, 4);
    int fillType = 0; // = QDepthmapView::FULLFILL

    MetaGraph metaGraph("Test MetaGraph");
    metaGraph.region = Region4f(bottomLeft, topRight);
    LatticeMap latticeMap(metaGraph.region, "Test LatticeMap");
    latticeMap.setGrid(spacing, offset);
    Point2f gridBottomLeft = latticeMap.getRegion().bottomLeft;
    Point2f midPoint(gridBottomLeft.x +
                         spacing * (floor(static_cast<double>(latticeMap.getCols()) * 0.5) + 0.5),
                     gridBottomLeft.y +
                         spacing * (floor(static_cast<double>(latticeMap.getRows()) * 0.5) + 0.5));
    std::vector<Line4f> lines;
    latticeMap.blockLines(lines);
    latticeMap.makePoints(midPoint, fillType);

    std::vector<Line4f> mergeLines;

    PixelRef bottomLeftPixel = latticeMap.pixelate(bottomLeft);
    PixelRef topRightPixel = latticeMap.pixelate(topRight);

    // make sure pixels are not already merged
    REQUIRE(!latticeMap.isPixelMerged(bottomLeftPixel));
    REQUIRE(!latticeMap.isPixelMerged(topRightPixel));

    // merge
    latticeMap.mergePixels(bottomLeftPixel, topRightPixel);

    // make sure pixels are merged
    REQUIRE(latticeMap.isPixelMerged(bottomLeftPixel));
    REQUIRE(latticeMap.isPixelMerged(topRightPixel));

    SECTION("Make sure we get the correct number of merged pixel pairs") {
        const std::vector<std::pair<PixelRef, PixelRef>> &pixelPairs =
            latticeMap.getMergedPixelPairs();
        REQUIRE(pixelPairs.size() == 1);
        REQUIRE(pixelPairs[0].first == bottomLeftPixel);
        REQUIRE(pixelPairs[0].second == topRightPixel);
    }

    SECTION("Overwrite the pixelpair by re-merging the first pixel of the pair") {
        PixelRef aboveBottomLeftPixel =
            latticeMap.pixelate(Point2f(bottomLeft.x, bottomLeft.y + 1));

        // merge
        latticeMap.mergePixels(aboveBottomLeftPixel, topRightPixel);

        // make sure pixels are merged
        REQUIRE(latticeMap.isPixelMerged(aboveBottomLeftPixel));
        REQUIRE(latticeMap.isPixelMerged(topRightPixel));

        // and previous pixel is not merged any more
        REQUIRE(!latticeMap.isPixelMerged(bottomLeftPixel));

        // make sure we get the correct number of merged pixel pairs
        const std::vector<std::pair<PixelRef, PixelRef>> &pixelPairs =
            latticeMap.getMergedPixelPairs();
        REQUIRE(pixelPairs.size() == 1);
        REQUIRE(pixelPairs[0].first == aboveBottomLeftPixel);
        REQUIRE(pixelPairs[0].second == topRightPixel);
    }

    SECTION("Overwrite the pixelpair by re-merging the second pixel of the pair") {
        PixelRef belowTopRightPixel = latticeMap.pixelate(Point2f(topRight.x, topRight.y - 1));

        // merge
        latticeMap.mergePixels(bottomLeftPixel, belowTopRightPixel);

        // make sure pixels are merged
        REQUIRE(latticeMap.isPixelMerged(bottomLeftPixel));
        REQUIRE(latticeMap.isPixelMerged(belowTopRightPixel));

        // and previous pixel is not merged any more
        REQUIRE(!latticeMap.isPixelMerged(topRightPixel));

        // make sure we get the correct number of merged pixel pairs
        const std::vector<std::pair<PixelRef, PixelRef>> &pixelPairs2 =
            latticeMap.getMergedPixelPairs();
        REQUIRE(pixelPairs2.size() == 1);
        REQUIRE(pixelPairs2[0].first == bottomLeftPixel);
        REQUIRE(pixelPairs2[0].second == belowTopRightPixel);
    }

    SECTION("Merge the same pixel twice to erase the pair") {
        latticeMap.mergePixels(bottomLeftPixel, bottomLeftPixel);

        // make sure no pixel is merged
        REQUIRE(!latticeMap.isPixelMerged(bottomLeftPixel));
        REQUIRE(!latticeMap.isPixelMerged(topRightPixel));

        // make sure we get the correct number of merged pixel pairs
        const std::vector<std::pair<PixelRef, PixelRef>> &pixelPairs3 =
            latticeMap.getMergedPixelPairs();
        REQUIRE(pixelPairs3.size() == 0);
    }
}

TEST_CASE("LatticeMap copy()", "") {
    std::vector<Line4f> lines;
    lines.push_back(Line4f(Point2f(1.888668, 1.560937), Point2f(1.888668, 6.908548)));
    lines.push_back(Line4f(Point2f(1.888668, 6.908548), Point2f(7.882500, 6.908548)));
    lines.push_back(Line4f(Point2f(7.882500, 6.908548), Point2f(7.897130, 5.123703)));
    lines.push_back(Line4f(Point2f(4.813618, 5.139680), Point2f(7.897130, 5.123703)));
    lines.push_back(Line4f(Point2f(4.813618, 3.862943), Point2f(4.813618, 5.139680)));
    lines.push_back(Line4f(Point2f(4.813618, 3.862943), Point2f(6.068108, 3.848524)));
    lines.push_back(Line4f(Point2f(6.068108, 3.848524), Point2f(6.084223, 1.544019)));
    lines.push_back(Line4f(Point2f(1.888668, 1.560937), Point2f(6.084223, 1.544019)));
    ShapeMap shp;
    for (const auto &line : lines) {
        shp.makeLineShape(line);
    }

    LatticeMap pnt(shp.getRegion());
    pnt.setGrid(0.5);
    pnt.blockLines(lines);
    pnt.fillPoint(Point2f(3.0, 6.0));
    pnt.sparkGraph2(nullptr, false, -1);

    LatticeMap newPnt(shp.getRegion());
    newPnt.copy(pnt, true, true);

    Point2f p(3.01, 6.7);
    Region4f region(p, p);

    VGAMetricDepth(newPnt, newPnt.getPointsInRegion(region)).run(nullptr);
}
