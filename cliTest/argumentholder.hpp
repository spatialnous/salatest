// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>

class ArgumentHolder {
  public:
    ArgumentHolder(std::initializer_list<std::string> l) : m_arguments(l) {
        for (auto &arg : m_arguments) {
            m_argv.push_back(arg.data());
        }
    }

    char **argv() const { return (char **)m_argv.data(); }

    size_t argc() const { return m_argv.size(); }

  private:
    std::vector<std::string> m_arguments;
    std::vector<const char *> m_argv;
};
