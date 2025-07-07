// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <chrono>

class SimpleTimer {
  public:
    SimpleTimer() : m_startTime(std::chrono::high_resolution_clock::now()) {}

    double getTimeInSeconds() const {
        auto t2 = std::chrono::high_resolution_clock::now();
        return static_cast<double>(
                   std::chrono::duration_cast<std::chrono::milliseconds>(t2 - m_startTime)
                       .count()) /
               1000.0;
    }

    void reset() { m_startTime = std::chrono::high_resolution_clock::now(); }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};
