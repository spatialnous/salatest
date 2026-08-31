// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "performancewriter.hpp"

#include <algorithm>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

PerformanceWriter::PerformanceWriter(const std::string &filename) : m_filename(filename) {}

void PerformanceWriter::addData(const std::string &message, double timeInSeconds) {
    std::stringstream ss;
    ss << "\"" << message << "\"," << timeInSeconds << "\n";
    m_data.push_back(ss.str());
}

void PerformanceWriter::write() const {
    if (!m_filename.empty()) {
        std::ofstream outfile(m_filename);
        outfile << "\"action\",\"duration\"\n";
        std::for_each(m_data.begin(), m_data.end(),
                      [&outfile](const std::string &line) mutable -> void { (outfile) << line; });
        outfile << std::flush;
    }
}
