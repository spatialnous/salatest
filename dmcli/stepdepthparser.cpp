// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stepdepthparser.hpp"
#include "dminterface/options.hpp"
#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include "salalib/entityparsing.hpp"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

void StepDepthParser::parse(size_t argc, char **argv) {

    std::vector<std::string> points;
    std::string pointFile;
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-sdp", argv[i]) == 0) {
            if (!pointFile.empty()) {
                throw dmcli::CommandLineException("-sdp cannot be used together with -sdf");
            }
            ENFORCE_ARGUMENT("-sdp", i)
            if (!dmcli::has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid step depth point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw dmcli::CommandLineException(message.str().c_str());
            }
            points.push_back(argv[i]);
        } else if (std::strcmp("-sdf", argv[i]) == 0) {
            if (!points.empty()) {
                throw dmcli::CommandLineException("-sdf cannot be used together with -sdp");
            }
            ENFORCE_ARGUMENT("-sdf", i)
            pointFile = argv[i];
        } else if (std::strcmp("-sdt", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-sdt", i)
            if (std::strcmp(argv[i], "angular") == 0) {
                m_stepType = StepType::ANGULAR;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_stepType = StepType::METRIC;
            } else if (std::strcmp(argv[i], "visual") == 0) {
                m_stepType = StepType::VISUAL;
            } else {
                throw dmcli::CommandLineException(std::string("Invalid step type: ") + argv[i]);
            }
        }
    }

    if (pointFile.empty() && points.empty()) {
        throw dmcli::CommandLineException("Either -sdp or -sdf must be given");
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
                throw genlib::RuntimeException("Could not translate errno to a string");
            }
            message << buffer;
#else
            message << strerror(errno);
#endif

            message << std::flush;
            throw genlib::RuntimeException(message.str().c_str());
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, '\t');
        m_stepDepthPoints.insert(std::end(m_stepDepthPoints), std::begin(parsed), std::end(parsed));
    } else if (!points.empty()) {
        std::stringstream pointsStream;
        pointsStream << "x,y";
        std::vector<std::string>::iterator iter = points.begin(), end = points.end();
        for (; iter != end; ++iter) {
            pointsStream << "\n" << *iter;
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
        m_stepDepthPoints.insert(std::end(m_stepDepthPoints), std::begin(parsed), std::end(parsed));
    }

    if (m_stepType == StepType::NONE) {
        throw dmcli::CommandLineException("Step depth type (-sdt) must be provided");
    }
}

void StepDepthParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::cout << "ok\nSelecting cells... " << std::flush;

    for (auto &point : m_stepDepthPoints) {
        auto graphRegion = metaGraph.getRegion();
        if (!graphRegion.contains(point)) {
            throw genlib::RuntimeException("Point outside of target region");
        }
        Region4f r(point, point);
        metaGraph.setCurSel(r, true);
    }

    std::cout << "ok\nCalculating step-depth... " << std::flush;

    Options options;
    options.global = 0;

    switch (m_stepType) {
    case StepDepthParser::StepType::ANGULAR:
        options.pointDepthSelection = 3;
        break;
    case StepDepthParser::StepType::METRIC:
        options.pointDepthSelection = 2;
        break;
    case StepDepthParser::StepType::VISUAL:
        options.pointDepthSelection = 1;
        break;
    default: {
        throw dmcli::SetupCheckException("Error, unsupported step type");
    }
    }

    DO_TIMED("Calculating step-depth",
             metaGraph.analyseGraph(dm_runmethods::getCommunicator(clp).get(),
                                    options.pointDepthSelection, options.outputType, options.local,
                                    options.gatesOnly, options.global, options.radius, false);)

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
        /* legacy mode where the columns are sorted before stored */

        auto &map = dm_runmethods::safeGetDisplayedLatticeMap(metaGraph);
        auto displayedAttribute = map.getDisplayedAttribute();

        auto sortedDisplayedAttribute = static_cast<int>(
            map.getAttributeTable().getColumnSortedIndex(static_cast<size_t>(displayedAttribute)));
        map.setDisplayedAttribute(sortedDisplayedAttribute);

        // sala no longer stores points as "selected", but previous
        // versions do. Fake-select the origin point
        int selState = 0x0010;
        auto &selSet = map.getSelSet();
        for (auto &sel : selSet) {
            auto &point = map.getPoint(sel);
            point.set(point.getState() | selState);
        }
    }

    std::cout << " ok\nWriting out result..." << std::flush;
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
