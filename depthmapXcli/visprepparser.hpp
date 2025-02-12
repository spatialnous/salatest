// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/genlib/point2f.hpp"

#include <vector>

class VisPrepParser : public IModeParser {
  public:
    VisPrepParser()
        : m_grid(-1.0), m_maxVisibility(-1.0), m_boundaryGraph(false), m_makeGraph(false),
          m_unmakeGraph(false), m_removeLinksWhenUnmaking(false) {}

    std::string getModeName() const override { return "VISPREP"; }

    std::string getHelp() const override {
        return "Mode options for VISPREP (visual analysis preparation) are:\n"
               "  -pg <gridsize> floating point number defining the grid spacing. If this\n"
               "      is provided it will create a new map\n"
               "  -pp <fillpoint> point where to fill. Can be repeated\n"
               "  -pf <fillpoint file> a file with a point per line to fill\n"
               "  -pr <max visibility> restrict visibility (-1 is unrestricted, default)\n"
               "  -pb Make boundary graph\n"
               "  -pm Make graph\n"
               "  -pu Unmake graph\n"
               "  -pl Remove links when unmaking\n";
    }

    void parse(size_t argc, char **argv) override;

    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    double getGrid() const { return m_grid; }
    std::vector<Point2f> getFillPoints() const { return m_fillPoints; }
    bool getBoundaryGraph() const { return m_boundaryGraph; }
    double getMaxVisibility() const { return m_maxVisibility; }
    bool getMakeGraph() const { return m_makeGraph; }
    bool getUnmakeGraph() const { return m_unmakeGraph; }
    bool getRemoveLinksWhenUnmaking() const { return m_removeLinksWhenUnmaking; }

  private:
    double m_grid;
    std::vector<Point2f> m_fillPoints;
    double m_maxVisibility;
    bool m_boundaryGraph;
    bool m_makeGraph;
    bool m_unmakeGraph;
    bool m_removeLinksWhenUnmaking;
};
