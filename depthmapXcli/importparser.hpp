// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "imodeparser.h"

#include "salalib/importtypedefs.h"

#include <string>
#include <vector>

class ImportParser : public IModeParser {
  public:
    std::string getModeName() const override { return "IMPORT"; }

    std::string getHelp() const override {
        return "Mode options for IMPORT:\n"
               "   The file provided by -f here will be used as the base. If that file"
               "is not a graph, a new graph will be created and the file will be imported\n"
               "   -if <file(s) to import> one or more files to import\n"
               "   -it Import map type (to convert to)\n"
               "       Possible map types:\n"
               "         - drawing (default, does not preserve attributes, typically for dxf "
               "files)\n"
               "         - data (preserves attributes, typically for csv and tsv files)\n"
               "   -iaa will import and attach attributes to an existing map\n";
    }

  public:
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    const std::vector<std::string> &getFilesToImport() const { return m_filesToImport; }
    bool toImportAsAttrbiutes() const { return m_importAsAttributes; }
    depthmapX::ImportType getImportMapType() const { return m_importMapType; }

  private:
    depthmapX::ImportType m_importMapType = depthmapX::ImportType::DRAWINGMAP;
    std::vector<std::string> m_filesToImport;
    bool m_importAsAttributes = false;
};
