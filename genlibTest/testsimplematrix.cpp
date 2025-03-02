// SPDX-FileCopyrightText: 2018 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/genlib/simplematrix.hpp"

#include "catch_amalgamated.hpp"

#include <algorithm>
#include <vector>

template <typename T>
void compareMatrixContent(depthmapX::BaseMatrix<T> const &matrix, std::vector<T> const &expected) {
    REQUIRE(matrix.size() == expected.size());
    std::vector<T> result(matrix.size());
    std::copy(matrix.begin(), matrix.end(), result.begin());
    REQUIRE(result == expected);
}

// Function declared separately as it performs unsafe operations (accessing fields
// of moved objects) and will thus be easier to exclude from static analysis
void testUnsafeRowMatrixCopyMove() {
    depthmapX::RowMatrix<std::string> matrix(2, 3);
    matrix(0, 0) = "0,0";
    matrix(1, 0) = "1,0";
    matrix(0, 1) = "0,1";
    matrix(1, 1) = "1,1";
    matrix(1, 2) = "1,2";
    matrix(0, 2) = "0,2";

    std::vector<std::string> expected{"0,0", "0,1", "0,2", "1,0", "1,1", "1,2"};
    compareMatrixContent(matrix, expected);

    depthmapX::RowMatrix<std::string> copy(matrix);
    compareMatrixContent(matrix, expected);
    compareMatrixContent(copy, expected);

    depthmapX::RowMatrix<std::string> clone(std::move(copy));
    compareMatrixContent(clone, expected);
    REQUIRE(copy.size() == 0);

    copy = clone;
    compareMatrixContent(copy, expected);
    REQUIRE(copy.columns() == 3);
    REQUIRE(copy.rows() == 2);
    compareMatrixContent(clone, expected);

    depthmapX::RowMatrix<std::string> assignMove(1, 1);
    assignMove = std::move(copy);
    compareMatrixContent(assignMove, expected);
    REQUIRE(copy.size() == 0);
}

TEST_CASE("Row matrix test assignemnt copy and move") { testUnsafeRowMatrixCopyMove(); }

TEST_CASE("Row matrix test exceptions") {
    depthmapX::RowMatrix<int> matrix(2, 3);
    matrix(0, 0) = 1;
    matrix(1, 2) = -1;
    matrix(0, 1) = 2;
    matrix(0, 2) = 3;
    matrix(1, 0) = -23;
    matrix(1, 1) = 0;

    REQUIRE(matrix(1, 2) == -1);

    compareMatrixContent(matrix, std::vector<int>{1, 2, 3, -23, 0, -1});

    REQUIRE_THROWS_WITH(matrix(5, 0), Catch::Matchers::ContainsSubstring("row out of range"));
    REQUIRE_THROWS_WITH(matrix(0, 5), Catch::Matchers::ContainsSubstring("column out of range"));
}

// Function declared separately as it performs unsafe operations (accessing fields
// of moved objects) and will thus be easier to exclude from static analysis
void testUnsafeColumnMatrixCopyMove() {
    depthmapX::ColumnMatrix<std::string> matrix(2, 3);
    matrix(0, 0) = "0,0";
    matrix(1, 0) = "1,0";
    matrix(0, 1) = "0,1";
    matrix(1, 1) = "1,1";
    matrix(1, 2) = "1,2";
    matrix(0, 2) = "0,2";

    std::vector<std::string> expected{"0,0", "1,0", "0,1", "1,1", "0,2", "1,2"};
    compareMatrixContent(matrix, expected);

    depthmapX::ColumnMatrix<std::string> copy(matrix);
    compareMatrixContent(matrix, expected);
    compareMatrixContent(copy, expected);

    depthmapX::ColumnMatrix<std::string> clone(std::move(copy));
    compareMatrixContent(clone, expected);
    REQUIRE(copy.size() == 0);

    copy = clone;
    compareMatrixContent(copy, expected);
    REQUIRE(copy.columns() == 3);
    REQUIRE(copy.rows() == 2);
    compareMatrixContent(clone, expected);

    depthmapX::ColumnMatrix<std::string> assignMove(1, 1);
    assignMove = std::move(copy);
    compareMatrixContent(assignMove, expected);
    REQUIRE(copy.size() == 0);
}
TEST_CASE("Column matrix test assignemnt copy and move") { testUnsafeColumnMatrixCopyMove(); }

TEST_CASE("Column matrix test exceptions") {
    depthmapX::ColumnMatrix<int> matrix(2, 3);
    matrix(0, 0) = 1;
    matrix(1, 2) = -1;
    matrix(0, 1) = 2;
    matrix(0, 2) = 3;
    matrix(1, 0) = -23;
    matrix(1, 1) = 0;

    REQUIRE(matrix(1, 2) == -1);

    compareMatrixContent(matrix, std::vector<int>{1, -23, 2, 0, 3, -1});

    REQUIRE_THROWS_WITH(matrix(5, 0), Catch::Matchers::ContainsSubstring("row out of range"));
    REQUIRE_THROWS_WITH(matrix(0, 5), Catch::Matchers::ContainsSubstring("column out of range"));
}

TEST_CASE("Fill and reset") {
    depthmapX::ColumnMatrix<int> matrix(2, 3);
    matrix.initialiseValues(-42);
    compareMatrixContent(matrix, std::vector<int>(6, -42));

    matrix.reset(3, 4);
    REQUIRE(matrix.rows() == 3);
    REQUIRE(matrix.columns() == 4);

    matrix.initialiseValues(12);
    compareMatrixContent(matrix, std::vector<int>(12, 12));
}
