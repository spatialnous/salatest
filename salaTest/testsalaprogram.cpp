// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../salalib/mapconverter.hpp"
#include "catch_amalgamated.hpp"
#include "salalib/genlib/point2f.hpp"
#include "salalib/salaprogram.hpp"
#include "salalib/shapegraph.hpp"
#include "salalib/shapemapgroupdata.hpp"
#include <sstream>

// Most of these test cases are adapted from salalib/salascript-tests.txt
// with some added for completeness

TEST_CASE("Trivial scripts") {

    std::stringstream script;
    SalaObj expected;

    SECTION("comment") { script << "# comment\n"; }

    SECTION("single dimension lists") {
        script << "x = [1,2]\n"
               << "x[1] = 10\n"
               << "x[1]\n";
        expected = SalaObj(10);
    }

    SECTION("multiple dimension lists") {
        script << "x = [[1,2],[3,4]]\n"
               << "x[0][1] = 10\n"
               << "x[0][1]\n";
        expected = SalaObj(10);
    }

    SECTION("range direct access") {
        script << "range(5,10)[2]\n";
        expected = SalaObj(7);
    }

    SECTION("list return length") {
        script << "x = [1,2,3]\n"
               << "len(x)\n";
        expected = SalaObj(3);
    }

    SECTION("list return list") {
        script << "x = [1,2,3]\n"
               << "x\n";
        expected = SalaObj(SalaObj::Type::S_LIST, 3);
        expected.list_at(0) = SalaObj(1);
        expected.list_at(1) = SalaObj(2);
        expected.list_at(2) = SalaObj(3);
    }

    SECTION("2D list return length") {
        script << "x = [[1,2],[3,4]]\n"
               << "len(x)\n";
        expected = SalaObj(2);
    }

    SECTION("2D list return list") {
        script << "x = [[1,2],[3,4]]\n"
               << "x\n";
        expected = SalaObj(SalaObj::Type::S_LIST, 2);
        expected.list_at(0) = SalaObj(SalaObj::Type::S_LIST, 2);
        expected.list_at(0).list_at(0) = SalaObj(1);
        expected.list_at(0).list_at(1) = SalaObj(2);
        expected.list_at(1) = SalaObj(SalaObj::Type::S_LIST, 2);
        expected.list_at(1).list_at(0) = SalaObj(3);
        expected.list_at(1).list_at(1) = SalaObj(4);
    }

    SECTION("Pythonesque curios: lists by reference") {
        script << "x = [1,2,3,4]\n"
               << "y = x\n"
               << "y[3] = 40\n"
               << "x[3]\n";
        expected = SalaObj(40);
    }

    SalaGrf graph;
    SalaObj context = SalaObj(SalaObj::S_POINTMAPOBJ, graph);
    SalaProgram program(context);
    program.parse(script);
    SalaObj result = program.evaluate();
    REQUIRE(result == expected);
}

TEST_CASE("Trivial errors") {

    std::stringstream script;

    SECTION("simple for with error: i should be uninitialised") {
        script << "x = 0\n"
               << "for i in range(5,10):\n"
               << "    x = 1\n"
               << "x = x + i\n";
    }

    SalaGrf graph;
    SalaObj context = SalaObj(SalaObj::S_POINTMAPOBJ, graph);
    SalaProgram program(context);
    program.parse(script);
    REQUIRE_THROWS_WITH(program.evaluate(), "Unknown exception");
}

TEST_CASE("Variables from outer scope are accessible in inner scope") {
    std::stringstream script;
    SalaObj expected;
    SECTION("Access to global scope from within a for loop") {
        script << "x = 5\n"
               << "for i in range(0,1):\n"
               << "    x = 100\n"
               << "x";
        expected = SalaObj(100);
    }

    SalaGrf graph;
    SalaObj context = SalaObj(SalaObj::S_POINTMAPOBJ, graph);
    SalaProgram program(context);
    program.parse(script);
    SalaObj result = program.evaluate();
    REQUIRE(result.toInt() == expected.toInt());
}

