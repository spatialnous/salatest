// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/genlib/bsptree.hpp"
#include "salalib/genlib/comm.hpp"
#include "salalib/genlib/line4f.hpp"

#include "catch_amalgamated.hpp"

#include <memory>
#include <utility>
#include <vector>

TEST_CASE("BSPTree::pickMidpointLine") {
    std::vector<Line4f> lines;
    lines.push_back(Line4f(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line4f(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line4f(Point2f(3, 2), Point2f(4, 2)));

    BSPNode node;

    REQUIRE(BSPTree::pickMidpointLine(lines, nullptr) == 1);

    SECTION("Additional lines") {
        lines.push_back(Line4f(Point2f(4, 2), Point2f(5, 2)));
        REQUIRE(BSPTree::pickMidpointLine(lines, nullptr) == 1);

        lines.push_back(Line4f(Point2f(5, 1), Point2f(6, 1)));
        REQUIRE(BSPTree::pickMidpointLine(lines, nullptr) == 2);

        // the only line with height > width becomes chosen
        lines.push_back(Line4f(Point2f(15, 4), Point2f(15, 0)));
        REQUIRE(BSPTree::pickMidpointLine(lines, nullptr) == 5);
    }
    SECTION("rotated middle") {

        // height > width, rotated, close to midpoint
        lines.push_back(Line4f(Point2f(4.5, 1), Point2f(4.5, 3)));

        lines.push_back(Line4f(Point2f(5, 2), Point2f(6, 2)));
        lines.push_back(Line4f(Point2f(6, 2), Point2f(7, 2)));

        // height > width, rotated, not close to midpoint
        lines.push_back(Line4f(Point2f(6.5, 1), Point2f(6.5, 3)));

        REQUIRE(BSPTree::pickMidpointLine(lines, nullptr) == 3);
    }
}

static void compareLines(const Line4f &l1, const Line4f &l2, float epsilon) {
    REQUIRE(l1.start().x == Catch::Approx(l2.start().x).epsilon(epsilon));
    REQUIRE(l1.start().y == Catch::Approx(l2.start().y).epsilon(epsilon));
    REQUIRE(l1.end().x == Catch::Approx(l2.end().x).epsilon(epsilon));
    REQUIRE(l1.end().y == Catch::Approx(l2.end().y).epsilon(epsilon));
}

TEST_CASE("BSPTree::makeLines") {
    const float epsilon = 0.001f;
    typedef std::pair<std::vector<Line4f>, std::vector<Line4f>> LineVecPair;

    std::vector<Line4f> lines;
    lines.push_back(Line4f(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line4f(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line4f(Point2f(3, 2), Point2f(4, 2)));
    lines.push_back(Line4f(Point2f(4, 2), Point2f(5, 2)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    LineVecPair result = BSPTree::makeLines(nullptr, 0, lines, node.get());

    REQUIRE(result.first.size() == 3);
    REQUIRE(result.second.size() == 0);

    compareLines(result.first[0], lines[0], epsilon);
    compareLines(result.first[1], lines[2], epsilon);
    compareLines(result.first[2], lines[3], epsilon);

    SECTION("One on the right") {
        lines.push_back(Line4f(Point2f(5, 1), Point2f(6, 1)));

        result = BSPTree::makeLines(nullptr, 0, lines, node.get());

        REQUIRE(result.first.size() == 3);
        REQUIRE(result.second.size() == 1);

        compareLines(result.second[0], lines[4], epsilon);
    }
    SECTION("One line with height > width becomes chosen") {
        // height > width, rotated, not close to midpoint
        lines.push_back(Line4f(Point2f(5.5, 1), Point2f(5.5, 3)));

        lines.push_back(Line4f(Point2f(6, 2), Point2f(7, 2)));

        result = BSPTree::makeLines(nullptr, 0, lines, node.get());

        REQUIRE(result.first.size() == 4);
        REQUIRE(result.second.size() == 1);

        compareLines(result.first[0], lines[0], epsilon);
        compareLines(result.first[1], lines[1], epsilon);
        compareLines(result.first[2], lines[2], epsilon);
        compareLines(result.first[3], lines[3], epsilon);
        compareLines(result.second[0], lines[5], epsilon);
    }

    SECTION("One broken between") {
        // height > width, rotated, close to midpoint
        lines.push_back(Line4f(Point2f(5.5, 1), Point2f(5.5, 3)));

        lines.push_back(Line4f(Point2f(6, 2), Point2f(7, 2)));
        lines.push_back(Line4f(Point2f(7, 2), Point2f(8, 2)));
        lines.push_back(Line4f(Point2f(8, 2), Point2f(9, 2)));
        lines.push_back(Line4f(Point2f(9, 2), Point2f(10, 2)));

        // line with two points at different sides of chosen
        lines.push_back(Line4f(Point2f(3, -2), Point2f(6, -2)));

        result = BSPTree::makeLines(nullptr, 0, lines, node.get());

        // adds one on each side
        REQUIRE(result.first.size() == 5);
        REQUIRE(result.second.size() == 5);

        compareLines(result.first[0], lines[0], epsilon);
        compareLines(result.first[1], lines[1], epsilon);
        compareLines(result.first[2], lines[2], epsilon);
        compareLines(result.first[3], lines[3], epsilon);

        compareLines(result.second[0], lines[5], epsilon);
        compareLines(result.second[1], lines[6], epsilon);
        compareLines(result.second[2], lines[7], epsilon);
        compareLines(result.second[3], lines[8], epsilon);

        compareLines(result.first[4], Line4f(Point2f(3, -2), Point2f(5.5, -2)), epsilon);
        compareLines(result.second[4], Line4f(Point2f(5.5, -2), Point2f(6, -2)), epsilon);
    }
}

TEST_CASE("BSPTree::make (all horizontal lines)", "all-left tree") {
    const float epsilon = 0.001f;

    std::vector<Line4f> lines;
    lines.push_back(Line4f(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line4f(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line4f(Point2f(3, 2), Point2f(4, 2)));
    lines.push_back(Line4f(Point2f(4, 2), Point2f(5, 2)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    BSPTree::make(nullptr, 0, lines, node.get());

    compareLines(node->getLine(), lines[1], epsilon);

    REQUIRE(node->left != nullptr);
    REQUIRE(node->right == nullptr);

    compareLines(node->left->getLine(), lines[0], epsilon);

    REQUIRE(node->left->left != nullptr);
    REQUIRE(node->left->right == nullptr);

    compareLines(node->left->left->getLine(), lines[2], epsilon);

    REQUIRE(node->left->left->left != nullptr);
    REQUIRE(node->left->left->right == nullptr);

    compareLines(node->left->left->left->getLine(), lines[3], epsilon);

    REQUIRE(node->left->left->left->left == nullptr);
    REQUIRE(node->left->left->left->right == nullptr);
}

TEST_CASE("BSPTree::make (all vertical lines)", "split tree") {
    const float epsilon = 0.001f;

    std::vector<Line4f> lines;
    lines.push_back(Line4f(Point2f(1.5, 1), Point2f(1.5, 3)));
    lines.push_back(Line4f(Point2f(2.5, 1), Point2f(2.5, 3)));
    lines.push_back(Line4f(Point2f(3.5, 1), Point2f(3.5, 3)));
    lines.push_back(Line4f(Point2f(4.5, 1), Point2f(4.5, 3)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    BSPTree::make(nullptr, 0, lines, node.get());

    compareLines(node->getLine(), lines[1], epsilon);

    REQUIRE(node->left != nullptr);
    REQUIRE(node->right != nullptr);

    compareLines(node->left->getLine(), lines[0], epsilon);
    compareLines(node->right->getLine(), lines[2], epsilon);

    REQUIRE(node->left->left == nullptr);
    REQUIRE(node->left->right == nullptr);

    REQUIRE(node->right->left == nullptr);
    REQUIRE(node->right->right != nullptr);

    compareLines(node->right->right->getLine(), lines[3], epsilon);

    REQUIRE(node->right->right->left == nullptr);
    REQUIRE(node->right->right->right == nullptr);
}
