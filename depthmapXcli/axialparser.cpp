// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "axialparser.h"

#include "exceptions.h"
#include "parsingutils.h"
#include "runmethods.h"
#include "simpletimer.h"

#include "salalib/entityparsing.h"

#include <cstring>

using namespace depthmapX;

AxialParser::AxialParser()
    : m_runFewestLines(false), m_runAnalysis(false), m_choice(false), m_local(false), m_rra(false) {

}

std::string AxialParser::getModeName() const { return "AXIAL"; }

std::string AxialParser::getHelp() const {
    return "Mode options for Axial Analysis:\n"
           "  -xl <x>,<y> Calculate all lines map from this seed point (can be used more than "
           "once)\n"
           "  -xf Calculate fewest lines map from all lines map\n"
           "  -xa <radius/list of radii> run axial anlysis with specified radii\n"
           " All modes expect to find the required input in the in graph\n"
           " Any combination of flags above can be specified, they will always be run in the order "
           "-aa -af -au -ax\n"
           " Further flags for axial analysis are:\n"
           "   -xac Include choice (betweenness)\n"
           "   -xal Include local measures\n"
           "   -xar Include RA, RRA and total depth\n"
           "   -xaw <map attribute name> perform weighted analysis using this attribute\n"
           "\n";
}

void AxialParser::parse(size_t argc, char **argv) {
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-xl") == 0) {
            ENFORCE_ARGUMENT("-xl", i)
            m_allAxesRoots.push_back(EntityParsing::parsePoint(argv[i]));
        } else if (std::strcmp(argv[i], "-xf") == 0) {
            m_runFewestLines = true;
        } else if (std::strcmp(argv[i], "-xa") == 0) {
            ENFORCE_ARGUMENT("-xa", i)
            if (m_runAnalysis) {
                throw CommandLineException("-xa can only be used once");
            }
            m_radii = depthmapX::parseRadiusList(argv[i]);
            m_runAnalysis = true;
        } else if (std::strcmp(argv[i], "-xal") == 0) {
            m_local = true;
        } else if (std::strcmp(argv[i], "-xac") == 0) {
            m_choice = true;
        } else if (std::strcmp(argv[i], "-xar") == 0) {
            m_rra = true;
        } else if (std::strcmp(argv[i], "-xaw") == 0) {
            ENFORCE_ARGUMENT("-xaw", i)
            m_attribute = argv[i];
        }
    }

    if (!runAllLines() && !runFewestLines() && !runUnlink() && !runAnalysis()) {
        throw CommandLineException("No axial analysis mode present");
    }
}

void AxialParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    auto state = metaGraph.getState();
    if (runAllLines()) {
        if (~state & MetaGraphDX::LINEDATA) {
            throw depthmapX::RuntimeException(
                "Line drawing must be loaded before axial map can be constructed");
        }
        std::cout << "Making all line map... " << std::flush;
        DO_TIMED("Making all axes map",
                 for_each(getAllAxesRoots().begin(), getAllAxesRoots().end(),
                          [&metaGraph, &clp](const Point2f &point) -> void {
                              metaGraph.makeAllLineMap(dm_runmethods::getCommunicator(clp).get(),
                                                       point);
                          }))
        std::cout << "ok" << std::endl;
    }

    if (runFewestLines()) {
        if (~state & MetaGraphDX::LINEDATA) {
            throw depthmapX::RuntimeException(
                "Line drawing must be loaded before fewest line map can be constructed");
        }
        if (!metaGraph.hasAllLineMap()) {
            throw depthmapX::RuntimeException("All line map must be constructed before fewest "
                                              "lines can be constructed. Use -aa to do this");
        }
        std::cout << "Constructing fewest line map... " << std::flush;
        DO_TIMED("Fewest line map",
                 metaGraph.makeFewestLineMap(dm_runmethods::getCommunicator(clp).get(), 1))
        std::cout << "ok" << std::endl;
    }

    if (runAnalysis()) {
        std::cout << "Running axial analysis... " << std::flush;
        Options options;
        const std::vector<double> &radii = getRadii();
        options.radiusSet.insert(radii.begin(), radii.end());
        options.choice = useChoice();
        options.local = useLocal();
        options.fulloutput = calculateRRA();
        options.weightedMeasureCol = -1;

        if (!getAttribute().empty()) {
            const auto &map = dm_runmethods::safeGetDisplayedShapeGraph(metaGraph);
            const auto &table = map.getAttributeTable();
            for (size_t i = 0; i < table.getNumColumns(); i++) {
                if (getAttribute() == table.getColumnName(i).c_str()) {
                    options.weightedMeasureCol = static_cast<int>(i);
                }
            }
            if (options.weightedMeasureCol == -1) {
                throw depthmapX::RuntimeException("Given attribute (" + getAttribute() +
                                                  ") does not exist in currently selected map");
            }
        }

        DO_TIMED(
            "Axial analysis",
            metaGraph.analyseAxial(dm_runmethods::getCommunicator(clp).get(), options,
                                   (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0")))
        std::cout << "ok\n" << std::flush;
    }

    if (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0") {
        /* legacy mode where the columns are sorted before stored */
        auto &map = dm_runmethods::safeGetDisplayedShapeGraph(metaGraph);
        auto displayedAttribute = map.getDisplayedAttribute();

        auto sortedDisplayedAttribute = static_cast<int>(
            map.getAttributeTable().getColumnSortedIndex(static_cast<size_t>(displayedAttribute)));
        map.setDisplayedAttribute(sortedDisplayedAttribute);
    }

    std::cout << "Writing out result..." << std::flush;
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
