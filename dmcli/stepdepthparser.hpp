// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/genlib/point2f.hpp"

#include <vector>

class StepDepthParser : public IModeParser {
  public:
    StepDepthParser() : m_stepType(StepType::NONE) {}

    std::string getModeName() const override { return "STEPDEPTH"; }

    std::string getHelp() const override {
        return "Mode options for pointmap STEPDEPTH are:\n"
               "  -sdp <step depth point> point where to calculate step depth from. Can be "
               "repeated\n"
               "  -sdf <step depth point file> a file with a point per line to calculate step "
               "depth from\n"
               "  -sdt <type> step type. One of metric, angular or visual\n";
    }

    enum class StepType { NONE, ANGULAR, METRIC, VISUAL };

    void parse(size_t argc, char **argv) override;

    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    std::vector<Point2f> getStepDepthPoints() const { return m_stepDepthPoints; }

    StepType getStepType() const { return m_stepType; }

  private:
    std::vector<Point2f> m_stepDepthPoints;

    StepType m_stepType;
};
