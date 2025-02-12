// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/genlib/point2f.hpp"

#include <vector>

class SegmentShortestPathParser : public IModeParser {
  public:
    SegmentShortestPathParser() : m_stepType(StepType::NONE) {}

    std::string getModeName() const override { return "SEGMENTSHORTESTPATH"; }

    std::string getHelp() const override {
        return "Mode options for pointmap SEGMENTSHORTESTPATH are:\n"
               "  -sspo <shortest path origin point> point where to calculate shortest path "
               "between.\n"
               "  -sspd <shortest path destination point> point where to calculate shortest path "
               "between.\n"
               "  -sspt <type> step type. One of metric, tulip or topological.\n";
    }

    enum class StepType { NONE, TULIP, METRIC, TOPOLOGICAL };

    void parse(size_t argc, char **argv) override;

    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    Point2f getShortestPathOrigin() const { return m_originPoint; }
    Point2f getShortestPathDestination() const { return m_destinationPoint; }

    StepType getStepType() const { return m_stepType; }

  private:
    Point2f m_originPoint;
    Point2f m_destinationPoint;

    StepType m_stepType;
};
