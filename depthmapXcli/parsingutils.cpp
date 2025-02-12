// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parsingutils.hpp"

#include "exceptions.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

std::vector<double> depthmapX::parseRadiusList(const std::string &radiusList) {
    std::vector<double> result;
    std::stringstream stream(radiusList);
    bool addN = false;

    while (stream.good()) {
        std::string value;
        getline(stream, value, ',');
        if (value == "n" || value == "N") {
            addN = true;
        } else {
            char *end;
            long int val = std::strtol(value.c_str(), &end, 10);
            if (val == 0) {
                std::stringstream message;
                message << "Found either 0 or unparsable radius " << value << std::flush;
                throw CommandLineException(message.str());
            }
            if (val < 0) {
                throw CommandLineException("Radius must be either n or a positive integer");
            }
            if (strlen(end) > 0) {
                std::stringstream message;
                message << "Found non integer radius " << value << std::flush;
                throw CommandLineException(message.str());
            }
            result.push_back((double)val);
        }
    }

    std::sort(result.begin(), result.end());
    if (result.empty() || addN) {
        result.push_back(-1.0);
    }
    return result;
}
