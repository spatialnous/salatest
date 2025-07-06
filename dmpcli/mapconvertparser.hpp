// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/shapemap.hpp"

class MapConvertParser : public IModeParser {
  public:
    MapConvertParser()
        : m_outMapType(ShapeMap::EMPTYMAP), m_outMapName(""), m_removeInputMap(false),
          m_copyAttributes(false), m_removeStubLengthPRC(0) {}

    // IModeParser interface
  public:
    std::string getModeName() const override { return "MAPCONVERT"; }

    std::string getHelp() const override {
        return "Mode options for Map Conversion:\n"
               "  -co Output map type (to convert to)\n"
               "      Possible input/output map types:\n"
               "        - drawing\n"
               "        - axial\n"
               "        - segment\n"
               "        - data\n"
               "        - convex\n"
               "  -con Output map name\n"
               "  -cir Remove input map\n"
               "  -coc Copy attributes to output map (Only between DATA, AXIAL and SEGMENT)\n"
               "  -crsl <%> Percent of line length of axial stubs to remove (Only for AXIAL -> "
               "SEGMENT)\n\n";
    }
    void parse(size_t argc, char **argv) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    int outputMapType() const { return m_outMapType; }
    std::string outputMapName() const { return m_outMapName; }
    bool removeInputMap() const { return m_removeInputMap; }
    bool copyAttributes() const { return m_copyAttributes; }
    double removeStubLength() const { return m_removeStubLengthPRC; }

  private:
    std::string getMapTypeName(int mapType) const;
    int m_outMapType;
    std::string m_outMapName;
    bool m_removeInputMap;
    bool m_copyAttributes;
    double m_removeStubLengthPRC;
};
