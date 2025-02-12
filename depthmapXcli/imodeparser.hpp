// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// Interface to encapsulate handling command line and invoking the respective
// depthmapX mode

#include "commandlineparser.h"
#include "performancesink.h"

class IModeParser {
  public:
    virtual std::string getModeName() const = 0;
    virtual std::string getHelp() const = 0;
    virtual void parse(size_t argc, char **argv) = 0;
    virtual void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const = 0;
    virtual ~IModeParser() {}
};
