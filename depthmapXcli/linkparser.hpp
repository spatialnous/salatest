// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "commandlineparser.h"
#include "imodeparser.h"

#include <string>
#include <vector>

class LinkParser : public IModeParser {
  public:
    enum MapTypeGroup { POINTMAPS, SHAPEGRAPHS };
    enum LinkMode { LINK, UNLINK };
    enum LinkType { COORDS, REFS };

    LinkParser() : m_mapTypeGroup(POINTMAPS), m_linkMode(LINK), m_linkType(COORDS) {}

    std::string getModeName() const override { return "LINK"; }

    std::string getHelp() const override {
        return "Mode options for LINK:\n"
               "  -lmt <type> Map type group to select displayed map from. One of:\n"
               "       pointmaps (default, vga: link)\n"
               "       shapegraphs (axial:link/unlink, segment:link, convex:link)\n"
               "  -lm  <mode> one of:\n"
               "       link (default)\n"
               "       unlink\n"
               "  -lt  <type> one of:\n"
               "       coords (default, provided as x,y or x1,y1,x2,y2 coordinates)\n"
               "       refs (provided as the ids (Ref) of the shapes)\n"
               "  -lnk <single link/unlink coordinates> provided in csv (x1,y1,x2,y2)\n"
               "       for example \"0.1,0.2,0.2,0.4\" to create a link from 0.1,0.2\n"
               "       to 0.2,0.4. In the case of axial-map unlinks a single (x,y) set may\n"
               "       be provided. In the case of refs provide the ids in csv (reffrom,refto)"
               "       Provide multiple times for multiple links/unlinks\n"
               "  -lf  <links file> as in -lnk\n";
    }

  public:
    void parse(size_t argc, char *argv[]) override;
    void run(const CommandLineParser &clp, IPerformanceSink &perfWriter) const override;

    // link options
    const std::string &getLinksFile() const { return m_linksFile; }
    const std::vector<std::string> &getManualLinks() const { return m_manualLinks; }
    const MapTypeGroup &getMapTypeGroup() const { return m_mapTypeGroup; }
    const LinkMode &getLinkMode() const { return m_linkMode; }
    const LinkType &getLinkType() const { return m_linkType; }

  private:
    std::string m_linksFile;
    std::vector<std::string> m_manualLinks;
    MapTypeGroup m_mapTypeGroup;
    LinkMode m_linkMode;
    LinkType m_linkType;
};
