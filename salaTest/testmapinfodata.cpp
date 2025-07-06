// SPDX-FileCopyrightText: 2017-2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "salalib/parsers/mapinfodata.hpp"
#include "salalib/shapemap.hpp"

#include "catch_amalgamated.hpp"

TEST_CASE("MapInfo failing header", "") {
    std::string mifdata = "Version 300\n";

    SECTION("Missing quotes around delimiter") {
        mifdata += "Charset \"WindowsLatin1\"\n"
                   "Delimiter ,\n"
                   "Index 1,2\n"
                   "CoordSys Earth Projection 8, 79, \"m\", -2, 49, 0.9996012717, 400000, -100000";
    }

    SECTION("Missing CoordSys") {
        mifdata += "Charset \"WindowsLatin1\"\n"
                   "Delimiter \",\"\n"
                   "Index 1,2\n"
                   "Bounds (-7845061.1011, -15524202.1641) (8645061.1011, 4470074.53373)\n";
    }

    std::stringstream mifstream(mifdata);

    MapInfoData mapinfodata;
    REQUIRE_FALSE(mapinfodata.readheader(mifstream));
}

TEST_CASE("MapInfo failing column attribute columns", "") {

    std::string mifdata = "";

    SECTION("Missing Columns at beginning") {
        mifdata += "Tolumns 2\n"
                   "  ID Integer\n"
                   "  Length_m Float\n"
                   "Data\n";
    }

    SECTION("Missing Column number") {
        mifdata += "Columns\n"
                   "  ID Integer\n"
                   "  Length_m Float\n"
                   "Data\n";
    }

    SECTION("Missing Data at end") {
        mifdata += "Columns 2\n"
                   "  ID Integer\n"
                   "  Length_m Float\n"
                   "Bata\n";
    }

    std::string middata = "1,1017.81\n"
                          "2,568.795\n"
                          "3,216.026";

    std::stringstream mifstream(mifdata);
    std::stringstream midstream(middata);

    std::vector<std::string> columnheads;

    MapInfoData mapinfodata;
    REQUIRE_FALSE(mapinfodata.readcolumnheaders(mifstream, columnheads));
}

TEST_CASE("MapInfo MID file with empty column", "") {
    const float epsilon = 0.001f;

    // A typical MIF

    std::string mifdata =
        "Version 300\n"
        "Charset \"WindowsLatin1\"\n"
        "Delimiter \",\"\n"
        "Index 1,2,3,4\n"
        "CoordSys Earth Projection 8, 79, \"m\", -2, 49, 0.9996012717, 400000, -100000\n";

    mifdata += "Columns 4\n"
               "  ID Integer\n"
               "  Length_m Float\n"
               "  Height_m Float\n"
               "  Width_m Float\n"
               "Data\n"
               "\n"
               "Line 534014.29 182533.33 535008.52 182764.11\n"
               "    Pen (1,2,0)\n"
               "Line 533798.68 183094.69 534365.48 183159.01\n"
               "    Pen (1,2,0)\n"
               "Point 534014.29 182533.33\n"
               "    Symbol (34,0,12)";

    // A MID with empty columns

    std::string middata = "1,1017.81,,\n"
                          "2,568.795,,\n"
                          "3,216.026,,";

    ShapeMap shapeMap("MapInfoTest");
    MapInfoData mapinfodata;

    std::stringstream mifstream(mifdata);
    std::stringstream midstream(middata);
    REQUIRE(mapinfodata.import(mifstream, midstream, shapeMap) == MINFO_OK);

    AttributeTable &att = shapeMap.getAttributeTable();

    REQUIRE(att.getNumColumns() == 4);
    REQUIRE(att.getColumn(0).getName() == "Id");
    REQUIRE(att.getColumn(1).getName() == "Length_M");
    REQUIRE(att.getColumn(2).getName() == "Height_M");
    REQUIRE(att.getColumn(3).getName() == "Width_M");

    std::map<int, SalaShape> shapes = shapeMap.getAllShapes();
    auto shapeRef0 = genlib::getMapAtIndex(shapes, 0);
    auto shapeRef1 = genlib::getMapAtIndex(shapes, 1);
    auto shapeRef2 = genlib::getMapAtIndex(shapes, 2);

    REQUIRE(att.getNumRows() == 3);
    auto &row0 = att.getRow(AttributeKey(shapeRef0->first));
    auto &row1 = att.getRow(AttributeKey(shapeRef1->first));
    auto &row2 = att.getRow(AttributeKey(shapeRef2->first));

    REQUIRE(row0.getValue("Id") == 1);
    REQUIRE(row1.getValue("Id") == 2);
    REQUIRE(row2.getValue("Id") == 3);
    REQUIRE(row0.getValue("Length_M") == Catch::Approx(1017.81).epsilon(epsilon));
    REQUIRE(row1.getValue("Length_M") == Catch::Approx(568.795).epsilon(epsilon));
    REQUIRE(row2.getValue("Length_M") == Catch::Approx(216.026).epsilon(epsilon));
    REQUIRE(row0.getValue("Height_M") == -1);
    REQUIRE(row1.getValue("Height_M") == -1);
    REQUIRE(row2.getValue("Height_M") == -1);
    REQUIRE(row0.getValue("Width_M") == -1);
    REQUIRE(row1.getValue("Width_M") == -1);
    REQUIRE(row2.getValue("Width_M") == -1);
}

