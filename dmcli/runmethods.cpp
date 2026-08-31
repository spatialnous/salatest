// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "runmethods.hpp"

#include "exceptions.hpp"
#include "printcommunicator.hpp"
#include "simpletimer.hpp"

#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace dm_runmethods {
    MetaGraphDM loadGraph(const std::string &filename, IPerformanceSink &perfWriter) {
        std::cout << "Loading graph " << filename << std::flush;
        MetaGraphDM mgraph("Test mgraph");
        DO_TIMED("Load graph file", mgraph.readFromFile(filename);)

        if (mgraph.getReadStatus() != MetaGraphReadWrite::ReadWriteStatus::OK) {
            std::stringstream message;
            message << "Failed to load graph from file " << filename << ", error "
                    << MetaGraphReadWrite::getReadMessage(mgraph.getReadStatus()) << std::flush;
            throw genlib::RuntimeException(message.str().c_str());
        }
        std::cout << " ok\n" << std::flush;
        return mgraph;
    }

    std::unique_ptr<Communicator> getCommunicator(const CommandLineParser &clp) {
        if (clp.printProgress()) {
            return std::unique_ptr<Communicator>(new PrintCommunicator());
        }
        return nullptr;
    }

    void writeGraph(const CommandLineParser &clp, MetaGraphDM &metaGraph,
                    const std::string &filename, bool currentlayer) {
        metaGraph.write(filename, METAGRAPH_VERSION, currentlayer, clp.ignoreDisplayData());
    }

    LatticeMapDM &safeGetDisplayedLatticeMap(MetaGraphDM &mgraph) {
        if (mgraph.hasDisplayedLatticeMap()) {
            return mgraph.getDisplayedLatticeMap();
        } else if (!mgraph.getLatticeMaps().empty()) {
            return mgraph.getLatticeMaps().back();
        } else {
            throw dmcli::CommandLineException("No available lattice maps to process");
        }
    }

    ShapeGraphDM &safeGetDisplayedShapeGraph(MetaGraphDM &mgraph) {
        if (mgraph.hasDisplayedShapeGraph()) {
            return mgraph.getDisplayedShapeGraph();
        } else if (!mgraph.getShapeGraphs().empty()) {
            return mgraph.getShapeGraphs().back();
        } else {
            throw dmcli::CommandLineException("No available shapegraphs to process");
        }
    }

    void enforceDisplayedDataMapSet(MetaGraphDM &mgraph) {
        if (mgraph.hasDisplayedDataMap()) {
            return;
        } else if (!mgraph.getDataMaps().empty()) {
            mgraph.setDisplayedDataMapRef(mgraph.getDataMaps().size() - 1);
        } else {
            throw dmcli::CommandLineException("No available datamaps to process");
        }
    }

    void enforceDisplayedLatticeMapSet(MetaGraphDM &mgraph) {
        if (mgraph.hasDisplayedLatticeMap()) {
            return;
        } else if (!mgraph.getLatticeMaps().empty()) {
            mgraph.setDisplayedLatticeMapRef(mgraph.getLatticeMaps().size() - 1);
        } else {
            throw dmcli::CommandLineException("No available lattice maps to process");
        }
    }

    void enforceDisplayedShapeGraphSet(MetaGraphDM &mgraph) {
        if (mgraph.hasDisplayedShapeGraph()) {
            return;
        } else if (!mgraph.getShapeGraphs().empty()) {
            mgraph.setDisplayedShapeGraphRef(mgraph.getShapeGraphs().size() - 1);
        } else {
            throw dmcli::CommandLineException("No available shapegraphs to process");
        }
    }
} // namespace dm_runmethods
