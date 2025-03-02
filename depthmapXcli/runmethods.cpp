// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "runmethods.hpp"

#include "exceptions.hpp"
#include "printcommunicator.hpp"
#include "simpletimer.hpp"

#include <memory>
#include <sstream>
#include <vector>

namespace dm_runmethods {
    MetaGraphDX loadGraph(const std::string &filename, IPerformanceSink &perfWriter) {
        std::cout << "Loading graph " << filename << std::flush;
        MetaGraphDX mgraph("Test mgraph");
        DO_TIMED("Load graph file", mgraph.readFromFile(filename);)

        if (mgraph.getReadStatus() != MetaGraphReadWrite::ReadWriteStatus::OK) {
            std::stringstream message;
            message << "Failed to load graph from file " << filename << ", error "
                    << MetaGraphReadWrite::getReadMessage(mgraph.getReadStatus()) << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
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

    void writeGraph(const CommandLineParser &clp, MetaGraphDX &metaGraph,
                    const std::string &filename, bool currentlayer) {
        metaGraph.write(filename, METAGRAPH_VERSION, currentlayer, clp.ignoreDisplayData());
    }

    PointMapDX &safeGetDisplayedPointMap(MetaGraphDX &mgraph) {
        if (mgraph.hasDisplayedPointMap()) {
            return mgraph.getDisplayedPointMap();
        } else if (!mgraph.getPointMaps().empty()) {
            return mgraph.getPointMaps().back();
        } else {
            throw depthmapX::CommandLineException("No available pointmaps to process");
        }
    }

    ShapeGraphDX &safeGetDisplayedShapeGraph(MetaGraphDX &mgraph) {
        if (mgraph.hasDisplayedShapeGraph()) {
            return mgraph.getDisplayedShapeGraph();
        } else if (!mgraph.getShapeGraphs().empty()) {
            return mgraph.getShapeGraphs().back();
        } else {
            throw depthmapX::CommandLineException("No available shapegraphs to process");
        }
    }

    void enforceDisplayedDataMapSet(MetaGraphDX &mgraph) {
        if (mgraph.hasDisplayedDataMap()) {
            return;
        } else if (!mgraph.getDataMaps().empty()) {
            mgraph.setDisplayedDataMapRef(mgraph.getDataMaps().size() - 1);
        } else {
            throw depthmapX::CommandLineException("No available datamaps to process");
        }
    }

    void enforceDisplayedPointMapSet(MetaGraphDX &mgraph) {
        if (mgraph.hasDisplayedPointMap()) {
            return;
        } else if (!mgraph.getPointMaps().empty()) {
            mgraph.setDisplayedPointMapRef(mgraph.getPointMaps().size() - 1);
        } else {
            throw depthmapX::CommandLineException("No available pointmaps to process");
        }
    }

    void enforceDisplayedShapeGraphSet(MetaGraphDX &mgraph) {
        if (mgraph.hasDisplayedShapeGraph()) {
            return;
        } else if (!mgraph.getShapeGraphs().empty()) {
            mgraph.setDisplayedShapeGraphRef(mgraph.getShapeGraphs().size() - 1);
        } else {
            throw depthmapX::CommandLineException("No available shapegraphs to process");
        }
    }
} // namespace dm_runmethods
