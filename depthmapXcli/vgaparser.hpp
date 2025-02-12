// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "imodeparser.h"

#include <string>

class VgaParser : public IModeParser {
  public:
    std::string getModeName() const override { return "VGA"; }

    std::string getHelp() const override {
        return "Mode options for VGA:\n"
               "-vm <vga mode> one of isovist, visiblity, metric, angular, thruvision\n"
               "-vg turn on global measures for visibility, requires radius between 1 and 99 or n\n"
               "-vl turn on local measures for visibility\n"
               "-vr set visibility radius\n";
    }

  public:
    VgaParser();
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    enum VgaMode { NONE, ISOVIST, VISBILITY, METRIC, ANGULAR, THRU_VISION };

    // vga options
    VgaMode getVgaMode() const { return m_vgaMode; }
    bool localMeasures() const { return m_localMeasures; }
    bool globalMeasures() const { return m_globalMeasures; }
    const std::string &getRadius() const { return m_radius; }

  private:
    // vga options
    VgaMode m_vgaMode;
    bool m_localMeasures;
    bool m_globalMeasures;
    std::string m_radius;
};
