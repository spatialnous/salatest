// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.h"

class SegmentParser : public IModeParser {
  public:
    SegmentParser();

    // IModeParser interface
  public:
    std::string getModeName() const override;
    std::string getHelp() const override;
    void parse(size_t argc, char **argv) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    enum class InAnalysisType { NONE, ANGULAR_TULIP, ANGULAR_FULL, TOPOLOGICAL, METRIC };

    enum class InRadiusType { NONE, SEGMENT_STEPS, ANGULAR, METRIC };

    InAnalysisType getAnalysisType() const { return m_analysisType; }

    InRadiusType getRadiusType() const { return m_radiusType; }

    bool includeChoice() const { return m_includeChoice; }

    int getTulipBins() const { return m_tulipBins; }

    const std::vector<double> getRadii() const { return m_radii; }

    const std::string getAttribute() const { return m_attribute; }

  private:
    InAnalysisType m_analysisType;
    InRadiusType m_radiusType;
    bool m_includeChoice;
    int m_tulipBins;
    std::vector<double> m_radii;
    std::string m_attribute;
};
