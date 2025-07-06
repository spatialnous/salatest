// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>

// Interface for performance writers
class IPerformanceSink {
  public:
    virtual void addData(const std::string &message, double timeInSeconds) = 0;
    virtual ~IPerformanceSink() {}
};
