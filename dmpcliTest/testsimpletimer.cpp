// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dmpcli/simpletimer.hpp"

#include "catch_amalgamated.hpp"

#include <chrono>
#include <thread>

TEST_CASE("TestSimpleTimer", "") {
    SimpleTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE_THAT(timer.getTimeInSeconds(), Catch::Matchers::WithinAbs(0.5, 0.2));
}

TEST_CASE("TestSimpleTimerReset", "") {
    SimpleTimer timer1;
    SimpleTimer timer2;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE_THAT(timer1.getTimeInSeconds(), Catch::Matchers::WithinAbs(0.5, 0.2));
    REQUIRE_THAT(timer2.getTimeInSeconds(), Catch::Matchers::WithinAbs(0.5, 0.2));
    timer2.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#if __APPLE__
    // Apple's threads seem to be slightly slower, perhaps because of app-nap?
    REQUIRE_THAT(timer1.getTimeInSeconds(), Catch::Matchers::WithinAbs(1.0, 0.3));
#else
    REQUIRE_THAT(timer1.getTimeInSeconds(), Catch::Matchers::WithinAbs(1.0, 0.2));
#endif
    REQUIRE_THAT(timer2.getTimeInSeconds(), Catch::Matchers::WithinAbs(0.5, 0.2));
}
