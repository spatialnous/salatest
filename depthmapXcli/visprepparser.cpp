// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "visprepparser.h"
#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

#include "salalib/entityparsing.h"
#include "salalib/gridproperties.h"

#include <cstring>
#include <sstream>

namespace {
    void fillGraph(MetaGraphDX &graph, const Point2f &point) {
        auto r = graph.getRegion();
        if (!r.contains(point)) {
            throw depthmapX::RuntimeException("Point outside of target region");
        }
        graph.makePoints(point, 0, nullptr);
    }
} // namespace

using namespace depthmapX;

void VisPrepParser::parse(size_t argc, char **argv) {

    // All options:
    // Create grid (m_grid > 0)
    // Fill grid (pointFile or points not empty)
    // Make graph
    // Unmake graph

    // All possible combinations:
    // Create Grid + Fill Grid
    // Create Grid + Fill Grid + Make graph
    // Fill Grid
    // Fill Grid + Make graph
    // Make Graph
    // Unmake Graph

    // Bad combinations:
    // Create Grid + Make graph = Creates empty graph
    // Unmake graph + Anything else = Pointless action

    std::vector<std::string> points;
    std::string pointFile;
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-pg", argv[i]) == 0) {
            if (m_grid >= 0) {
                throw CommandLineException("-pg can only be used once");
            }
            ENFORCE_ARGUMENT("-pg", i)
            m_grid = std::atof(argv[i]);
            if (m_grid <= 0) {
                throw CommandLineException(std::string("-pg must be a number >0, got ") + argv[i]);
            }
        } else if (std::strcmp("-pp", argv[i]) == 0) {
            if (!pointFile.empty()) {
                throw CommandLineException("-pp cannot be used together with -pf");
            }
            ENFORCE_ARGUMENT("-pp", i)
            if (!has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid fill point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw CommandLineException(message.str().c_str());
            }
            points.push_back(argv[i]);
        } else if (std::strcmp("-pf", argv[i]) == 0) {
            if (!points.empty()) {
                throw CommandLineException("-pf cannot be used together with -pp");
            }
            ENFORCE_ARGUMENT("-pf", i)
            pointFile = argv[i];
        } else if (std::strcmp("-pr", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-pr", i);
            m_maxVisibility = std::atof(argv[i]);
            if (m_maxVisibility == 0.0) {
                std::stringstream message;
                message << "Restricted visibility of '" << argv[i]
                        << "' makes no sense, use a positive number or -1 for unrestricted";
                throw CommandLineException(message.str());
            }
        } else if (std::strcmp("-pb", argv[i]) == 0) {
            m_boundaryGraph = true;
        } else if (std::strcmp("-pm", argv[i]) == 0) {
            if (getUnmakeGraph()) {
                throw CommandLineException("-pm cannot be used together with -pu");
            }
            m_makeGraph = true;
        } else if (std::strcmp("-pu", argv[i]) == 0) {
            if (getMakeGraph()) {
                throw CommandLineException("-pu cannot be used together with -pm");
            }
            m_unmakeGraph = true;
        } else if (std::strcmp("-pl", argv[i]) == 0) {
            m_removeLinksWhenUnmaking = true;
        }
    }

    if (!getMakeGraph() && !getUnmakeGraph() && m_grid <= 0 && pointFile.empty() &&
        points.empty()) {
        throw CommandLineException("Nothing to do");
    }

    if (m_grid > 0 && getMakeGraph() && pointFile.empty() && points.empty()) {
        throw CommandLineException("Creating a graph for an unfilled grid is not possible. "
                                   "Either -pp or -pf must be given");
    }

    if (!pointFile.empty()) {
        std::ifstream pointsStream(pointFile);
        if (!pointsStream) {
            std::stringstream message;
            message << "Failed to load file " << pointFile << ", error ";

            // See https://en.cppreference.com/w/c/string/byte/strerror
#if defined(_MSC_VER)
            char buffer[100];
            if (strerror_s(buffer, errno)) {
                throw depthmapX::RuntimeException("Could not translate errno to a string");
            }
            message << buffer;
#else
            message << strerror(errno);
#endif
            message << std::flush;
            throw depthmapX::RuntimeException(message.str().c_str());
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, '\t');
        m_fillPoints.insert(std::end(m_fillPoints), std::begin(parsed), std::end(parsed));
    } else if (!points.empty()) {
        std::stringstream pointsStream;
        pointsStream << "x,y";
        std::vector<std::string>::iterator iter = points.begin(), end = points.end();
        for (; iter != end; ++iter) {
            pointsStream << "\n" << *iter;
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
        m_fillPoints.insert(std::end(m_fillPoints), std::begin(parsed), std::end(parsed));
    }

    if (getUnmakeGraph() && (m_grid > 0 || !m_fillPoints.empty())) {
        throw CommandLineException("-pu can not be used with any other option apart from -pl");
    }

    if (m_removeLinksWhenUnmaking && !m_unmakeGraph) {
        throw CommandLineException("-pl can only be used together with -pu");
    }
}

void VisPrepParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    std::cout << "Initial checks... " << std::flush;
    auto state = metaGraph.getState();
    if (~state & MetaGraphDX::DX_LINEDATA) {
        throw depthmapX::RuntimeException("Graph must have line data before preparing VGA");
    }
    if (m_grid > 0) {
        // Create a new pointmap and set tha grid
        Region4f r = metaGraph.getRegion();

        GridProperties gp(std::max(r.width(), r.height()));
        if (m_grid > gp.getMax() || m_grid < gp.getMin()) {
            std::stringstream message;
            message << "Chosen grid spacing " << m_grid
                    << " is outside of the expected interval of " << gp.getMin()
                    << " <= spacing <= " << gp.getMax() << std::flush;
            throw depthmapX::RuntimeException(message.str());
        }

        std::cout << "ok\nSetting up grid... " << std::flush;
        metaGraph.addNewPointMap();
        DO_TIMED("Setting grid", metaGraph.setGrid(m_grid, Point2f(0.0, 0.0)))
    } else if (metaGraph.getPointMaps().empty()) {
        std::stringstream message;
        message << "No map exists to use. Please create a new one by providing a grid size"
                << std::flush;
        throw depthmapX::RuntimeException(message.str());
    }

    if (m_unmakeGraph) {
        if (!metaGraph.hasDisplayedPointMap()) {
            metaGraph.setDisplayedPointMapRef(metaGraph.getPointMaps().size() - 1);
        }
        if (!dm_runmethods::safeGetDisplayedPointMap(metaGraph).getInternalMap().isProcessed()) {
            std::stringstream message;
            message << "Current map has not had its graph made so there's nothing to unmake"
                    << std::flush;
            throw depthmapX::RuntimeException(message.str());
        }
        DO_TIMED("Unmaking graph", metaGraph.unmakeGraph(m_removeLinksWhenUnmaking))
    } else {
        if (m_fillPoints.size() > 0) {
            std::cout << "ok\nFilling grid... " << std::flush;
            if (!metaGraph.hasDisplayedPointMap()) {
                metaGraph.setDisplayedPointMapRef(metaGraph.getPointMaps().size() - 1);
            }
            DO_TIMED("Filling grid", for_each(m_fillPoints.begin(), m_fillPoints.end(),
                                              [&metaGraph](const Point2f &point) -> void {
                                                  fillGraph(metaGraph, point);
                                              }))
        }
        if (m_makeGraph) {
            std::cout << "ok\nMaking graph... " << std::flush;
            if (!metaGraph.hasDisplayedPointMap()) {
                metaGraph.setDisplayedPointMapRef(metaGraph.getPointMaps().size() - 1);
            }
            DO_TIMED("Making graph", metaGraph.makeGraph(dm_runmethods::getCommunicator(clp).get(),
                                                         m_boundaryGraph ? 1 : 0, m_maxVisibility))

            if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = dm_runmethods::safeGetDisplayedPointMap(metaGraph);
                auto displayedAttribute = map.getDisplayedAttribute();

                auto sortedDisplayedAttribute =
                    static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                        static_cast<size_t>(displayedAttribute)));
                map.setDisplayedAttribute(sortedDisplayedAttribute);
            }
        }
    }

    std::cout << " ok\nWriting out result..." << std::flush;
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