TEST_CASE("Shapemap scripts") {

    const double epsilon = 0.001;

    Point2f line1Start(0, 0);
    Point2f line1End(3, 0);
    Point2f line2Start(1, 1);
    Point2f line2End(1, -1);
    Point2f line3Start(2, 1);
    Point2f line3End(2, -2);
    Point2f line4Start(2, 1);
    Point2f line4End(4, 1);
    Point2f line5Start(5, 3);
    Point2f line5End(3, 1);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");

    spacePixels.back().makeLineShape(Line4f(line1Start, line1End));
    spacePixels.back().makeLineShape(Line4f(line2Start, line2End));
    spacePixels.back().makeLineShape(Line4f(line3Start, line3End));
    spacePixels.back().makeLineShape(Line4f(line4Start, line4End));
    spacePixels.back().makeLineShape(Line4f(line5Start, line5End));

    auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);
    auto shapeGraph = MapConverter::convertDrawingToAxial(nullptr, "Test axial", drawingMapRefs);

    std::stringstream script;
    std::vector<double> expectedColVals;

    SECTION("pass ref to new column") {
        script << "value(\"Ref Number\")\n";
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(1.0);
        expectedColVals.push_back(2.0);
        expectedColVals.push_back(3.0);
        expectedColVals.push_back(4.0);
    }

    SECTION("if, function of a function on a range") {
        script << "x = len(range(1,value(\"Ref Number\")))\n"
               << "if x == 2:\n"
               << "   return 5\n"
               << "elif x < 1:\n"
               << "   return 10\n"
               << "x\n"
               << "# first two objects should be set to 10, next to 5, and then ref number after "
                  "that;\n";
        expectedColVals.push_back(10.0);
        expectedColVals.push_back(10.0);
        expectedColVals.push_back(1.0);
        expectedColVals.push_back(5.0);
        expectedColVals.push_back(3.0);
    }

    SECTION("simple if") {
        script << "if value(\"Ref Number\") < 2:\n"
               << "    0\n"
               << "elif value(\"Ref Number\") == 3:\n"
               << "    5\n"
               << "else\n"
               << "    10\n";
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(10.0);
        expectedColVals.push_back(5.0);
        expectedColVals.push_back(10.0);
    }

    SECTION("various member functions tests") {
        script << "this.value(\"Ref Number\")\n"
               << "    len(range(1,this.value(\"Ref Number\")))\n"
               << "elif value(\"Ref Number\") == 3:\n"
               << "    range(1,this.value(\"Ref Number\")).length()\n";
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(1.0);
        expectedColVals.push_back(2.0);
        expectedColVals.push_back(3.0);
    }

    auto newCol = shapeGraph->addAttribute("NewCol");
    SalaGrf graph;
    graph.map.shape = shapeGraph.get();
    SalaObj context = SalaObj(SalaObj::S_SHAPEMAPOBJ, graph);
    SalaProgram program(context);
    program.parse(script);
    program.runupdate(static_cast<int>(newCol));

    REQUIRE(shapeGraph->getAttributeTable().getNumRows() == expectedColVals.size());

    auto iter = expectedColVals.begin();
    auto &attributes = shapeGraph->getAttributeTable();
    for (auto rowIter = attributes.begin(); rowIter != attributes.end(); rowIter++) {
        REQUIRE(rowIter->getRow().getValue(newCol) == Catch::Approx(*iter).epsilon(epsilon));
        iter++;
    }
}

