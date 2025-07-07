// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dmcliTest/selfcleaningfile.hpp"

#include "salalib/genlib/readwritehelpers.hpp"

#include "catch_amalgamated.hpp"

#include <fstream>

TEST_CASE("vector reading and writing") {
    using namespace dXreadwrite;
    std::vector<int> intVec{1, 5, 34, -2, 5};
    SelfCleaningFile intFile("integers.bin");
    {
        std::ofstream outfile(intFile.Filename());
        writeVector(outfile, intVec);
    }

    {
        std::ifstream infile(intFile.Filename());
        auto copy = readVector<int>(infile);
        REQUIRE(copy == intVec);
    }

    std::vector<int> intCopy;
    {
        std::ifstream infile(intFile.Filename());
        readIntoVector(infile, intCopy);
    }
    REQUIRE(intCopy == intVec);
}

TEST_CASE("map reading and writing") {
    using namespace dXreadwrite;
    std::map<int, float> intFloatMap;
    intFloatMap.insert(std::make_pair(1, 0.1f));
    intFloatMap.insert(std::make_pair(5, 5000.0f));
    intFloatMap.insert(std::make_pair(34, -3.4f));
    intFloatMap.insert(std::make_pair(-2, 0.2f));
    intFloatMap.insert(std::make_pair(6, 0.6f));
    SelfCleaningFile intFloatFile("intFloatMap.bin");
    {
        std::ofstream outfile(intFloatFile.Filename());
        writeMap(outfile, intFloatMap);
    }

    {
        std::ifstream infile(intFloatFile.Filename());
        auto copy = readMap<int, float>(infile);
        REQUIRE(copy == intFloatMap);
    }

    std::map<int, float> intCopy;
    {
        std::ifstream infile(intFloatFile.Filename());
        readIntoMap(infile, intCopy);
    }
    REQUIRE(intCopy == intFloatMap);
}
