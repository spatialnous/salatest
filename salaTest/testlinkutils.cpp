// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/linkutils.hpp"

#include "catch_amalgamated.hpp"
#include "salalib/shapemapgroupdata.hpp"

TEST_CASE("Test linking - fully filled grid (no geometry)", "") {
    double spacing = 0.5;
    Point2f offset(0, 0); // seems that this is always set to 0,0
    Point2f bottomLeft(0, 0);
    Point2f topRight(2, 4);
    int fillType = 0; // = QDepthmapView::FULLFILL

    PointMap pointMap(Region4f(bottomLeft, topRight), "Test PointMap");
    pointMap.setGrid(spacing, offset);
    Point2f gridBottomLeft = pointMap.getRegion().bottomLeft;
    Point2f midPoint(
        gridBottomLeft.x + spacing * (floor(static_cast<double>(pointMap.getCols()) * 0.5) + 0.5),
        gridBottomLeft.y + spacing * (floor(static_cast<double>(pointMap.getRows()) * 0.5) + 0.5));
    std::vector<Line4f> lines;
    pointMap.blockLines(lines);
    pointMap.makePoints(midPoint, fillType);

    std::vector<Line4f> mergeLines;

    SECTION("Successful: bottom-left to top-right") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE(links.size() == 1);
        REQUIRE(links[0].a.x == 0);
        REQUIRE(links[0].a.y == 0);
        REQUIRE(links[0].b.x == 4);
        REQUIRE(links[0].b.y == 8);

        // make sure pixels are not already merged
        REQUIRE(!pointMap.isPixelMerged(links[0].a));
        REQUIRE(!pointMap.isPixelMerged(links[0].b));

        // merge
        depthmapX::mergePixelPairs(links, pointMap);

        // make sure pixels are merged
        REQUIRE(pointMap.isPixelMerged(links[0].a));
        REQUIRE(pointMap.isPixelMerged(links[0].b));

        const std::vector<std::pair<PixelRef, PixelRef>> &mergedPixelPairs =
            pointMap.getMergedPixelPairs();

        REQUIRE(mergedPixelPairs.size() == 1);
        REQUIRE(mergedPixelPairs[0].first.x == 0);
        REQUIRE(mergedPixelPairs[0].first.y == 0);
        REQUIRE(mergedPixelPairs[0].second.x == 4);
        REQUIRE(mergedPixelPairs[0].second.y == 8);

        const std::vector<SimpleLine> &newMergeLines = depthmapX::getMergedPixelsAsLines(pointMap);

        Point2f p1position = pointMap.depixelate(links[0].a);
        Point2f p2position = pointMap.depixelate(links[0].b);

        REQUIRE(newMergeLines.size() == 1);
        REQUIRE(newMergeLines[0].start().x == p1position.x);
        REQUIRE(newMergeLines[0].start().y == p1position.y);
        REQUIRE(newMergeLines[0].end().x == p2position.x);
        REQUIRE(newMergeLines[0].end().y == p2position.y);
    }

    SECTION("Successfull: bottom-left to top-right and bottom-right to top-left") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        Point2f start(topRight.x, bottomLeft.y);
        Point2f end(bottomLeft.x, topRight.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE(links.size() == 2);
        REQUIRE(links[0].a.x == 0);
        REQUIRE(links[0].a.y == 0);
        REQUIRE(links[0].b.x == 4);
        REQUIRE(links[0].b.y == 8);
        REQUIRE(links[1].a.x == 0);
        REQUIRE(links[1].a.y == 8);
        REQUIRE(links[1].b.x == 4);
        REQUIRE(links[1].b.y == 0);

        // make sure pixels are not already merged
        REQUIRE(!pointMap.isPixelMerged(links[0].a));
        REQUIRE(!pointMap.isPixelMerged(links[0].b));
        REQUIRE(!pointMap.isPixelMerged(links[1].a));
        REQUIRE(!pointMap.isPixelMerged(links[1].b));

        // merge
        depthmapX::mergePixelPairs(links, pointMap);

        // make sure pixels are merged
        REQUIRE(pointMap.isPixelMerged(links[0].a));
        REQUIRE(pointMap.isPixelMerged(links[0].b));
        REQUIRE(pointMap.isPixelMerged(links[1].a));
        REQUIRE(pointMap.isPixelMerged(links[1].b));
    }

    SECTION("Failing: merge line start out of grid") {
        Point2f start(bottomLeft.x - spacing, bottomLeft.y - spacing);
        mergeLines.push_back(Line4f(start, topRight));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(
            depthmapX::mergePixelPairs(links, pointMap),
            Catch::Matchers::ContainsSubstring("Line ends not both on painted analysis space"));
    }

    SECTION("Failing: merge line end out of grid") {
        Point2f end(topRight.x + spacing, topRight.y + spacing);
        mergeLines.push_back(Line4f(bottomLeft, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(
            depthmapX::mergePixelPairs(links, pointMap),
            Catch::Matchers::ContainsSubstring("Line ends not both on painted analysis space"));
    }

    SECTION("Failing: second link start overlapping") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        Point2f start(bottomLeft.x, bottomLeft.y);
        Point2f end(topRight.x - 1, topRight.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(depthmapX::mergePixelPairs(links, pointMap),
                            Catch::Matchers::ContainsSubstring("Overlapping link found"));
    }

    SECTION("Failing: second link end overlapping") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        Point2f start(bottomLeft.x + 1, bottomLeft.y);
        Point2f end(topRight.x, topRight.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(depthmapX::mergePixelPairs(links, pointMap),
                            Catch::Matchers::ContainsSubstring("Overlapping link found"));
    }

    SECTION("Failing: fully overlapping link (bottom-left to top-right)") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(depthmapX::mergePixelPairs(links, pointMap),
                            Catch::Matchers::ContainsSubstring("Overlapping link found"));
    }

    SECTION("Failing: link overlapping to previously merged") {
        mergeLines.push_back(Line4f(bottomLeft, topRight));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE(links.size() == 1);
        REQUIRE(links[0].a.x == 0);
        REQUIRE(links[0].a.y == 0);
        REQUIRE(links[0].b.x == 4);
        REQUIRE(links[0].b.y == 8);

        // make sure pixels are not already merged
        REQUIRE(!pointMap.isPixelMerged(links[0].a));
        REQUIRE(!pointMap.isPixelMerged(links[0].b));

        // merge
        depthmapX::mergePixelPairs(links, pointMap);

        // make sure pixels are merged
        REQUIRE(pointMap.isPixelMerged(links[0].a));
        REQUIRE(pointMap.isPixelMerged(links[0].b));

        // now try to merge the same link again
        REQUIRE_THROWS_WITH(depthmapX::mergePixelPairs(links, pointMap),
                            Catch::Matchers::ContainsSubstring(
                                "Link pixel found that is already linked on the map"));
    }
}

