// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "imodeparser.h"

#include "salalib/genlib/point2f.h"

#include <string>

class AgentParser : public IModeParser {
  public:
    std::string getModeName() const override { return "AGENTS"; }

    std::string getHelp() const override {
        return "Mode options for AGENTS:\n"
               "-am <agent mode> one of:\n"
               "    standard\n"
               "    los-length (Line of Sight length)\n"
               "    occ-length (Occluded length)\n"
               "    occ-any (Any occlusions)\n"
               "    occ-group-45 (Occlusion group bins - 45 degrees)\n"
               "    occ-group-60 (Occlusion group bins - 60 degrees)\n"
               "    occ-furthest (Furthest occlusion per bin)\n"
               "    bin-far-dist (Per bin far distance weighted)\n"
               "    bin-angle (Per bin angle weighted)\n"
               "    bin-far-dist-angle (Per bin far-distance and angle weighted)\n"
               "    bin-memory (Per bin memory)\n"
               "-ats <timesteps> set total system timesteps\n"
               "-arr <rate> set agent release rate (likelyhood of release per timestep)\n"
               "-atrails <no of trails> record trails for this amount of agents (set to 0 to "
               "record all"
               ", with max possible currently = 50)\n"
               "-afov <fov> set agent field-of-view (bins)\n"
               "-asteps <steps> set agent steps before turn decision\n"
               "-alife <timesteps> set agent total lifetime (in timesteps)\n"
               "-alocseed <seed> set agents to start at random locations with specific seed (0 to "
               "10)\n"
               "-alocfile <agent starting points file>\n"
               "-aloc <single agent starting point coordinates> provided in csv (x1,y1) "
               "for example \"0.1,0.2\". Provide multiple times for multiple links\n"
               "-ot <output type> available output types (may use more than one):"
               "    graph (graph file, default)"
               "    gatecounts (csv with cells of grid with gate counts)"
               "    trails (csv with lines showing path traversed by each agent)";
    }

  public:
    AgentParser();
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    enum AgentMode {
        NONE,
        STANDARD,
        LOS_LENGTH,
        OCC_LENGTH,
        OCC_ANY,
        OCC_GROUP_45,
        OCC_GROUP_60,
        OCC_FURTHEST,
        BIN_FAR_DIST,
        BIN_ANGLE,
        BIN_FAR_DIST_ANGLE,
        BIN_MEMORY
    };

    enum OutputType { GRAPH, GATECOUNTS, TRAILS };

    // agent options
    AgentMode getAgentMode() const { return m_agentMode; }

    size_t totalSystemTimestemps() const { return m_totalSystemTimestemps; }
    double releaseRate() const { return m_releaseRate; }
    int recordTrailsForAgents() const { return m_recordTrailsForAgents; }
    int randomReleaseLocationSeed() const { return m_randomReleaseLocationSeed; }

    int agentFOV() const { return m_agentFOV; }
    int agentStepsBeforeTurnDecision() const { return m_agentStepsBeforeTurnDecision; }
    int agentLifeTimesteps() const { return m_agentLifeTimesteps; }

    const std::vector<Point2f> &getReleasePoints() const { return m_releasePoints; }

    std::vector<OutputType> outputTypes() const { return m_outputTypes; }

  private:
    // agent options
    AgentMode m_agentMode;

    size_t m_totalSystemTimestemps = 0;
    double m_releaseRate = 0.0;
    int m_recordTrailsForAgents = -1;

    int m_agentFOV = 0;                     // Field of view (bins)
    int m_agentStepsBeforeTurnDecision = 0; // Steps before turn decision
    int m_agentLifeTimesteps = 0;           // Timesteps in system

    int m_randomReleaseLocationSeed = -1;
    std::vector<Point2f> m_releasePoints;

    std::vector<OutputType> m_outputTypes;
};
