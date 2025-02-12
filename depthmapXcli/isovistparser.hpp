// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.hpp"

#include "salalib/isovistdef.hpp"

#include <vector>

class IsovistParser : public IModeParser {
  public:
    IsovistParser();

    // IModeParser interface
  public:
    std::string getModeName() const override;
    std::string getHelp() const override;
    void parse(size_t argc, char **argv) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    const std::vector<IsovistDefinition> &getIsovists() const { return m_isovists; }

  private:
    std::vector<IsovistDefinition> m_isovists;
    bool m_closeIsovistPolys = false;
};
