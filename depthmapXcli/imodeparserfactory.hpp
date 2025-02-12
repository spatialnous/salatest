// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparser.h"

#include <memory>
#include <vector>

typedef std::vector<std::unique_ptr<IModeParser>> ModeParserVec;

class IModeParserFactory {
  public:
    virtual const ModeParserVec &getModeParsers() const = 0;
    virtual ~IModeParserFactory() {}
};
