// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "importparser.hpp"

#include "exceptions.hpp"
#include "parsingutils.hpp"
#include "runmethods.hpp"
#include "simpletimer.hpp"

#include "salalib/importutils.hpp"

#include <cstring>
#include <memory>
#include <sstream>

using namespace depthmapX;

void ImportParser::parse(size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc; ++i) {
        if (strcmp("-if", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-if", i)
            m_filesToImport.push_back(argv[i]);
        } else if (std::strcmp("-it", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-it", i)
            if (std::strcmp(argv[i], "drawing") == 0) {
                m_importMapType = depthmapX::ImportType::DRAWINGMAP;
            } else if (std::strcmp(argv[i], "data") == 0) {
                m_importMapType = depthmapX::ImportType::DATAMAP;
            } else {
                throw CommandLineException(std::string("Invalid map import (-it) type: ") +
                                           argv[i]);
            }
        } else if (strcmp("-iaa", argv[i]) == 0) {
            m_importAsAttributes = true;
        }
    }
}

void ImportParser::run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const {
    std::ifstream mainFileStream(clp.getFileName().c_str());
    if (!mainFileStream.good()) {
        std::stringstream message;
        message << "File not found: " << clp.getFileName() << std::flush;
        throw depthmapX::RuntimeException(message.str().c_str());
    }

    MetaGraphDX metaGraph("Test mgraph");
    DO_TIMED("Load graph file", metaGraph.readFromFile(clp.getFileName());)

    std::optional<std::string> mimicVersion = clp.getMimickVersion();

    if (metaGraph.getReadStatus() == MetaGraphReadWrite::ReadWriteStatus::NOT_A_GRAPH) {
        // not a graph, try to import the file
        std::string ext = clp.getFileName().substr(clp.getFileName().length() - 4,
                                                   clp.getFileName().length() - 1);
        std::ifstream file(clp.getFileName());

        bool asDrawingLines = false;
        depthmapX::ImportFileType importFileType = depthmapX::ImportFileType::TSV;
        if (dXstring::toLower(ext) == ".csv") {
            importFileType = depthmapX::ImportFileType::CSV;
        } else if (dXstring::toLower(ext) == ".dxf") {
            importFileType = depthmapX::ImportFileType::DXF;
            asDrawingLines = true;
        }
        if (asDrawingLines) {
            auto newDrawingFile = metaGraph.loadLineData(dm_runmethods::getCommunicator(clp).get(),
                                                         clp.getFileName(), importFileType, false);
            if (mimicVersion.has_value() && *mimicVersion == "depthmapX 0.8.0") {
                // this version does not actually set the map type of the space pixels
                for (auto &map : metaGraph.getDrawingFiles()[newDrawingFile].maps) {
                    map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
                }
            }
        } else {
            auto newMaps =
                depthmapX::importFile(file, dm_runmethods::getCommunicator(clp).get(),
                                      clp.getFileName(), getImportMapType(), importFileType);
            if (getImportMapType() == depthmapX::ImportType::DATAMAP) {
                for (auto &&map : newMaps) {
                    metaGraph.getDataMaps().emplace_back(
                        std::make_unique<ShapeMap>(std::move(map)));
                }
                if (!metaGraph.getDataMaps().empty()) {
                    metaGraph.setDisplayedDataMapRef(metaGraph.getDataMaps().size() - 1);
                    metaGraph.setState(metaGraph.getState() | MetaGraphDX::DX_DATAMAPS);
                    metaGraph.setViewClass(MetaGraphDX::DX_SHOWHIDESHAPE);
                }
            } else {
                auto newDrawingFile =
                    metaGraph.addDrawingFile(clp.getFileName(), std::move(newMaps));
                metaGraph.setState(metaGraph.getState() | MetaGraphDX::DX_LINEDATA);
                if (mimicVersion.has_value() && *mimicVersion == "depthmapX 0.8.0") {
                    // this version does not actually set the map type of the space pixels
                    for (auto &map : metaGraph.getDrawingFiles()[newDrawingFile].maps) {
                        map.getInternalMap().setMapType(ShapeMap::EMPTYMAP);
                    }
                }
            }
        }
    } else if (metaGraph.getReadStatus() == MetaGraphReadWrite::ReadWriteStatus::OK) {
        if (toImportAsAttrbiutes()) {

            if (metaGraph.getDisplayedMapType() == ShapeMap::EMPTYMAP) {
                throw depthmapX::RuntimeException("No map displayed to attach attributes to");
            }

            std::vector<std::string> fileNames = getFilesToImport();
            for (std::string &fileName : fileNames) {
                std::string ext = fileName.substr(fileName.length() - 4, fileName.length() - 1);
                std::ifstream file(fileName);
                char delimiter = '\t';
                if (dXstring::toLower(ext) == ".csv") {
                    delimiter = ',';
                }

                DO_TIMED("Importing attributes",
                         depthmapX::importAttributes(metaGraph.getDisplayedMapAttributes(), file,
                                                     delimiter);)
            }
        }
    }
    DO_TIMED("Writing graph",
             dm_runmethods::writeGraph(clp, metaGraph, clp.getOuputFile().c_str(), false))
}
