// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2024 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/attributetableview.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("Test Attribute view") {
    AttributeTable table;

    table.insertOrResetColumn("foo");
    table.insertOrResetColumn("bar");
    table.addRow(AttributeKey(0)).setValue(0, 1.0f).setValue(1, 1.1f);
    table.addRow(AttributeKey(7)).setValue(0, 0.7f).setValue(1, 1.7f);

    AttributeTableView view(table);
    view.setDisplayColIndex(0);

    REQUIRE(view.getConstTableIndex().front().key.value == 7);

    REQUIRE(view.getNormalisedValue(view.getConstTableIndex().front().key,
                                    *view.getConstTableIndex().front().row) == Catch::Approx(0.0f));

    REQUIRE(&view.getDisplayParams() != &table.getDisplayParams());
    REQUIRE(&view.getDisplayParams() == &table.getColumn(0).getDisplayParams());

    table.addRow(AttributeKey(3));
    view.setDisplayColIndex(-1);
    REQUIRE(view.getNormalisedValue(AttributeKey(3), table.getRow(AttributeKey(3))) ==
            Catch::Approx(3.0 / 7));
    REQUIRE(view.getConstTableIndex().size() == 3);

    REQUIRE(&table.getDisplayParams() == &view.getDisplayParams());

    view.setDisplayColIndex(-2);
    REQUIRE(view.getNormalisedValue(AttributeKey(3), table.getRow(AttributeKey(3))) ==
            Catch::Approx(3.0 / 7));
    REQUIRE(view.getConstTableIndex().empty());

    REQUIRE(&table.getDisplayParams() == &view.getDisplayParams());
}

TEST_CASE("Test attribute table handle") {
    AttributeTable table;

    table.insertOrResetColumn("foo");
    table.insertOrResetColumn("bar");
    table.addRow(AttributeKey(0)).setValue(0, 1.0f).setValue(1, 1.1f);
    table.addRow(AttributeKey(7)).setValue(0, 0.7f).setValue(1, 1.7f);

    AttributeTableHandle handle(table);
    handle.setDisplayColIndex(0);

    REQUIRE(handle.getTableIndex().front().key.value == 7);
    REQUIRE(handle.getConstTableIndex().front().key.value == 7);

    handle.getTableIndex().front().mutableRow->setValue(0, 0.8f);

    REQUIRE(table.getRow(AttributeKey(7)).getValue(0) == Catch::Approx(0.8));

    handle.setDisplayColIndex(-1);
    REQUIRE(handle.getTableIndex().size() == 2);

    REQUIRE(&table.getDisplayParams() == &handle.getDisplayParams());

    handle.setDisplayColIndex(-2);
    REQUIRE(handle.getTableIndex().empty());

    REQUIRE(&table.getDisplayParams() == &handle.getDisplayParams());
}
