// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "modeparserregistry.hpp"

#include "agentparser.hpp"
#include "axialparser.hpp"
#include "exportparser.hpp"
#include "importparser.hpp"
#include "isovistparser.hpp"
#include "linkparser.hpp"
#include "mapconvertparser.hpp"
#include "segmentparser.hpp"
#include "segmentshortestpathparser.hpp"
#include "stepdepthparser.hpp"
#include "vgaparser.hpp"
#include "visprepparser.hpp"

void ModeParserRegistry::populateParsers() {
    // Register any mode parsers here
    REGISTER_PARSER(VgaParser)
    REGISTER_PARSER(LinkParser)
    REGISTER_PARSER(VisPrepParser)
    REGISTER_PARSER(AxialParser)
    REGISTER_PARSER(SegmentParser)
    REGISTER_PARSER(AgentParser)
    REGISTER_PARSER(IsovistParser)
    REGISTER_PARSER(ExportParser)
    REGISTER_PARSER(ImportParser)
    REGISTER_PARSER(StepDepthParser)
    REGISTER_PARSER(MapConvertParser)
    REGISTER_PARSER(SegmentShortestPathParser)
    // *********
}
