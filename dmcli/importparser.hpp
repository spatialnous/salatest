// SPDX-FileCopyrightText: 2017-2026 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.hpp"
#include "imodeparser.hpp"

#include "salalib/importtypedefs.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

class ImportParser : public IModeParser {
  public:
    std::string getModeName() const override { return "IMPORT"; }

    std::string getHelp() const override {
        std::stringstream s;
        s << "Mode options for Importing (mode: " << getModeName()
          << "):\n"
             "  The file provided by -f here will be used as the base. If that file"
             "is not a graph, a new graph will be created and the file will be imported\n"
             "  -if  <file(s) to import> one or more files to import\n"
             "  -it  <type> Import map type (to convert to). Possible map types:\n"
             "       drawing (default, does not preserve attributes, typically for dxf "
             "files)\n"
             "       data (preserves attributes, typically for csv and tsv files)\n"
             "  -iaa will import and attach attributes to an existing map\n";
        return s.str();
    }

  public:
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    const std::vector<std::string> &getFilesToImport() const { return m_filesToImport; }
    bool toImportAsAttrbiutes() const { return m_importAsAttributes; }
    sala::ImportType getImportMapType() const { return m_importMapType; }

  private:
    sala::ImportType m_importMapType = sala::ImportType::DRAWINGMAP;
    std::vector<std::string> m_filesToImport;
    bool m_importAsAttributes = false;
};
