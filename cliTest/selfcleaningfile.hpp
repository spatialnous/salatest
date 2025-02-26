// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <iostream>
#include <stdio.h>
#include <string>

class SelfCleaningFile {
  public:
    SelfCleaningFile(const std::string &filename) : m_filename(filename) {}
    ~SelfCleaningFile() {
        const int result = std::remove(m_filename.c_str());
        if (result != 0) {
            std::cerr << "failed to remove file: " << m_filename
                      << std::endl; // No such file or directory
        }
    }

    const std::string &Filename() { return m_filename; }

  private:
    const std::string m_filename;
};