TEST_CASE("Complete proper MapInfo file", "") {
    const float epsilon = 0.001f;

    // A typical MIF

    std::string mifdata =
        "Version 300\n"
        "Charset \"WindowsLatin1\"\n"
        "Delimiter \",\"\n"
        "Index 1,2\n"
        "CoordSys Earth Projection 8, 79, \"m\", -2, 49, 0.9996012717, 400000, -100000";

    SECTION("With Bounds") {
        mifdata += "Bounds (-7845061.1011, -15524202.1641) (8645061.1011, 4470074.53373)\n";
    }

    SECTION("Without Bounds") { mifdata += "\n"; }

    mifdata += "Columns 2\n"
               "  ID Integer\n"
               "  Length_m Float\n"
               "Data\n"
               "\n"
               "Line 534014.29 182533.33 535008.52 182764.11\n"
               "    Pen (1,2,0)\n"
               "Line 533798.68 183094.69 534365.48 183159.01\n"
               "    Pen (1,2,0)\n"
               "Point 534014.29 182533.33\n"
               "    Symbol (34,0,12)";

    // A Typical MID

    std::string middata = "1,1017.81\n"
                          "2,568.795\n"
                          "3,216.026";

    ShapeMap shapeMap("MapInfoTest");
    MapInfoData mapinfodata;

    std::stringstream mifstream(mifdata);
    std::stringstream midstream(middata);
    REQUIRE(mapinfodata.import(mifstream, midstream, shapeMap) == MINFO_OK);

    std::map<int, SalaShape> shapes = shapeMap.getAllShapes();
    auto shapeRef0 = genlib::getMapAtIndex(shapes, 0);
    auto shapeRef1 = genlib::getMapAtIndex(shapes, 1);
    auto shapeRef2 = genlib::getMapAtIndex(shapes, 2);

    auto &shape0 = shapeRef0->second;
    auto &shape1 = shapeRef1->second;
    auto &shape2 = shapeRef2->second;
    REQUIRE(shapes.size() == 3);
    REQUIRE(shape0.isLine());
    REQUIRE(shape0.getLine().ax() == Catch::Approx(534014.29).epsilon(epsilon));
    REQUIRE(shape0.getLine().ay() == Catch::Approx(182533.33).epsilon(epsilon));
    REQUIRE(shape0.getLine().bx() == Catch::Approx(535008.52).epsilon(epsilon));
    REQUIRE(shape0.getLine().by() == Catch::Approx(182764.11).epsilon(epsilon));
    REQUIRE(shape1.isLine());
    REQUIRE(shape1.getLine().ax() == Catch::Approx(533798.68).epsilon(epsilon));
    REQUIRE(shape1.getLine().ay() == Catch::Approx(183094.69).epsilon(epsilon));
    REQUIRE(shape1.getLine().bx() == Catch::Approx(534365.48).epsilon(epsilon));
    REQUIRE(shape1.getLine().by() == Catch::Approx(183159.01).epsilon(epsilon));
    REQUIRE(shape2.isPoint());
    REQUIRE(shape2.getPoint().x == Catch::Approx(534014.29).epsilon(epsilon));
    REQUIRE(shape2.getPoint().y == Catch::Approx(182533.33).epsilon(epsilon));

    AttributeTable &att = shapeMap.getAttributeTable();
    REQUIRE(att.getNumColumns() == 2);
    REQUIRE(att.getColumnName(0) == "Id");
    REQUIRE(att.getColumnName(1) == "Length_M");

    REQUIRE(att.getNumRows() == 3);
    auto &row0 = att.getRow(AttributeKey(shapeRef0->first));
    auto &row1 = att.getRow(AttributeKey(shapeRef1->first));
    auto &row2 = att.getRow(AttributeKey(shapeRef2->first));
    REQUIRE(row0.getValue("Id") == 1);
    REQUIRE(row1.getValue("Id") == 2);
    REQUIRE(row2.getValue("Id") == 3);
    REQUIRE(row0.getValue("Length_M") == Catch::Approx(1017.81).epsilon(epsilon));
    REQUIRE(row1.getValue("Length_M") == Catch::Approx(568.795).epsilon(epsilon));
    REQUIRE(row2.getValue("Length_M") == Catch::Approx(216.026).epsilon(epsilon));
}
