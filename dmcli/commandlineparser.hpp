// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2026 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
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
    bool printQuirksMode() const { return m_printQuirksMode; }
    bool simpleMode() const { return m_simpleMode; }
    bool printProgress() const { return m_printProgress; }
    bool ignoreDisplayData() const { return m_ignoreDisplayData; }
    const std::optional<std::string> &getMimickVersion() const { return m_mimicVersion; }
    const IModeParser &modeOptions() const { return *m_modeParser; }

    void printHelp();
    void printVersion();
    void printQuirks();
    bool quirkKnown(const std::string &op) const;
    void run(IPerformanceSink &perfWriter) const;

  private:
    std::string m_fileName;
    std::string m_outputFile;
    std::string m_timingFile;
    bool m_valid;
    bool m_printVersionMode;
    bool m_printQuirksMode;
    bool m_simpleMode;
    bool m_printProgress;
    bool m_ignoreDisplayData = false;
    std::optional<std::string> m_mimicVersion = std::nullopt;
    std::optional<std::vector<std::string>> m_enabledQuirks = std::nullopt;

    const IModeParserFactory &m_parserFactory;
    IModeParser *m_modeParser;
    static constexpr std::array<std::string_view, 1> ACCEPTED_QUIRKS = {
        "legacy-agent-loc-rng" //
    };
};
