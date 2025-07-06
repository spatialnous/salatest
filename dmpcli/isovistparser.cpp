// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "isovistparser.hpp"

#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include "salalib/entityparsing.hpp"

#include <cstring>
#include <sstream>

IsovistParser::IsovistParser() {}

std::string IsovistParser::getModeName() const { return "ISOVIST"; }

std::string IsovistParser::getHelp() const {
    return "Arguments for isovist mode:\n"
           "  -ii <x,y[,angle,viewangle]> Define an isoivist at position x,y with\n"
           "    optional direction angle and view angle for partial isovists\n"
           "  -if <isovist file> load isovist definitions from a file (csv)\n"
           "    the relevant headers must be called x, y, angle and viewangle\n"
           "    the latter two are optional.\n"
           "  Those two arguments cannot be mixed\n"
           "  Angles for partial isovists are in degrees, counted anti-clockwise with 0°\n"
           "  pointing to the right.\n\n";
}

void IsovistParser::parse(size_t argc, char **argv) {
    std::string isovistFile;

    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-ii") == 0) {
            if (!isovistFile.empty()) {
                throw dmpcli::CommandLineException("-ii cannot be used together with -if");
            }
            ENFORCE_ARGUMENT("-ii", i)
            m_isovists.push_back(EntityParsing::parseIsovist(argv[i]));
        } else if (std::strcmp(argv[i], "-if") == 0) {
            if (!isovistFile.empty()) {
                throw dmpcli::CommandLineException("-if can only be used once");
            }
            if (!m_isovists.empty()) {
                throw dmpcli::CommandLineException("-if cannot be used together with -ii");
            }
            ENFORCE_ARGUMENT("-if", i)
            isovistFile = argv[i];
        } else if (std::strcmp(argv[i], "-ic") == 0) {
            m_closeIsovistPolys = true;
        }
    }

    if (!isovistFile.empty()) {
        std::ifstream file(isovistFile);
        if (!file.good()) {
            std::stringstream message;
            message << "Failed to find file " << isovistFile;
            throw dmpcli::CommandLineException(message.str());
        }
        m_isovists = EntityParsing::parseIsovists(file, ',');
    }
    if (m_isovists.empty()) {
        throw dmpcli::CommandLineException("No isovists defined. Use -ii or -if");
    }
}

void IsovistParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::cout << "Making " << m_isovists.size() << " isovists... " << std::flush;
    DO_TIMED("Make isovists",
             std::for_each(m_isovists.begin(), m_isovists.end(),
                           [&metaGraph, &clp, this](const IsovistDefinition &isovist) -> void {
                               metaGraph.makeIsovist(dm_runmethods::getCommunicator(clp).get(),
                                                     isovist.getLocation(), isovist.getLeftAngle(),
                                                     isovist.getRightAngle(), clp.simpleMode(),
                                                     m_closeIsovistPolys);
                           }))
    std::cout << " ok\nWriting out result..." << std::flush;

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
        /* legacy mode where the columns are sorted before stored */

        auto &map = metaGraph.getDataMaps().back();
        auto displayedAttribute = map.getDisplayedAttribute();

        auto sortedDisplayedAttribute = static_cast<int>(
            map.getAttributeTable().getColumnSortedIndex(static_cast<size_t>(displayedAttribute)));
        map.setDisplayedAttribute(sortedDisplayedAttribute);
    }

    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