TEST_CASE("Test linking - half filled grid", "") {

    double spacing = 0.5;
    Point2f offset(0, 0); // seems that this is always set to 0,0
    int fillType = 0;     // = QDepthmapView::FULLFILL

    Point2f lineStart(0, 0);
    Point2f lineEnd(2, 4);

    Point2f bottomLeft(std::min(lineStart.x, lineEnd.x), std::min(lineStart.y, lineEnd.y));
    Point2f topRight(std::max(lineStart.x, lineEnd.x), std::max(lineStart.y, lineEnd.y));

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");

    spacePixels.back().makeLineShape(Line4f(lineStart, lineEnd));
    spacePixelFileData.region = spacePixels.back().getRegion();

    PointMap pointMap(Region4f(bottomLeft, topRight), "Test PointMap");
    pointMap.setGrid(spacing, offset);

    Point2f gridBottomLeft = pointMap.getRegion().bottomLeft;
    Point2f gridTopRight = pointMap.getRegion().topRight;
    Point2f topLeftFillPoint(gridBottomLeft.x + spacing, gridTopRight.y - spacing);
    std::vector<Line4f> lines = spacePixels.back().getAllShapesAsLines();
    pointMap.blockLines(lines);
    pointMap.makePoints(topLeftFillPoint, fillType);

    std::vector<Line4f> mergeLines;

    SECTION("Successful: top-left pixel to one to its right") {
        Point2f start(bottomLeft.x, topRight.y);
        Point2f end(bottomLeft.x + spacing, topRight.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE(links.size() == 1);
        REQUIRE(links[0].a.x == 0);
        REQUIRE(links[0].a.y == 8);
        REQUIRE(links[0].b.x == 1);
        REQUIRE(links[0].b.y == 8);

        // make sure pixels are not already merged
        REQUIRE(!pointMap.isPixelMerged(links[0].a));
        REQUIRE(!pointMap.isPixelMerged(links[0].b));

        // merge
        depthmapX::mergePixelPairs(links, pointMap);

        // make sure pixels are merged
        REQUIRE(pointMap.isPixelMerged(links[0].a));
        REQUIRE(pointMap.isPixelMerged(links[0].b));
    }

    SECTION("Failing: merge line (bottom-right to the one its left) completely out of grid") {
        Point2f start(topRight.x, bottomLeft.y);
        Point2f end(topRight.x - 1, bottomLeft.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(
            depthmapX::mergePixelPairs(links, pointMap),
            Catch::Matchers::ContainsSubstring("Line ends not both on painted analysis space"));
    }

    SECTION("Failing: merge line (bottom-right to top-left) start out of grid") {
        Point2f start(topRight.x, bottomLeft.y);
        Point2f end(bottomLeft.x, topRight.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(
            depthmapX::mergePixelPairs(links, pointMap),
            Catch::Matchers::ContainsSubstring("Line ends not both on painted analysis space"));
    }

    SECTION("Failing: merge line (top-left to bottom-right) end out of grid") {
        Point2f start(bottomLeft.x, topRight.y);
        Point2f end(topRight.x, bottomLeft.y);
        mergeLines.push_back(Line4f(start, end));
        std::vector<PixelRefPair> links = depthmapX::pixelateMergeLines(mergeLines, pointMap);
        REQUIRE_THROWS_WITH(
            depthmapX::mergePixelPairs(links, pointMap),
            Catch::Matchers::ContainsSubstring("Line ends not both on painted analysis space"));
    }
}
