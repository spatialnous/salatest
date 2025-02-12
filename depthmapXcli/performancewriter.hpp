// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "performancesink.h"

#include <string>
#include <vector>

class PerformanceWriter : public IPerformanceSink {
  private:
    std::vector<std::string> m_data;
    std::string m_filename;

  public:
    PerformanceWriter(const std::string &filename);
    void addData(const std::string &message, double timeInSeconds) override;
    void write() const;
};
