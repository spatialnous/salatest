// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <string>
#include <vector>

class IModeParserFactory;
class IModeParser;
class IPerformanceSink;

class CommandLineParser {
  public:
    CommandLineParser(const IModeParserFactory &parserFactory);
    void parse(size_t argc, char *argv[]);

    const std::string &getFileName() const { return m_fileName; }
    const std::string &getOuputFile() const { return m_outputFile; }
    const std::string &getTimingFile() const { return m_timingFile; }
    bool isValid() const { return m_valid; }
    bool printVersionMode() const { return m_printVersionMode; }
    bool simpleMode() const { return m_simpleMode; }
    bool printProgress() const { return m_printProgress; }
    const IModeParser &modeOptions() const { return *_modeParser; };

    void printHelp();
    void printVersion();
    void run(IPerformanceSink &perfWriter) const;

  private:
    std::string m_fileName;
    std::string m_outputFile;
    std::string m_timingFile;
    bool m_valid;
    bool m_printVersionMode;
    bool m_simpleMode;
    bool m_printProgress;

    const IModeParserFactory &_parserFactory;
    IModeParser *_modeParser;
};
