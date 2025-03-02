// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "salalib/genlib/exceptions.hpp"

#include <string>

namespace depthmapX {
    class CommandLineException : public depthmapX::BaseException {
      public:
        CommandLineException(std::string message) : depthmapX::BaseException(std::move(message)) {}
    };

    class SetupCheckException : public depthmapX::BaseException {
      public:
        SetupCheckException(std::string message) : depthmapX::BaseException(std::move(message)) {}
    };
} // namespace depthmapX
