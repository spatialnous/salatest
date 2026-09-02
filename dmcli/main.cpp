// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commandlineparser.hpp"
#include "modeparserregistry.hpp"
#include "performancewriter.hpp"

#include "dmcli/consts.hpp"

#include <cstddef>
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
    ModeParserRegistry registry;
    CommandLineParser args(registry);
    try {
        args.parse(static_cast<size_t>(argc), argv);
        if (!args.isValid()) {
            if (args.printVersionMode()) {
                args.printVersion();
            } else {
                args.printHelp();
            }
            return 0;
        }

        PerformanceWriter perfWriter(args.getTimingFile());

        args.run(perfWriter);
        perfWriter.write();

    } catch (std::exception &e) {
        std::cout << e.what() << "\n"
                  << "Type '" << APP_NAME << " -h' for help" << std::endl;
        return -1;
    }
    return 0;
}