TEST_CASE("Shapemap scripts with unexpected results") {

    const double epsilon = 0.001;

    Point2f line1Start(0, 0);
    Point2f line1End(3, 0);
    Point2f line2Start(1, 1);
    Point2f line2End(1, -1);
    Point2f line3Start(2, 1);
    Point2f line3End(2, -2);
    Point2f line4Start(2, 1);
    Point2f line4End(4, 1);
    Point2f line5Start(5, 3);
    Point2f line5End(3, 1);

    std::vector<std::pair<ShapeMapGroupData, std::vector<ShapeMap>>> drawingFiles(1);

    auto &spacePixelFileData = drawingFiles.back().first;
    spacePixelFileData.name = "Test SpacePixelGroup";
    auto &spacePixels = drawingFiles.back().second;
    spacePixels.emplace_back("Test ShapeMap");

    spacePixels.back().makeLineShape(Line4f(line1Start, line1End));
    spacePixels.back().makeLineShape(Line4f(line2Start, line2End));
    spacePixels.back().makeLineShape(Line4f(line3Start, line3End));
    spacePixels.back().makeLineShape(Line4f(line4Start, line4End));
    spacePixels.back().makeLineShape(Line4f(line5Start, line5End));

    auto drawingMapRefs = ShapeMapGroupData::getAsRefMaps(drawingFiles);
    auto shapeGraph = MapConverter::convertDrawingToAxial(nullptr, "Test axial", drawingMapRefs);

    std::stringstream script;
    std::vector<double> expectedColVals;

    SECTION("for with else and 0 length ranges") {
        script << "int x = 0\n"
               << "for i in range(2,value(\"Ref Number\")):\n"
               << "    x = x + i\n"
               << "    x\n"
               << "else:\n"
               << "    0\n";
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(0.0);
        expectedColVals.push_back(2.0);
        expectedColVals.push_back(5.0);
    }

    SECTION("Total Depth Calculation") {
        script << "total_depth = 0\n"
               << "depth = 0\n"
               << "pop_list = [this]\n"
               << "push_list = []\n"
               << "setmark(true)\n"
               << "while len(pop_list):\n"
               << "    total_depth = total_depth + depth\n"
               << "    curs = pop_list.pop()\n"
               << "    for i in curs.connections():\n"
               << "        if i.mark() is none:\n"
               << "            i.setmark(true)\n"
               << "            push_list.append(i)\n"
               << "    if len(pop_list) == 0:\n"
               << "        depth = depth + 1\n"
               << "        pop_list = push_list\n"
               << "        push_list = []\n"
               << "total_depth\n";
        expectedColVals.push_back(7.0);
        expectedColVals.push_back(10.0);
        expectedColVals.push_back(6.0);
        expectedColVals.push_back(7.0);
        expectedColVals.push_back(10.0);
    }

    SECTION("Shortest Cycle") {
        script << "push_list = []\n"
               << "pop_list = []\n"
               << "live_paths = []\n"
               << "setmark([-1,0])\n"
               << "depth = 1\n"
               << "path_index = 0\n"
               << "for i in connections():\n"
               << "    pop_list.append([path_index,i])\n"
               << "    live_paths.append(1)\n"
               << "    i.setmark([path_index,depth])\n"
               << "    path_index = path_index + 1\n"
               << "if path_index < 2:\n"
               << "    return -1 # no cycle possible\n"
               << "live_path_count = path_index\n"
               << "while len(pop_list) and live_path_count > 1:\n"
               << "    curs = pop_list.pop()\n"
               << "    path_index = curs[0]\n"
               << "    this_node = curs[1]\n"
               << "    live_paths[path_index] = live_paths[path_index] - 1\n"
               << "    for i in this_node.connections():\n"
               << "        if i.mark() is none:\n"
               << "            i.setmark([path_index,depth+1])\n"
               << "            push_list.append([path_index,i])\n"
               << "            live_paths[path_index] = live_paths[path_index] + 1\n"
               << "        elif i.mark()[0] != path_index and i.mark()[0] != -1:\n"
               << "            # found a cycle!\n"
               << "            return i.mark()[1] + this_node.mark()[1] + 1\n"
               << "    if live_paths[path_index] == 0:\n"
               << "        live_path_count = live_path_count - 1\n"
               << "    if len(pop_list) == 0:\n"
               << "        depth = depth + 1\n"
               << "        pop_list = push_list\n"
               << "        push_list = []\n"
               << "-1 # no cycle found\n";
        expectedColVals.push_back(-1.0);
        expectedColVals.push_back(-1.0);
        expectedColVals.push_back(-1.0);
        expectedColVals.push_back(-1.0);
        expectedColVals.push_back(-1.0);
    }

    auto newCol = shapeGraph->addAttribute("NewCol");
    SalaGrf graph;
    graph.map.shape = shapeGraph.get();
    SalaObj context = SalaObj(SalaObj::S_SHAPEMAPOBJ, graph);
    SalaProgram program(context);
    program.parse(script);
    program.runupdate(static_cast<int>(newCol));

    REQUIRE(shapeGraph->getAttributeTable().getNumRows() == expectedColVals.size());

    auto iter = expectedColVals.begin();
    auto &attributes = shapeGraph->getAttributeTable();
    for (auto rowIter = attributes.begin(); rowIter != attributes.end(); rowIter++) {
        REQUIRE(rowIter->getRow().getValue(newCol) == Catch::Approx(*iter).epsilon(epsilon));
        iter++;
    }
}

TEST_CASE("Performance tests") {
    //# For a graph with 100000 segments for cpu timing:
    // x=value("Angular Connectivity")*value("Angular Step Depth")+value("Axial Line
    // Ref")+value("Connectivity")/value("Segment Length")^value("T1024 Choice R1000 metric")
    // y=value("T1024 Choice R3000 metric")*value("T1024 Choice R4000 metric")/value("T1024 Choice
    // R5000 metric")^value("T1024 Total Depth [Segment Length Wgt] R4000 metric") y/x
}
