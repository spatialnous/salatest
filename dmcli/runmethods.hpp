// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.hpp"
#include "dminterface/metagraphdm.hpp"
#include "performancesink.hpp"

#include <memory>
#include <string>

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#define DO_TIMED(message, code)                                                                    \
    SimpleTimer CONCAT(t_, __LINE__);                                                              \
    code;                                                                                          \
    perfWriter.addData(message, CONCAT(t_, __LINE__).getTimeInSeconds());

class Line4f;
class Point2f;

namespace dm_runmethods {
    MetaGraphDM loadGraph(const std::string &filename, IPerformanceSink &perfWriter);
    std::unique_ptr<Communicator> getCommunicator(const CommandLineParser &clp);
    void writeGraph(const CommandLineParser &clp, MetaGraphDM &metaGraph,
                    const std::string &filename, bool currentlayer);
    LatticeMapDM &safeGetDisplayedLatticeMap(MetaGraphDM &mgraph);
    ShapeGraphDM &safeGetDisplayedShapeGraph(MetaGraphDM &mgraph);

    void enforceDisplayedDataMapSet(MetaGraphDM &mgraph);
    void enforceDisplayedLatticeMapSet(MetaGraphDM &mgraph);
    void enforceDisplayedShapeGraphSet(MetaGraphDM &mgraph);
} // namespace dm_runmethods
