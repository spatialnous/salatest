// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdio.h>
#include <string>

class SelfCleaningFile {
  public:
    SelfCleaningFile(const std::string &filename) : m_filename(filename) {}
    ~SelfCleaningFile() { std::remove(m_filename.c_str()); }

    const std::string &Filename() { return m_filename; }

  private:
    const std::string m_filename;
};
