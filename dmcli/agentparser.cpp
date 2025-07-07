// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "agentparser.hpp"

#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include "salalib/agents/agentanalysis.hpp"
#include "salalib/entityparsing.hpp"
#include "salalib/exportutils.hpp"

#include <cstring>
#include <iostream>
#include <sstream>

AgentParser::AgentParser() : m_agentMode(AgentMode::NONE) {}

void AgentParser::parse(size_t argc, char *argv[]) {
    std::vector<std::string> points;
    std::string pointFile;

    for (size_t i = 1; i < argc;) {

        if (std::strcmp("-am", argv[i]) == 0) {
            if (m_agentMode != AgentMode::NONE) {
                throw dmcli::CommandLineException(
                    "-am can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-am", i)
            if (std::strcmp(argv[i], "standard") == 0) {
                m_agentMode = AgentMode::STANDARD;
            } else if (std::strcmp(argv[i], "los-length") == 0) {
                m_agentMode = AgentMode::LOS_LENGTH;
            } else if (std::strcmp(argv[i], "occ-length") == 0) {
                m_agentMode = AgentMode::OCC_LENGTH;
            } else if (std::strcmp(argv[i], "occ-any") == 0) {
                m_agentMode = AgentMode::OCC_ANY;
            } else if (std::strcmp(argv[i], "occ-group-45") == 0) {
                m_agentMode = AgentMode::OCC_GROUP_45;
            } else if (std::strcmp(argv[i], "occ-group-60") == 0) {
                m_agentMode = AgentMode::OCC_GROUP_60;
            } else if (std::strcmp(argv[i], "occ-furthest") == 0) {
                m_agentMode = AgentMode::OCC_FURTHEST;
            } else if (std::strcmp(argv[i], "bin-far-dist") == 0) {
                m_agentMode = AgentMode::BIN_FAR_DIST;
            } else if (std::strcmp(argv[i], "bin-angle") == 0) {
                m_agentMode = AgentMode::BIN_ANGLE;
            } else if (std::strcmp(argv[i], "bin-far-dist-angle") == 0) {
                m_agentMode = AgentMode::BIN_FAR_DIST_ANGLE;
            } else if (std::strcmp(argv[i], "bin-memory") == 0) {
                m_agentMode = AgentMode::BIN_MEMORY;
            } else {
                throw dmcli::CommandLineException(std::string("Invalid AGENTS mode: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-ats") == 0) {
            if (m_totalSystemTimestemps > 0) {
                throw dmcli::CommandLineException("-ats can only be used once");
            }
            ENFORCE_ARGUMENT("-ats", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(std::string("-ats must be a number >0, got ") +
                                                  argv[i]);
            }
            m_totalSystemTimestemps = static_cast<size_t>(std::atoi(argv[i]));
            if (m_totalSystemTimestemps <= 0) {
                throw dmcli::CommandLineException(std::string("-ats must be a number >0, got ") +
                                                  argv[i]);
            }
        } else if (std::strcmp(argv[i], "-arr") == 0) {
            if (m_releaseRate > 0) {
                throw dmcli::CommandLineException("-arr can only be used once");
            }
            ENFORCE_ARGUMENT("-arr", i)
            if (!dmcli::has_only_digits_dots_commas(argv[i])) {
                throw dmcli::CommandLineException(std::string("-arr must be a number >0, got ") +
                                                  argv[i]);
            }
            m_releaseRate = std::atof(argv[i]);
            if (m_releaseRate <= 0) {
                throw dmcli::CommandLineException(std::string("-arr must be a number >0, got ") +
                                                  argv[i]);
            }
        } else if (std::strcmp(argv[i], "-atrails") == 0) {
            if (m_recordTrailsForAgents >= 0) {
                throw dmcli::CommandLineException("-atrails can only be used once");
            }
            ENFORCE_ARGUMENT("-atrails", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(
                    std::string(
                        "-atrails must be a number >=1 or 0 for all (max possible = 50), got ") +
                    argv[i]);
            }
            m_recordTrailsForAgents = std::atoi(argv[i]);
            if (m_recordTrailsForAgents < 0) {
                throw dmcli::CommandLineException(
                    std::string(
                        "-atrails must be a number >=1 or 0 for all (max possible = 50), got ") +
                    argv[i]);
            }
        } else if (std::strcmp(argv[i], "-afov") == 0) {
            if (m_agentFOV > 0) {
                throw dmcli::CommandLineException("-afov can only be used once");
            }
            ENFORCE_ARGUMENT("-afov", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(
                    std::string("-afov must be a number between 1 and 32, got ") + argv[i]);
            }
            m_agentFOV = std::atoi(argv[i]);
            if (m_agentFOV <= 0 || m_agentFOV > 32) {
                throw dmcli::CommandLineException(
                    std::string("-afov must be a number between 1 and 32, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-asteps") == 0) {
            if (m_agentStepsBeforeTurnDecision > 0) {
                throw dmcli::CommandLineException("-asteps can only be used once");
            }
            ENFORCE_ARGUMENT("-asteps", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(std::string("-asteps must be a number >0, got ") +
                                                  argv[i]);
            }
            m_agentStepsBeforeTurnDecision = std::atoi(argv[i]);
            if (m_agentStepsBeforeTurnDecision <= 0) {
                throw dmcli::CommandLineException(std::string("-asteps must be a number >0, got ") +
                                                  argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alife") == 0) {
            if (m_agentLifeTimesteps > 0) {
                throw dmcli::CommandLineException("-alife can only be used once");
            }
            ENFORCE_ARGUMENT("-alife", i)
            if (!dmcli::has_only_digits(argv[i])) {
                throw dmcli::CommandLineException(std::string("-alife must be a number >0, got ") +
                                                  argv[i]);
            }
            m_agentLifeTimesteps = std::atoi(argv[i]);
            if (m_agentLifeTimesteps <= 0) {
                throw dmcli::CommandLineException(std::string("-alife must be a number >0, got ") +
                                                  argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alocseed") == 0) {
            if (!pointFile.empty()) {
                throw dmcli::CommandLineException(
                    "-alocseed cannot be used together with -alocfile");
            }
            if (!points.empty()) {
                throw dmcli::CommandLineException("-alocseed cannot be used together with -aloc");
            }
            ENFORCE_ARGUMENT("-alocseed", i)
            if (!dmcli::has_only_digits(argv[i])) {
                std::stringstream message;
                message << "Invalid starting location seed provided (" << argv[i]
                        << "). Should only contain digits" << std::flush;
                throw dmcli::CommandLineException(message.str().c_str());
            }
            m_randomReleaseLocationSeed = std::atoi(argv[i]);
            if (m_randomReleaseLocationSeed < 0 || m_randomReleaseLocationSeed > 10) {
                throw dmcli::CommandLineException(
                    std::string("-alocseed must be a number between 0 and 10, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-alocfile") == 0) {
            if (!points.empty()) {
                throw dmcli::CommandLineException("-alocfile cannot be used together with -aloc");
            }
            if (m_randomReleaseLocationSeed > -1) {
                throw dmcli::CommandLineException(
                    "-alocfile cannot be used together with -alocseed");
            }
            ENFORCE_ARGUMENT("-alocfile", i)
            pointFile = argv[i];
        } else if (std::strcmp(argv[i], "-aloc") == 0) {
            if (!pointFile.empty()) {
                throw dmcli::CommandLineException("-aloc cannot be used together with -alocfile");
            }
            if (m_randomReleaseLocationSeed > -1) {
                throw dmcli::CommandLineException("-aloc cannot be used together with -alocseed");
            }
            ENFORCE_ARGUMENT("-aloc", i)
            if (!dmcli::has_only_digits_dots_commas(argv[i])) {
                std::stringstream message;
                message << "Invalid starting point provided (" << argv[i]
                        << "). Should only contain digits dots and commas" << std::flush;
                throw dmcli::CommandLineException(message.str().c_str());
            }
            points.push_back(argv[i]);
        } else if (std::strcmp(argv[i], "-ot") == 0) {
            ENFORCE_ARGUMENT("-ot", i)
            if (std::strcmp(argv[i], "graph") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::GRAPH) !=
                    m_outputTypes.end()) {
                    throw dmcli::CommandLineException(
                        "Same output type argument (graph) provided twice");
                }
                m_outputTypes.push_back(OutputType::GRAPH);
            } else if (std::strcmp(argv[i], "gatecounts") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::GATECOUNTS) !=
                    m_outputTypes.end()) {
                    throw dmcli::CommandLineException(
                        "Same output type argument (gatecounts) provided twice");
                }
                m_outputTypes.push_back(OutputType::GATECOUNTS);
            } else if (std::strcmp(argv[i], "trails") == 0) {
                if (std::find(m_outputTypes.begin(), m_outputTypes.end(), OutputType::TRAILS) !=
                    m_outputTypes.end()) {
                    throw dmcli::CommandLineException(
                        "Same output type argument (trails) provided twice");
                }
                m_outputTypes.push_back(OutputType::TRAILS);
            }
        }
        ++i;
    }

    if (m_agentMode == AgentMode::NONE) {
        m_agentMode = AgentMode::STANDARD;
    }

    if (m_totalSystemTimestemps == 0) {
        throw dmcli::CommandLineException(
            "Total number of timesteps (-ats <timesteps>) is required");
    }

    if (m_releaseRate == 0) {
        throw dmcli::CommandLineException("Release rate (-arr <rate>) is required");
    }

    if (m_agentFOV == 0) {
        throw dmcli::CommandLineException("Agent field-of-view (-afov <bins>) is required");
    }

    if (m_agentStepsBeforeTurnDecision == 0) {
        throw dmcli::CommandLineException(
            "Agent number of steps before turn decision (-asteps <steps>) is required");
    }

    if (m_agentLifeTimesteps == 0) {
        throw dmcli::CommandLineException(
            "Agent life in timesteps (-alife <timesteps>) is required");
    }

    if (pointFile.empty() && points.empty() && m_randomReleaseLocationSeed == -1) {
        throw dmcli::CommandLineException("Either -aloc, -alocfile or -alocseed must be given");
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
        m_releasePoints.insert(std::end(m_releasePoints), std::begin(parsed), std::end(parsed));
    } else if (!points.empty()) {
        std::stringstream pointsStream;
        pointsStream << "x,y";
        std::vector<std::string>::iterator iter = points.begin(), end = points.end();
        for (; iter != end; ++iter) {
            pointsStream << "\n" << *iter;
        }
        std::vector<Point2f> parsed = EntityParsing::parsePoints(pointsStream, ',');
        m_releasePoints.insert(std::end(m_releasePoints), std::begin(parsed), std::end(parsed));
    }
}

void AgentParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {

    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    auto &currentMap = dm_runmethods::safeGetDisplayedPointMap(metaGraph);

    int agentViewAlgorithm = AgentProgram::SEL_STANDARD;

    switch (getAgentMode()) {
    case AgentParser::NONE:
    case AgentParser::STANDARD:
        agentViewAlgorithm = AgentProgram::SEL_STANDARD;
        break;
    case AgentParser::LOS_LENGTH:
        agentViewAlgorithm = AgentProgram::SEL_LOS;
        break;
    case AgentParser::OCC_LENGTH:
        agentViewAlgorithm = AgentProgram::SEL_LOS_OCC;
        break;
    case AgentParser::OCC_ANY:
        agentViewAlgorithm = AgentProgram::SEL_OCC_ALL;
        break;
    case AgentParser::OCC_GROUP_45:
        agentViewAlgorithm = AgentProgram::SEL_OCC_BIN45;
        break;
    case AgentParser::OCC_GROUP_60:
        agentViewAlgorithm = AgentProgram::SEL_OCC_BIN60;
        break;
    case AgentParser::OCC_FURTHEST:
        agentViewAlgorithm = AgentProgram::SEL_OCC_STANDARD;
        break;
    case AgentParser::BIN_FAR_DIST:
        agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_DIST;
        break;
    case AgentParser::BIN_ANGLE:
        agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_ANG;
        break;
    case AgentParser::BIN_FAR_DIST_ANGLE:
        agentViewAlgorithm = AgentProgram::SEL_OCC_WEIGHT_DIST_ANG;
        break;
    case AgentParser::BIN_MEMORY:
        agentViewAlgorithm = AgentProgram::SEL_OCC_MEMORY;
        break;
    }

    std::optional<AgentAnalysis::TrailRecordOptions> recordTrails =
        recordTrailsForAgents() >= 0
            ? std::make_optional(AgentAnalysis::TrailRecordOptions{
                  recordTrailsForAgents() == 0
                      ? std::nullopt
                      : std::make_optional(static_cast<size_t>(recordTrailsForAgents())),
                  std::ref(metaGraph.getDataMaps()
                               .emplace_back("Agent Trails", ShapeMap::DATAMAP)
                               .getInternalMap())})
            : std::nullopt;

    if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
        // older versions of depthmapX limited the maximum number of trails to 50
        if (recordTrails.has_value()) {
            if ((recordTrails->limit.has_value() && recordTrails->limit > 50) ||
                !recordTrails->limit.has_value()) {
                recordTrails->limit = 50;
            }
        }
    }

    // the ui and code suggest that the results can be put on a separate
    // 'data map', but the functionality does not seem to actually be
    // there thus it is skipped for now
    std::optional<std::reference_wrapper<ShapeMap>> gateLayer = std::nullopt;

    auto analysis = std::unique_ptr<IAnalysis>(new AgentAnalysis(
        currentMap.getInternalMap(), totalSystemTimestemps(), releaseRate(),
        static_cast<size_t>(agentLifeTimesteps()), static_cast<unsigned short>(agentFOV()),
        static_cast<size_t>(agentStepsBeforeTurnDecision()), agentViewAlgorithm,
        randomReleaseLocationSeed(), getReleasePoints(), gateLayer, recordTrails));

    std::cout << "ok\nRunning agent analysis... " << std::flush;
    DO_TIMED("Running agent analysis",
             metaGraph.runAgentEngine(dm_runmethods::getCommunicator(clp).get(), analysis);)

    std::cout << " ok\nWriting out result..." << std::flush;
    std::vector<AgentParser::OutputType> resultTypes = outputTypes();
    if (resultTypes.size() == 0) {
        // if no choice was made for an output type assume the user just
        // wants a graph file

        if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
            /* legacy mode where the columns are sorted before stored */
            auto &map = dm_runmethods::safeGetDisplayedPointMap(metaGraph);
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }

        DO_TIMED("Writing graph",
                 dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    } else if (resultTypes.size() == 1) {
        // if only one type of output is given, assume that the user has
        // correctly entered a name with the correct extension and export
        // exactly with that name and extension

        switch (resultTypes[0]) {
        case AgentParser::OutputType::GRAPH: {

            if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                auto &map = dm_runmethods::safeGetDisplayedPointMap(metaGraph);
                auto displayedAttribute = map.getDisplayedAttribute();

                auto sortedDisplayedAttribute =
                    static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                        static_cast<size_t>(displayedAttribute)));
                map.setDisplayedAttribute(sortedDisplayedAttribute);
            }

            DO_TIMED("Writing graph",
                     dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
            break;
        }
        case AgentParser::OutputType::GATECOUNTS: {
            std::ofstream gatecountStream(clp.getOuputFile().c_str());
            DO_TIMED("Writing gatecounts",
                     currentMap.getInternalMap().outputSummary(gatecountStream, ','))
            break;
        }
        case AgentParser::OutputType::TRAILS: {
            std::ofstream trailStream(clp.getOuputFile().c_str());
            DO_TIMED("Writing trails",
                     exportUtils::writeMapShapesAsCat(recordTrails->map, trailStream))

            break;
        }
        }
    } else {
        // if more than one output type is given assume the user has given
        // a filename without an extension and thus the new file must have
        // an extension. Also to avoid name clashes in cases where the user
        // asked for outputs that would yield the same extension also add
        // a related suffix

        if (std::find(resultTypes.begin(), resultTypes.end(), AgentParser::OutputType::GRAPH) !=
            resultTypes.end()) {

            if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
                /* legacy mode where the columns are sorted before stored */
                for (auto &map : metaGraph.getShapeGraphs()) {
                    auto displayedAttribute = map.getDisplayedAttribute();

                    auto sortedDisplayedAttribute =
                        static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                            static_cast<size_t>(displayedAttribute)));
                    map.setDisplayedAttribute(sortedDisplayedAttribute);
                }
                auto &map = dm_runmethods::safeGetDisplayedPointMap(metaGraph);
                auto displayedAttribute = map.getDisplayedAttribute();

                auto sortedDisplayedAttribute =
                    static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                        static_cast<size_t>(displayedAttribute)));
                map.setDisplayedAttribute(sortedDisplayedAttribute);
            }

            std::string outFile = clp.getOuputFile() + ".graph";
            DO_TIMED("Writing graph",
                     dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
        }
        if (std::find(resultTypes.begin(), resultTypes.end(),
                      AgentParser::OutputType::GATECOUNTS) != resultTypes.end()) {
            std::string outFile = clp.getOuputFile() + "_gatecounts.csv";
            std::ofstream gatecountStream(outFile.c_str());
            DO_TIMED("Writing gatecounts",
                     currentMap.getInternalMap().outputSummary(gatecountStream, ','))
        }
        if (std::find(resultTypes.begin(), resultTypes.end(), AgentParser::OutputType::TRAILS) !=
            resultTypes.end()) {
            std::string outFile = clp.getOuputFile() + "_trails.cat";
            std::ofstream trailStream(outFile.c_str());
            DO_TIMED("Writing trails",
                     exportUtils::writeMapShapesAsCat(recordTrails->map, trailStream))
        }
    }
}
