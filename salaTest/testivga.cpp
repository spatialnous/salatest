// SPDX-FileCopyrightText: 2026 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "catch_amalgamated.hpp"

#include "salalib/pixelref.hpp"
#include "salalib/vgamodules/vgautils.hpp"

#include <optional>

TEST_CASE("RefIndex") {
    REQUIRE_THROWS_WITH(VGAUtils::RefIndex({5, 8, 14, 1256, 727}),
                        "RefIndex: refs must be strictly ascending");

    VGAUtils::RefIndex refIdx({5, 8, 14, 727, 1256});
    REQUIRE(refIdx.idx(14) == 2);
    REQUIRE(refIdx.idx(5) == 0);
    REQUIRE(refIdx.idx(727) == 3);
    REQUIRE(refIdx.idx(1256) == 4);
    REQUIRE(refIdx.idx(8) == 1);

    REQUIRE_THROWS_WITH(refIdx.idx(3131), "Ref 3131 not in refs");

    REQUIRE(refIdx.idx(14) == 2);
    REQUIRE(refIdx.idx(5) == 0);
    REQUIRE(refIdx.idx(727) == 3);
    REQUIRE(refIdx.idx(1256) == 4);
    REQUIRE(refIdx.idx(8) == 1);

    REQUIRE(refIdx.idxOptional(3131) == std::nullopt);
}
