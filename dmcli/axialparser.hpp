// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/genlib/point2f.hpp"

#include <cstddef>
#include <string>
#include <vector>

class AxialParser : public IModeParser {
  public:
    AxialParser();

    // IModeParser interface
  public:
    std::string getModeName() const override;
    std::string getHelp() const override;
    void parse(size_t argc, char **argv) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    // accessors
    bool runAllLines() const { return !m_allAxesRoots.empty(); }

    const std::vector<Point2f> &getAllAxesRoots() const { return m_allAxesRoots; }

    bool runFewestLines() const { return m_runFewestLines; }

    bool runUnlink() const {
        // not supported for now
        return false;
    }

    bool runAnalysis() const { return m_runAnalysis; }

    bool useChoice() const { return m_choice; }
    bool useLocal() const { return m_local; }
    bool calculateRRA() const { return m_rra; }

    const std::vector<double> &getRadii() const { return m_radii; }
    const std::string getAttribute() const { return m_attribute; }

  private:
    std::vector<Point2f> m_allAxesRoots;
    bool m_runFewestLines;
    bool m_runAnalysis;
    std::vector<double> m_radii;
    bool m_choice;
    bool m_local;
    bool m_rra;
    std::string m_attribute;
};
