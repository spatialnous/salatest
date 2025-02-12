// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2024 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/attributetableindex.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("Check index creation") {
    AttributeTable table;
    table.getOrInsertColumn("col1");
    table.getOrInsertColumn("col2");

    auto &row0 = table.addRow(AttributeKey(0));
    auto &row1 = table.addRow(AttributeKey(1));
    auto &row2 = table.addRow(AttributeKey(2));
    auto &row3 = table.addRow(AttributeKey(3));

    row0.setValue(0, 10.0);
    row1.setValue(0, 8.5);
    row2.setValue(0, 11.0);
    row3.setValue(0, 4.5);

    auto index = makeAttributeIndex(table, 0);
    REQUIRE(index.size() == 4);
    REQUIRE(index[0].key.value == 3);
    REQUIRE(index[1].key.value == 1);
    REQUIRE(index[2].key.value == 0);
    REQUIRE(index[3].key.value == 2);

    index[3].mutableRow->setValue(1, 1.5);
    REQUIRE(table.getRow(AttributeKey(2)).getValue(1) == Catch::Approx(1.5));
}
