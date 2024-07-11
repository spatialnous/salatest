// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "salalib/analysistype.h"
#include "salalib/radiustype.h"

#include <set>
#include <string>

// Options for mean depth calculations
struct Options {
    // Output type, see above
    AnalysisType output_type;
    // Options for the summary type:
    int local;
    int global;
    int cliques;
    //
    bool choice;
    // include measures that can be derived: RA, RRA and total depth
    bool fulloutput;

    RadiusType radius_type;
    double radius; // <- n.b. for metric integ radius is floating point
    // radius has to go up to a list (phase out radius as is)
    std::set<double> radius_set;
    //
    int point_depth_selection;
    int tulip_bins;
    bool process_in_memory;
    bool sel_only;
    bool gates_only;
    // for pushing to a gates layer
    int gatelayer;
    // a column to weight measures by:
    int weighted_measure_col;
    int weighted_measure_col2; // EFEF
    int routeweight_col;       // EFEF
    std::string output_file;   // To save an output graph (for example)
    // default values
    Options() {
        local = 0;
        global = 1;
        cliques = 0;
        choice = false;
        fulloutput = false;
        point_depth_selection = 0;
        tulip_bins = 1024;
        radius = -1;
        radius_type = RadiusType::TOPOLOGICAL;
        output_type = AnalysisType::ISOVIST;
        process_in_memory = false;
        gates_only = false;
        sel_only = false;
        gatelayer = -1;
        weighted_measure_col = -1;
    }
};
