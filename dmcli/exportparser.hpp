// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.hpp"
#include "imodeparser.hpp"

#include "dminterface/metagraphdm.hpp"

#include <cstddef>
#include <optional>
#include <string>

class ExportParser : public IModeParser {
  public:
    std::string getModeName() const override { return "EXPORT"; }

    std::string getHelp() const override {
        return "Mode options for EXPORT:\n"
               "-ei <export index> map index in type group\n"
               "-em <export mode> one of:\n"
               "    latticemap-data-csv\n"
               "    latticemap-connections-csv\n"
               "    latticemap-links-csv\n"
               "    shapegraph-map-csv\n"
               "    shapegraph-map-mif\n"
               "    shapegraph-connections-csv\n"
               "    shapegraph-links-unlinks-csv\n";
    }

  public:
    ExportParser();
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    std::optional<size_t> getExportMapIdx() const { return m_exportMapIdx; }

    enum ExportMode {
        NONE,
        LATTICEMAP_DATA_CSV,
        LATTICEMAP_CONNECTIONS_CSV,
        LATTICEMAP_LINKS_CSV,
        SHAPEGRAPH_MAP_CSV,
        SHAPEGRAPH_MAP_MIF,
        SHAPEGRAPH_CONNECTIONS_CSV,
        SHAPEGRAPH_LINKS_UNLINKS_CSV
    };
    ExportMode getExportMode() const { return m_exportMode; }

  private:
    std::optional<size_t> m_exportMapIdx;
    ExportMode m_exportMode;

    LatticeMapDM &getSelectedOrDisplayedLatticeMap(MetaGraphDM &mgraph) const;
    ShapeGraphDM &getSelectedOrDisplayedShapeGraph(MetaGraphDM &mgraph) const;
};
