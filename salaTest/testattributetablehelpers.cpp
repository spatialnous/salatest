// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "Catch/fakeit.hpp"
#include "salalib/attributetablehelpers.hpp"

#include <cstddef>
#include <set>
#include <string>

TEST_CASE("push to layer") {
    using namespace dXreimpl;
    using namespace fakeit;
    // TODO: Warning from fakeit: Zero size arrays are an extension
    Mock<LayerManager> layMan;
    AttributeTable table;
    When(Method(layMan, addLayer)).Do([](const std::string &name) -> size_t {
        REQUIRE(name == "testlayer");
        return 1;
    });
    When(Method(layMan, getKey).Using(1)).AlwaysReturn(2);
    When(Method(layMan, isVisible).Using(1)).AlwaysReturn(true);
    When(Method(layMan, setLayerVisible)).AlwaysReturn();

    table.insertOrResetColumn("col1");
    table.getOrInsertColumn("col2");

    auto &row = table.addRow(AttributeKey(0));
    row.setValue(0, 0.5f);

    auto &row2 = table.addRow(AttributeKey(1));
    row2.setValue(0, 1.0f);

    std::set<int> selSet = {0};
    pushSelectionToLayer(table, layMan.get(), "testlayer", selSet);
    Verify(Method(layMan, addLayer)).Once();
    Verify(Method(layMan, getKey).Using(1)).Once();
    Verify(Method(layMan, setLayerVisible).Using(1, true)).Once();

    REQUIRE(row.getLayerKey() == 3);
    REQUIRE(row2.getLayerKey() == 1);
}
