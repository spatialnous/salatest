// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "selfcleaningfile.hpp"

#include "catch_amalgamated.hpp"

#include <fstream>

namespace {
    bool fileExists(const std::string &filename) {
        std::ifstream f(filename.c_str());
        return f.good();
    }

    bool writeToFile(const std::string &filename, const std::string &content) {
        std::ofstream f(filename.c_str());
        if (!f.good()) {
            return false;
        }
        f << content;
        f.flush();
        return true;
    }
} // namespace

TEST_CASE("TestSelfCleaningFile", "Check it is deleted, doesn't fail when not present") {
    {
        SelfCleaningFile scf("foo.txt");
        REQUIRE(scf.Filename() == "foo.txt");
        REQUIRE(fileExists("foo.txt") == false);
        REQUIRE(writeToFile(scf.Filename(), "bla bla bla"));
        REQUIRE(fileExists(scf.Filename()));
    }
    REQUIRE(fileExists("foo.txt") == false);

    {
        SelfCleaningFile scf("foo.txt");
        REQUIRE(scf.Filename() == "foo.txt");
        REQUIRE(fileExists("foo.txt") == false);
    }
    REQUIRE(fileExists("foo.txt") == false);
}
