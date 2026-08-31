// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/latticemap.hpp"
#include "salalib/pushvalues.hpp"
#include "salalib/shapegraph.hpp"
#include "salalib/shapemap.hpp"
#include "salalib/shapemapgroupdata.hpp"

#include "catch_amalgamated.hpp"

#include <cmath>
#include <limits>
#include <utility>
#include <vector>

TEST_CASE("Push values from shapemaps to VGA", "") {

    float vgaMinX = 0.00;
    float vgaMinY = 0.00;
    float vgaMaxX = 6.00;
    float vgaMaxY = 6.00;
    float cellSize = 1.0;
    float minorOffset =
        cellSize *
        0.05f; // used to make sure that shapes don't fall exactly on the lattice map pixels

    // The testing lattice map looks like below, filled at the 'o'
    //
    //         1   2   3   4   5
    //       |   |   |   |   |   |
    //     - + - + - + - + - + - + -
    //  1    | o | o | o | o | o |
    //     - + - + - + - + - + - + -
    //  2    | o | o | o | o | o |
    //     - + - + - + - + - + - + -
    //  3    | o | o | o | o | o |
    //     - + - + - + - + - + - + -
    //  4    | o | o | o | o | o |
    //     - + - + - + - + - + - + -
    //  5    | o | o | o | o | o |
    //     - + - + - + - + - + - + -
    //       |   |   |   |   |   |

    // in fact it should not be a requirement to make all these maps through
    // the metagraph, but instead through a "map pusher" of some sorts

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);
    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Drawing file";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Drawing Map");

    ShapeMap &drawingMap = spacePixels.back();

    // rectangle containing the filled area of the lattice map offset by 0.5 to
    // make sure it falls exactly on the edge of the 1.0-sized cell
    drawingMap.makePolyShape(
        {
            Point2f(vgaMinX + cellSize * 0.5, vgaMinY + cellSize * 0.5), //
            Point2f(vgaMinX + cellSize * 0.5, vgaMaxY - cellSize * 0.5), //
            Point2f(vgaMaxX - cellSize * 0.5, vgaMaxY - cellSize * 0.5), //
            Point2f(vgaMaxX - cellSize * 0.5, vgaMinY + cellSize * 0.5)  //
        },
        false);
    //    mgraph->updateParentRegions(drawingMap);

    LatticeMap vgaMap(drawingMap.getRegion(), "VGA Map");
    vgaMap.setGrid(1.0);
    std::vector<Line4f> lines = drawingMap.getAllShapesAsLines();
    vgaMap.blockLines(lines);
    vgaMap.makePoints(
        Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset, (vgaMinY + vgaMaxY) * 0.5 + minorOffset),
        0);
    vgaMap.sparkGraph2(nullptr, false, -1);
    AttributeTable &vgaTable = vgaMap.getAttributeTable();

    int minI = std::numeric_limits<int>::max();
    int minJ = std::numeric_limits<int>::max();
    int maxI = -std::numeric_limits<int>::max();
    int maxJ = -std::numeric_limits<int>::max();
    for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
        PixelRef key(vgaRowIter->getKey().value);
        if (key.x < minI)
            minI = key.x;
        if (key.y < minJ)
            minJ = key.y;
        if (key.x > maxI)
            maxI = key.x;
        if (key.y > maxJ)
            maxJ = key.y;
    }
    auto midI = static_cast<int>(floor((minI + maxI) * 0.5));
    auto midJ = static_cast<int>(floor((minJ + maxJ) * 0.5));

    std::string attributeName = "Shape Value";
    auto vgaAttrColIdx = vgaMap.addAttribute(attributeName);

    SECTION("Data map") {
        ShapeMap sourceMap("Test ShapeMap");

        auto sourceAttrColIdx = sourceMap.addAttribute(attributeName);

        for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
            REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == -1.0f);
        }

        SECTION("Single polygon input") {

            // main testing shape. drawn in a way so that it contains the pixels
            // that are not on the outer-edge of the lattice map
            sourceMap.makePolyShape(
                {
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset) //
                },
                false);

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);

            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::MAX);

            // all values are 1 (like the polygon) except from those on the edges
            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = 1;
                if (key.x == minI || key.x == maxI || key.y == minJ || key.y == maxJ) {
                    expectedValue = -1;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Two overlapping polygons input", "") {

            // left polygon
            sourceMap.makePolyShape(
                {
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset) //
                },
                false);

            // right polygon
            sourceMap.makePolyShape(
                {
                    Point2f((vgaMinX + vgaMaxX) * 0.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f((vgaMinX + vgaMaxX) * 0.5 - minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset) //
                },
                false);

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);
            sourceMap.getAttributeTable().getRow(AttributeKey(1)).setValue(sourceAttrColIdx, 2);

            SECTION("Shared border max function") {

                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MAX);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = 1;
                    if (key.x == minI || key.x == maxI || key.y == minJ || key.y == maxJ) {
                        // cells on the border
                        expectedValue = -1;
                    } else if (key.x > midI) {
                        // cells on the right polygon
                        expectedValue = 2;
                    } else if (key.x == midI) {
                        // cells on the shared border get the MAXIMUM value from the two polygons
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Shared border min function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MIN);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = 1;
                    if (key.x == minI || key.x == maxI || key.y == minJ || key.y == maxJ) {
                        // cells on the border
                        expectedValue = -1;
                    } else if (key.x > midI) {
                        // cells on the right polygon
                        expectedValue = 2;
                    } else if (key.x == midI) {
                        // cells on the shared border get the MINIMUM value from the two polygons
                        expectedValue = 1;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Shared border average function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::AVG);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = 1;
                    if (key.x == minI || key.x == maxI || key.y == minJ || key.y == maxJ) {
                        // cells on the border
                        expectedValue = -1;
                    } else if (key.x > midI) {
                        // cells on the right polygon
                        expectedValue = 2;
                    } else if (key.x == midI) {
                        // cells on the shared border get the AVERAGE value of the two polygons
                        expectedValue = 1.5;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Shared border total function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::TOT);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = 1;
                    if (key.x == minI || key.x == maxI || key.y == minJ || key.y == maxJ) {
                        // cells on the border
                        expectedValue = -1;
                    } else if (key.x > midI) {
                        // cells on the right polygon
                        expectedValue = 2;
                    } else if (key.x == midI) {
                        // cells on the shared border get the TOTAL value of the two polygons
                        expectedValue = 3;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }
        }

        SECTION("Single line input", "") {

            // vertical line
            sourceMap.makeLineShape({
                Point2f((vgaMinX + vgaMaxX) * 0.5 - minorOffset, vgaMinY + minorOffset), //
                Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset, vgaMaxY - minorOffset)  //
            });

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);

            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::MAX);

            // all values are -1 except from those under the line (1)
            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == midI) {
                    expectedValue = 1;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Crossing lines input", "") {

            // vertical line
            sourceMap.makeLineShape({
                Point2f((vgaMinX + vgaMaxX) * 0.5 - minorOffset, vgaMinY + minorOffset), //
                Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset, vgaMaxY - minorOffset)  //
            });

            // horizontal line
            sourceMap.makeLineShape({
                Point2f(vgaMinX + minorOffset, (vgaMinY + vgaMaxY) * 0.5 - minorOffset), //
                Point2f(vgaMaxX - minorOffset, (vgaMinY + vgaMaxY) * 0.5 + minorOffset)  //
            });

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);
            sourceMap.getAttributeTable().getRow(AttributeKey(1)).setValue(sourceAttrColIdx, 2);

            SECTION("Crossing lines max function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MAX);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if (key.x == midI && key.y == midJ) {
                        // cells on line intersection get the MAXIMUM value of the two
                        expectedValue = 2;
                    } else if (key.x == midI) {
                        // vertical line
                        expectedValue = 1;
                    } else if (key.y == midJ) {
                        // horizontal line
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing lines min function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MIN);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if (key.x == midI && key.y == midJ) {
                        // cells on line intersection get the MINIMUM value of the two
                        expectedValue = 1;
                    } else if (key.x == midI) {
                        // vertical line
                        expectedValue = 1;
                    } else if (key.y == midJ) {
                        // horizontal line
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing lines average function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::AVG);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if (key.x == midI && key.y == midJ) {
                        // cells on line intersection get the AVERAGE value of the two
                        expectedValue = 1.5;
                    } else if (key.x == midI) {
                        // vertical line
                        expectedValue = 1;
                    } else if (key.y == midJ) {
                        // horizontal line
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing lines total function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::TOT);

                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if (key.x == midI && key.y == midJ) {
                        // cells on line intersection get the TOTAL value of the two
                        expectedValue = 3;
                    } else if (key.x == midI) {
                        // vertical line
                        expectedValue = 1;
                    } else if (key.y == midJ) {
                        // horizontal line
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }
        }

        SECTION("Single open polyline input", "") {

            // L shape
            sourceMap.makePolyShape(
                {
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset) //
                },
                true);

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);

            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::MAX);

            // all values are -1 except from those under the line (1)
            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == minI || key.y == maxJ) {
                    expectedValue = 1;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Crossing open polylines input", "") {

            // L shape
            sourceMap.makePolyShape(
                {
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset) //
                },
                true);

            // L shape rotated 180 degrees
            sourceMap.makePolyShape(
                {
                    Point2f(vgaMinX + cellSize * 1.5 - minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMinY + cellSize * 1.5 - minorOffset), //
                    Point2f(vgaMaxX - cellSize * 1.5 + minorOffset,
                            vgaMaxY - cellSize * 1.5 + minorOffset) //
                },
                true);

            sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);
            sourceMap.getAttributeTable().getRow(AttributeKey(1)).setValue(sourceAttrColIdx, 2);

            SECTION("Crossing open polylines max function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MAX);

                // all values are -1 except from those under the line (1)
                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if ((key.x == minI && key.y == minJ) || (key.x == maxI && key.y == maxJ)) {
                        // cells on line intersection get the MAXIMUM value of the two
                        expectedValue = 2;
                    } else if (key.x == minI || key.y == maxJ) {
                        // L shape
                        expectedValue = 1;
                    } else if (key.x == maxI || key.y == minJ) {
                        // L shape rotated 180 degrees
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing open polylines min function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::MIN);

                // all values are -1 except from those under the line (1)
                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if ((key.x == minI && key.y == minJ) || (key.x == maxI && key.y == maxJ)) {
                        // cells on line intersection get the MINUMUM value of the two
                        expectedValue = 1;
                    } else if (key.x == minI || key.y == maxJ) {
                        // L shape
                        expectedValue = 1;
                    } else if (key.x == maxI || key.y == minJ) {
                        // L shape rotated 180 degrees
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing open polylines average function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::AVG);

                // all values are -1 except from those under the line (1)
                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if ((key.x == minI && key.y == minJ) || (key.x == maxI && key.y == maxJ)) {
                        // cells on line intersection get the AVERAGE value of the two
                        expectedValue = 1.5;
                    } else if (key.x == minI || key.y == maxJ) {
                        // L shape
                        expectedValue = 1;
                    } else if (key.x == maxI || key.y == minJ) {
                        // L shape rotated 180 degrees
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }

            SECTION("Crossing open polylines total function") {
                PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                         PushValues::Func::TOT);

                // all values are -1 except from those under the line (1)
                for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end();
                     vgaRowIter++) {
                    PixelRef key(vgaRowIter->getKey().value);
                    float expectedValue = -1;
                    if ((key.x == minI && key.y == minJ) || (key.x == maxI && key.y == maxJ)) {
                        // cells on line intersection get the TOTAL value of the two
                        expectedValue = 3;
                    } else if (key.x == minI || key.y == maxJ) {
                        // L shape
                        expectedValue = 1;
                    } else if (key.x == maxI || key.y == minJ) {
                        // L shape rotated 180 degrees
                        expectedValue = 2;
                    }
                    REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
                }
            }
        }
    }

    SECTION("Axial map") {
        // the pushValues function takes the base shapemap so these sections are mainly to test
        // sending ShapeGraphs and setting the source map type to MetaGraph::VIEWAXIAL

        ShapeGraph sourceMap("Test Axial Map", ShapeMap::AXIALMAP);

        sourceMap.init(2, sourceMap.getRegion());
        sourceMap.initialiseAttributesAxial();

        for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
            REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == -1.0f);
        }

        // vertical line
        sourceMap.makeLineShape({
            Point2f((vgaMinX + vgaMaxX) * 0.5 - minorOffset, vgaMinY + minorOffset), //
            Point2f((vgaMinX + vgaMaxX) * 0.5 + minorOffset, vgaMaxY - minorOffset)  //
        });

        // horizontal line
        sourceMap.makeLineShape({
            Point2f(vgaMinX + minorOffset, (vgaMinY + vgaMaxY) * 0.5 - minorOffset), //
            Point2f(vgaMaxX - minorOffset, (vgaMinY + vgaMaxY) * 0.5 + minorOffset)  //
        });

        auto sourceAttrColIdx = sourceMap.addAttribute(attributeName);

        sourceMap.getAttributeTable().getRow(AttributeKey(0)).setValue(sourceAttrColIdx, 1);
        sourceMap.getAttributeTable().getRow(AttributeKey(1)).setValue(sourceAttrColIdx, 2);

        sourceMap.makeConnections();

        REQUIRE(sourceMap.getAttributeTable().hasColumn("Connectivity"));

        SECTION("Crossing lines max function") {
            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::MAX);

            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == midI && key.y == midJ) {
                    // cells on line intersection get the MAXIMUM value of the two
                    expectedValue = 2;
                } else if (key.x == midI) {
                    // vertical line
                    expectedValue = 1;
                } else if (key.y == midJ) {
                    // horizontal line
                    expectedValue = 2;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Crossing lines min function") {
            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::MIN);

            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == midI && key.y == midJ) {
                    // cells on line intersection get the MINIMUM value of the two
                    expectedValue = 1;
                } else if (key.x == midI) {
                    // vertical line
                    expectedValue = 1;
                } else if (key.y == midJ) {
                    // horizontal line
                    expectedValue = 2;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Crossing lines average function") {
            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::AVG);

            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == midI && key.y == midJ) {
                    // cells on line intersection get the AVERAGE value of the two
                    expectedValue = 1.5;
                } else if (key.x == midI) {
                    // vertical line
                    expectedValue = 1;
                } else if (key.y == midJ) {
                    // horizontal line
                    expectedValue = 2;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }

        SECTION("Crossing lines total function") {
            PushValues::shapeToPoint(sourceMap, attributeName, vgaMap, attributeName,
                                     PushValues::Func::TOT);

            for (auto vgaRowIter = vgaTable.begin(); vgaRowIter != vgaTable.end(); vgaRowIter++) {
                PixelRef key(vgaRowIter->getKey().value);
                float expectedValue = -1;
                if (key.x == midI && key.y == midJ) {
                    // cells on line intersection get the TOTAL value of the two
                    expectedValue = 3;
                } else if (key.x == midI) {
                    // vertical line
                    expectedValue = 1;
                } else if (key.y == midJ) {
                    // horizontal line
                    expectedValue = 2;
                }
                REQUIRE(vgaRowIter->getRow().getValue(vgaAttrColIdx) == expectedValue);
            }
        }
    }
}
