// SPDX-FileCopyrightText: 2018 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "segmentparser.hpp"
#include "depthmapXcli/dxinterface/options.hpp"
#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include "salalib/analysistype.hpp"

#include <cstring>

using namespace depthmapX;

SegmentParser::SegmentParser()
    : m_analysisType(InAnalysisType::NONE), m_radiusType(InRadiusType::NONE),
      m_includeChoice(false), m_tulipBins(0) {}

std::string SegmentParser::getModeName() const { return "SEGMENT"; }

std::string SegmentParser::getHelp() const {
    return "Mode options for Segment Analysis:\n"
           "  -st  <type of analysis> one of:\n"
           "       tulip (Angular Tulip - Faster)\n"
           "       angular (Angular Full - Slower)\n"
           "       topological\n"
           "       metric\n"
           "  -sr  <radius/list of radii>\n"
           "  -srt <radius type> (only for Tulip) one of:\n"
           "       steps\n"
           "       metric\n"
           "       angular\n"
           "  -sic to include choice (only for Tulip)\n"
           "  -stb <tulip bins> (4 to 1024, 1024 approximates full angular)\n"
           "  -swa <map attribute name> perform weighted analysis using this attribute (only for "
           "Tulip)\n";
}

void SegmentParser::parse(size_t argc, char **argv) {
    for (size_t i = 1; i < argc; ++i) {
        if (std::strcmp("-st", argv[i]) == 0) {
            if (m_analysisType != InAnalysisType::NONE) {
                throw CommandLineException(
                    "-st can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-st", i)
            if (std::strcmp(argv[i], "tulip") == 0) {
                m_analysisType = InAnalysisType::ANGULAR_TULIP;
            } else if (std::strcmp(argv[i], "angular") == 0) {
                m_analysisType = InAnalysisType::ANGULAR_FULL;
            } else if (std::strcmp(argv[i], "topological") == 0) {
                m_analysisType = InAnalysisType::TOPOLOGICAL;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_analysisType = InAnalysisType::METRIC;
            } else {
                throw CommandLineException(std::string("Invalid SEGMENT mode: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-srt") == 0) {
            if (m_radiusType != InRadiusType::NONE) {
                throw CommandLineException(
                    "-srt can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-srt", i)
            if (std::strcmp(argv[i], "steps") == 0) {
                m_radiusType = InRadiusType::SEGMENT_STEPS;
            } else if (std::strcmp(argv[i], "angular") == 0) {
                m_radiusType = InRadiusType::ANGULAR;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_radiusType = InRadiusType::METRIC;
            } else {
                throw CommandLineException(std::string("Invalid SEGMENT radius type: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-sic") == 0) {
            m_includeChoice = true;
        } else if (std::strcmp(argv[i], "-sr") == 0) {
            ENFORCE_ARGUMENT("-sr", i)
            m_radii = depthmapX::parseRadiusList(argv[i]);
        } else if (std::strcmp(argv[i], "-stb") == 0) {
            ENFORCE_ARGUMENT("-stb", i)

            if (!has_only_digits(argv[i])) {
                throw CommandLineException(
                    std::string("-stb must be a number between 4 and 1024, got ") + argv[i]);
            }
            m_tulipBins = std::atoi(argv[i]);
            if (m_tulipBins < 4 || m_tulipBins > 1024) {
                throw CommandLineException(
                    std::string("-stb must be a number between 4 and 1024, got ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-swa") == 0) {
            ENFORCE_ARGUMENT("-swa", i)
            m_attribute = argv[i];
        }
    }

    if (getAnalysisType() == InAnalysisType::NONE) {
        throw CommandLineException("No analysis type given");
    }

    if (getRadii().empty()) {
        throw CommandLineException("At least one radius must be provided");
    }

    if (getAnalysisType() == InAnalysisType::ANGULAR_TULIP &&
        getRadiusType() == InRadiusType::NONE) {
        throw CommandLineException("Radius type is required for tulip analysis");
    }

    if (getAnalysisType() == InAnalysisType::ANGULAR_TULIP && getTulipBins() == 0) {
        throw CommandLineException("Tulip bins are required for tulip analysis");
    }

    if (getAnalysisType() != InAnalysisType::ANGULAR_TULIP &&
        (getTulipBins() != 0 || getRadiusType() != InRadiusType::NONE || m_includeChoice)) {
        throw CommandLineException("-stb, -srt and -sic can only be used with tulip analysis");
    }
}

void SegmentParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    std::cout << "Running segment analysis... " << std::flush;
    Options options;
    const std::vector<double> &radii = getRadii();
    options.radiusSet.insert(radii.begin(), radii.end());
    options.choice = includeChoice();
    options.tulipBins = getTulipBins();
    options.weightedMeasureCol = -1;

    if (!getAttribute().empty()) {
        const auto &map = dm_runmethods::safeGetDisplayedShapeGraph(metaGraph);
        const AttributeTable &table = map.getAttributeTable();
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

    switch (getRadiusType()) {
    case InRadiusType::SEGMENT_STEPS: {
        options.radiusType = RadiusType::TOPOLOGICAL;
        break;
    }
    case InRadiusType::METRIC: {
        options.radiusType = RadiusType::METRIC;
        break;
    }
    case InRadiusType::ANGULAR: {
        options.radiusType = RadiusType::ANGULAR;
        break;
    }
    case InRadiusType::NONE:
        break;
    }
    switch (getAnalysisType()) {
    case InAnalysisType::ANGULAR_TULIP: {
        DO_TIMED("Segment tulip analysis",
                 metaGraph.analyseSegmentsTulip(
                     dm_runmethods::getCommunicator(clp).get(), options.radiusSet, options.selOnly,
                     options.tulipBins, options.weightedMeasureCol, options.radiusType,
                     options.choice, options.weightedMeasureCol2, options.routeweightCol, false,
                     (mimicVersion.has_value() && mimicVersion == "depthmapX 0.8.0")))
        break;
    }
    case InAnalysisType::ANGULAR_FULL: {
        DO_TIMED("Segment angular analysis",
                 metaGraph.analyseSegmentsAngular(dm_runmethods::getCommunicator(clp).get(),
                                                  options.radiusSet))
        break;
    }
    case InAnalysisType::TOPOLOGICAL: {
        options.outputType = AnalysisType::ISOVIST;
        DO_TIMED("Segment topological",
                 metaGraph.analyseTopoMetMultipleRadii(dm_runmethods::getCommunicator(clp).get(),
                                                       options.radiusSet, options.outputType,
                                                       options.radius, options.selOnly))
        break;
    }
    case InAnalysisType::METRIC: {
        options.outputType = AnalysisType::VISUAL;
        DO_TIMED("Segment metric", metaGraph.analyseTopoMetMultipleRadii(
                                       dm_runmethods::getCommunicator(clp).get(), options.radiusSet,
                                       options.outputType, options.radius, options.selOnly))
        break;
    }
    case InAnalysisType::NONE:
        throw depthmapX::RuntimeException("No segment analysis type given");
    }
    std::cout << "ok\n" << std::flush;

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
