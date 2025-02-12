// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "imodeparserfactory.h"

#include <memory>
#include <vector>

class ModeParserRegistry : public IModeParserFactory {
  public:
    ModeParserRegistry() { populateParsers(); }

    const ModeParserVec &getModeParsers() const override { return m_availableParsers; }

  private:
    void populateParsers();
    ModeParserVec m_availableParsers;
};

#define REGISTER_PARSER(parser)                                                                    \
    m_availableParsers.push_back(std::unique_ptr<IModeParser>(new parser));
