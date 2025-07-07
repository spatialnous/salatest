// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "salalib/genlib/exceptions.hpp"

#include <string>

namespace dmcli {
    class CommandLineException : public genlib::BaseException {
      public:
        CommandLineException(std::string message) : genlib::BaseException(std::move(message)) {}
    };

    class SetupCheckException : public genlib::BaseException {
      public:
        SetupCheckException(std::string message) : genlib::BaseException(std::move(message)) {}
    };
} // namespace dmcli
