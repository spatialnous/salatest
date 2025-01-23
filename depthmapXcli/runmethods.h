// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "dxinterface/metagraphdx.h"
#include "performancesink.h"

#include <string>
#include <vector>

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#define DO_TIMED(message, code)                                                                    \
    SimpleTimer CONCAT(t_, __LINE__);                                                              \
    code;                                                                                          \
    perfWriter.addData(message, CONCAT(t_, __LINE__).getTimeInSeconds());

class Line;
class Point2f;

namespace dm_runmethods {
    MetaGraphDX loadGraph(const std::string &filename, IPerformanceSink &perfWriter);
    std::unique_ptr<Communicator> getCommunicator(const CommandLineParser &clp);
    void writeGraph(const CommandLineParser &clp, MetaGraphDX &metaGraph,
                    const std::string &filename, bool currentlayer);
    PointMapDX &safeGetDisplayedPointMap(MetaGraphDX &mgraph);
    ShapeGraphDX &safeGetDisplayedShapeGraph(MetaGraphDX &mgraph);

    void enforceDisplayedDataMapSet(MetaGraphDX &mgraph);
    void enforceDisplayedPointMapSet(MetaGraphDX &mgraph);
    void enforceDisplayedShapeGraphSet(MetaGraphDX &mgraph);
} // namespace dm_runmethods
