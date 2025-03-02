// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vgaparser.hpp"
#include "depthmapXcli/dxinterface/options.hpp"
#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "radiusconverter.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include <cstring>

using namespace depthmapX;

VgaParser::VgaParser()
    : m_vgaMode(VgaMode::NONE), m_localMeasures(false), m_globalMeasures(false) {}

void VgaParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc;) {

        if (std::strcmp("-vm", argv[i]) == 0) {
            if (m_vgaMode != VgaMode::NONE) {
                throw CommandLineException(
                    "-vm can only be used once, modes are mutually exclusive");
            }
            ENFORCE_ARGUMENT("-vm", i)
            if (std::strcmp(argv[i], "isovist") == 0) {
                m_vgaMode = VgaMode::ISOVIST;
            } else if (std::strcmp(argv[i], "visibility") == 0) {
                m_vgaMode = VgaMode::VISBILITY;
            } else if (std::strcmp(argv[i], "metric") == 0) {
                m_vgaMode = VgaMode::METRIC;
            } else if (std::strcmp(argv[i], "angular") == 0) {
                m_vgaMode = VgaMode::ANGULAR;
            } else if (std::strcmp(argv[i], "thruvision") == 0) {
                m_vgaMode = VgaMode::THRU_VISION;
            } else {
                throw CommandLineException(std::string("Invalid VGA mode: ") + argv[i]);
            }
        } else if (std::strcmp(argv[i], "-vg") == 0) {
            m_globalMeasures = true;
        } else if (std::strcmp(argv[i], "-vl") == 0) {
            m_localMeasures = true;
        } else if (std::strcmp(argv[i], "-vr") == 0) {
            ENFORCE_ARGUMENT("-vr", i)
            m_radius = argv[i];
        }
        ++i;
    }

    if (m_vgaMode == VgaMode::NONE) {
        m_vgaMode = VgaMode::ISOVIST;
    }

    if (m_vgaMode == VgaMode::VISBILITY && m_globalMeasures) {
        if (m_radius.empty()) {
            throw CommandLineException(
                "Global measures in VGA/visibility analysis require a radius, use -vr <radius>");
        }
        if (m_radius != "n" && !has_only_digits(m_radius)) {
            throw CommandLineException(
                std::string("Radius must be a positive integer number or n, got ") + m_radius);
        }

    } else if (m_vgaMode == VgaMode::METRIC) {
        if (m_radius.empty()) {
            throw CommandLineException("Metric vga requires a radius, use -vr <radius>");
        }
    }
}

void VgaParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    RadiusConverter converter;
    auto metaGraph = dm_runmethods::loadGraph(clp.getFileName().c_str(), perfWriter);

    std::unique_ptr<Options> options(new Options());

    std::cout << "Getting options..." << std::flush;
    switch (getVgaMode()) {
    case VgaParser::VgaMode::VISBILITY:
        options->outputType = AnalysisType::VISUAL;
        options->local = localMeasures();
        options->global = globalMeasures();
        if (options->global) {
            options->radius = converter.ConvertForVisibility(getRadius());
        }
        break;
    case VgaParser::VgaMode::METRIC:
        options->outputType = AnalysisType::METRIC;
        options->radius = converter.ConvertForMetric(getRadius());
        break;
    case VgaParser::VgaMode::ANGULAR:
        options->outputType = AnalysisType::ANGULAR;
        break;
    case VgaParser::VgaMode::ISOVIST:
        options->outputType = AnalysisType::ISOVIST;
        break;
    case VgaParser::VgaMode::THRU_VISION:
        options->outputType = AnalysisType::THRU_VISION;
        break;
    default:
        throw depthmapX::SetupCheckException("Unsupported VGA mode");
    }
    std::cout << " ok\nAnalysing graph..." << std::flush;

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    if (!mimicVersion.has_value()) {
        // current version
        DO_TIMED("Run VGA",
                 metaGraph.analyseGraph(dm_runmethods::getCommunicator(clp).get(),
                                        options->pointDepthSelection, options->outputType,
                                        options->local, options->gatesOnly, options->global,
                                        options->radius, clp.simpleMode());)

    } else if (*mimicVersion == "depthmapX 0.8.0") {
        int currentDisplayedAttribute = -1;
        if (getVgaMode() == VgaParser::VgaMode::ISOVIST) {
            // in this version vga isovist analysis does not change the
            // displayed attribute, so we have to reset it back to what
            // it was before the analysis
            currentDisplayedAttribute =
                dm_runmethods::safeGetDisplayedPointMap(metaGraph).getDisplayedAttribute();
        }

        DO_TIMED("Run VGA",
                 metaGraph.analyseGraph(dm_runmethods::getCommunicator(clp).get(),
                                        options->pointDepthSelection, options->outputType,
                                        options->local, options->gatesOnly, options->global,
                                        options->radius, clp.simpleMode());)

        if (getVgaMode() == VgaParser::VgaMode::ISOVIST) {
            dm_runmethods::safeGetDisplayedPointMap(metaGraph).setDisplayedAttribute(
                currentDisplayedAttribute);
        }
        /* legacy mode where the columns are sorted before stored */
        for (auto &map : metaGraph.getPointMaps()) {
            auto displayedAttribute = map.getDisplayedAttribute();

            auto sortedDisplayedAttribute =
                static_cast<int>(map.getAttributeTable().getColumnSortedIndex(
                    static_cast<size_t>(displayedAttribute)));
            map.setDisplayedAttribute(sortedDisplayedAttribute);
        }
    }

    std::cout << " ok\nWriting out result..." << std::flush;
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
    std::cout << " ok" << std::endl;
}
