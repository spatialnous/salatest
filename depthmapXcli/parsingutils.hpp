// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define PASTE(arg) arg
#define ENFORCE_ARGUMENT(flag, counter)                                                            \
    if (++PASTE(counter) >= argc ||                                                                \
        (argv[PASTE(counter)][0] == '-' && !isdigit(argv[PASTE(counter)][1]) &&                    \
         argv[PASTE(counter)][1] != '.')) {                                                        \
        throw CommandLineException(flag " requires an argument");                                  \
    }

#include <string>
#include <vector>

namespace depthmapX {

    inline bool has_only_digits(const std::string &s) {
        return s.find_first_not_of("0123456789") == std::string::npos;
    }

    inline bool has_only_digits_dots(const std::string &s) {
        return s.find_first_not_of("0123456789.") == std::string::npos;
    }

    inline bool has_only_digits_dots_commas(const std::string &s) {
        return s.find_first_not_of("0123456789,.-") == std::string::npos;
    }

    std::vector<double> parseRadiusList(const std::string &radiusList);

} // namespace depthmapX
