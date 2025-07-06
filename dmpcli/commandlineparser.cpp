// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commandlineparser.hpp"

#include "exceptions.hpp"
#include "imodeparserfactory.hpp"
#include "interfaceversion.hpp"
#include "parsingutils.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

void CommandLineParser::printHelp() {
    std::cout << "Usage: " << APP_NAME
              << " -m <mode> -f <filename> -o <output file> [-s] [-t "
                 "<times.csv>] [-p] [mode options]\n"
              << "       " << APP_NAME << " -v prints the current version\n"
              << "       " << APP_NAME << " -h prints this help text\n"
              << "-s enables simple mode\n"
              << "-t <times.csv> enables output of runtimes as csv file\n"
              << "-p enables text progress printing\n"
              << "-idd ignore display data in metagraph files\n"
              << "-mmv mimic a previous version's quirks\n"

              << "Possible modes are:\n";
    std::for_each(m_parserFactory.getModeParsers().begin(), m_parserFactory.getModeParsers().end(),
                  [](const ModeParserVec::value_type &p) -> void {
                      std::cout << "  " << p->getModeName() << "\n";
                  });
    std::cout << "\n";
    std::for_each(
        m_parserFactory.getModeParsers().begin(), m_parserFactory.getModeParsers().end(),
        [](const ModeParserVec::value_type &p) -> void { std::cout << p->getHelp() << "\n"; });
    std::cout << std::flush;
}

void CommandLineParser::printVersion() { std::cout << TITLE_BASE << "\n" << std::flush; }

CommandLineParser::CommandLineParser(const IModeParserFactory &parserFactory)
    : m_valid(false), m_printVersionMode(false), m_simpleMode(false), m_printProgress(false),
      m_parserFactory(parserFactory), m_modeParser(nullptr) {}

void CommandLineParser::parse(size_t argc, char *argv[]) {
    m_valid = false;
    m_printVersionMode = false;
    if (argc <= 1) {
        throw dmpcli::CommandLineException(
            "No commandline parameters provided - don't know what to do");
    }
    for (size_t i = 1; i < argc;) {
        if (std::strcmp("-h", argv[i]) == 0) {
            return;
        } else if (std::strcmp("-v", argv[i]) == 0) {
            m_printVersionMode = true;
            return;
        } else if (std::strcmp("-m", argv[i]) == 0) {
            if (m_modeParser) {
                throw dmpcli::CommandLineException("-m can only be used once");
            }
            ENFORCE_ARGUMENT("-m", i)

            for (auto iter = m_parserFactory.getModeParsers().begin(),
                      end = m_parserFactory.getModeParsers().end();
                 iter != end; ++iter) {
                if ((*iter)->getModeName() == argv[i]) {
                    m_modeParser = iter->get();
                    break;
                }
            }

            if (!m_modeParser) {
                throw dmpcli::CommandLineException(std::string("Invalid mode: ") + argv[i]);
            }
        } else if (std::strcmp("-f", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-f", i)
            m_fileName = argv[i];
        } else if (std::strcmp("-o", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-o", i)
            m_outputFile = argv[i];
        } else if (std::strcmp("-t", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-t", i)
            m_timingFile = argv[i];
        } else if (std::strcmp("-s", argv[i]) == 0) {
            m_simpleMode = true;
        } else if (std::strcmp("-p", argv[i]) == 0) {
            m_printProgress = true;
        } else if (std::strcmp("-idd", argv[i]) == 0) {
            m_ignoreDisplayData = true;
        } else if (std::strcmp("-mmv", argv[i]) == 0) {
            ENFORCE_ARGUMENT("-t", i)
            m_mimicVersion = argv[i];
        }
        ++i;
    }

    if (!m_modeParser) {
        throw dmpcli::CommandLineException("-m for mode is required");
    }
    if (m_fileName.empty()) {
        throw dmpcli::CommandLineException("-f for input file is required");
    }
    if (m_outputFile.empty()) {
        throw dmpcli::CommandLineException("-o for output file is required");
    }
    m_modeParser->parse(argc, argv);
    m_valid = true;
}

void CommandLineParser::run(IPerformanceSink &perfWriter) const {
    if (!m_valid || !m_modeParser) {
        throw dmpcli::CommandLineException("Trying to run with invalid command line parameters");
    }
    m_modeParser->run(*this, perfWriter);
}
