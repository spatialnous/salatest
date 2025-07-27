// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "exportparser.hpp"

#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include <cstring>
#include <sstream>

ExportParser::ExportParser() : m_exportMapIdx(std::nullopt), m_exportMode(ExportMode::NONE) {}

void ExportParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc;) {

        if (std::strcmp("-ei", argv[i]) == 0) {
            if (m_exportMapIdx.has_value()) {
                throw dmcli::CommandLineException("-ei can only be used once");
            }
            ENFORCE_ARGUMENT("-ei", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(std::string("-ei must be an integer >0, got ") +
                                                  argv[i]);
            }
            m_exportMapIdx = static_cast<size_t>(std::atoi(argv[i]));
        } else if (std::strcmp("-em", argv[i]) == 0) {
            if (m_exportMode != ExportParser::NONE) {
                throw dmcli::CommandLineException(
                    "-em can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-em", i)
            // still accepts the old pointmap- prefix, for a few versions
            // until the regression test binary CLIs are updated
            if (std::strcmp(argv[i], "pointmap-data-csv") == 0 ||
                std::strcmp(argv[i], "latticemap-data-csv") == 0) {
                m_exportMode = ExportMode::LATTICEMAP_DATA_CSV;
            } else if (std::strcmp(argv[i], "pointmap-connections-csv") == 0 ||
                       std::strcmp(argv[i], "latticemap-connections-csv") == 0) {
                m_exportMode = ExportMode::LATTICEMAP_CONNECTIONS_CSV;
            } else if (std::strcmp(argv[i], "pointmap-links-csv") == 0 ||
                       std::strcmp(argv[i], "latticemap-links-csv") == 0) {
                m_exportMode = ExportMode::LATTICEMAP_LINKS_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-map-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_MAP_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-map-mif") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_MAP_MIF;
            } else if (std::strcmp(argv[i], "shapegraph-connections-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_CONNECTIONS_CSV;
            } else if (std::strcmp(argv[i], "shapegraph-links-unlinks-csv") == 0) {
                m_exportMode = ExportMode::SHAPEGRAPH_LINKS_UNLINKS_CSV;
            } else {
                throw dmcli::CommandLineException(std::string("Invalid EXPORT mode: ") + argv[i]);
            }
        }
        ++i;
    }
}

LatticeMapDM &ExportParser::getSelectedOrDisplayedLatticeMap(MetaGraphDM &mgraph) const {
    if (mgraph.getLatticeMaps().size() == 0) {
        throw dmcli::CommandLineException("No lattice maps available.");
    }
    if (m_exportMapIdx.has_value()) {
        if (mgraph.getLatticeMaps().size() <= m_exportMapIdx) {
            throw dmcli::CommandLineException(
                "Invalid map index selected: " + std::to_string(m_exportMapIdx.value()) +
                ", only " + std::to_string(mgraph.getLatticeMaps().size()) + " available.");
        }
        return mgraph.getLatticeMaps()[m_exportMapIdx.value()];
    }
    return dm_runmethods::safeGetDisplayedLatticeMap(mgraph);
}

ShapeGraphDM &ExportParser::getSelectedOrDisplayedShapeGraph(MetaGraphDM &mgraph) const {
    if (mgraph.getShapeGraphs().size() == 0) {
        throw dmcli::CommandLineException("No shapegraphs available.");
    }
    if (m_exportMapIdx.has_value()) {
        if (mgraph.getShapeGraphs().size() <= m_exportMapIdx) {
            throw dmcli::CommandLineException(
                "Invalid map index selected: " + std::to_string(m_exportMapIdx.value()) +
                ", only " + std::to_string(mgraph.getShapeGraphs().size()) + " available.");
        }
        return mgraph.getShapeGraphs()[m_exportMapIdx.value()];
    }
    return dm_runmethods::safeGetDisplayedShapeGraph(mgraph);
}

void ExportParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {

    auto mgraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    switch (getExportMode()) {
    case ExportParser::LATTICEMAP_DATA_CSV: {
        auto &currentMap = getSelectedOrDisplayedLatticeMap(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing lattice map data", currentMap.getInternalMap().outputSummary(stream, ','))
        stream.close();
        break;
    }
    case ExportParser::LATTICEMAP_CONNECTIONS_CSV: {
        auto &currentMap = getSelectedOrDisplayedLatticeMap(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing lattice map connections",
                 currentMap.getInternalMap().outputConnectionsAsCSV(stream, ","))
        stream.close();
        break;
    }
    case ExportParser::LATTICEMAP_LINKS_CSV: {
        auto &currentMap = getSelectedOrDisplayedLatticeMap(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing lattice map connections",
                 currentMap.getInternalMap().outputLinksAsCSV(stream, ","))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_MAP_CSV: {
        auto &currentMap = getSelectedOrDisplayedShapeGraph(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing lattice map connections", currentMap.getInternalMap().output(stream, ','))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_MAP_MIF: {
        auto &currentMap = getSelectedOrDisplayedShapeGraph(mgraph);
        std::string fileName = clp.getOuputFile().c_str();
        std::string mifFile = fileName + ".mif";
        std::string midFile = fileName + ".mid";
        if (0 == fileName.compare(fileName.length() - 4, 4, ".mif")) {
            // we are given the .mif
            mifFile = fileName;
            midFile = fileName.substr(0, fileName.length() - 4) + ".mid";

        } else if (0 == fileName.compare(fileName.length() - 4, 4, ".mid")) {
            // we are given the .mid
            mifFile = fileName.substr(0, fileName.length() - 4) + ".mif";
            midFile = std::move(fileName);
        }
        std::ofstream mifStream(mifFile);
        std::ofstream midStream(midFile);
        DO_TIMED("Writing lattice map connections",
                 currentMap.getInternalMap().outputMifMap(mifStream, midStream))
        mifStream.close();
        midStream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_CONNECTIONS_CSV: {
        auto &currentMap = getSelectedOrDisplayedShapeGraph(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing shapegraph connections",
                 currentMap.getInternalMap().isAxialMap()
                     ? currentMap.getInternalMap().writeAxialConnectionsAsPairsCSV(stream)
                     : currentMap.getInternalMap().writeSegmentConnectionsAsPairsCSV(stream))
        stream.close();
        break;
    }
    case ExportParser::SHAPEGRAPH_LINKS_UNLINKS_CSV: {
        auto &currentMap = getSelectedOrDisplayedShapeGraph(mgraph);
        std::ofstream stream(clp.getOuputFile().c_str());
        DO_TIMED("Writing shapegraph links and unlinks",
                 currentMap.getInternalMap().writeLinksUnlinksAsPairsCSV(stream))
        stream.close();
        break;
    }
    default: {
        throw dmcli::SetupCheckException("Error, unsupported export mode");
    }
    }
}
