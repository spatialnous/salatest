// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "salalib/genlib/exceptions.h"

#include <string>

namespace depthmapX {
    class CommandLineException : public depthmapX::BaseException {
      public:
        CommandLineException(std::string message) : depthmapX::BaseException(message) {}
    };

    class SetupCheckException : public depthmapX::BaseException {
      public:
        SetupCheckException(std::string message) : depthmapX::BaseException(message) {}
    };
} // namespace depthmapX
