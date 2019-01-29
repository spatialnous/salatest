// sala - a component of the depthmapX - spatial network analysis platform
// Copyright (C) 2000-2010, University College London, Alasdair Turner
// Copyright (C) 2011-2012, Tasos Varoudis
// Copyright (C) 2017-2018, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "salalib/segmmodules/segmtopologicalpd.h"

#include "genlib/stringutils.h"

bool SegmentTopologicalPD::run(Communicator *comm, const Options &options, ShapeGraph &map, bool simple_version) {

    dXreimpl::AttributeTable &attributes = map.getAttributeTable();

    bool retvar = true;

    // record axial line refs for topological analysis
    std::vector<int> axialrefs;
    // quick through to find the longest seg length
    std::vector<float> seglengths;
    float maxseglength = 0.0f;
    for (size_t cursor = 0; cursor < map.getShapeCount(); cursor++) {
        axialrefs.push_back(attributes.getRow(dXreimpl::AttributeKey(cursor)).getValue("Axial Line Ref"));
        seglengths.push_back(attributes.getRow(dXreimpl::AttributeKey(cursor)).getValue("Segment Length"));
        if (seglengths.back() > maxseglength) {
            maxseglength = seglengths.back();
        }
    }

    int maxbin = 2;
    std::string prefix = "Topological ";
    std::string depthcol = prefix + "Step Depth";

    attributes.insertOrResetColumn(depthcol.c_str());

    std::vector<unsigned int> seen(map.getShapeCount());
    std::vector<TopoMetSegmentRef> audittrail(map.getShapeCount());
    std::vector<int> list[512]; // 512 bins!
    int open = 0;

    for (size_t i = 0; i < map.getShapeCount(); i++) {
        seen[i] = 0xffffffff;
    }
    for (auto &cursor : map.getSelSet()) {
        seen[cursor] = 0;
        open++;
        double length = seglengths[cursor];
        audittrail[cursor] = TopoMetSegmentRef(cursor, Connector::SEG_CONN_ALL, length * 0.5, -1);
        list[0].push_back(cursor);
        attributes.getRow(dXreimpl::AttributeKey(cursor)).setValue(depthcol.c_str(), 0);
    }

    unsigned int segdepth = 0;
    int bin = 0;

    while (open != 0) {
        while (list[bin].size() == 0) {
            bin++;
            segdepth += 1;
            if (bin == maxbin) {
                bin = 0;
            }
        }
        //
        TopoMetSegmentRef &here = audittrail[list[bin].back()];
        list[bin].pop_back();
        open--;
        // this is necessary using unsigned ints for "seen", as it is possible to add a node twice
        if (here.done) {
            continue;
        } else {
            here.done = true;
        }

        Connector &axline = map.getConnections().at(here.ref);
        int connected_cursor = -2;

        auto iter = axline.m_back_segconns.begin();
        bool backsegs = true;

        while (connected_cursor != -1) {
            if (backsegs && iter == axline.m_back_segconns.end()) {
                iter = axline.m_forward_segconns.begin();
                backsegs = false;
            }
            if (!backsegs && iter == axline.m_forward_segconns.end()) {
                break;
            }

            connected_cursor = iter->first.ref;
            dXreimpl::AttributeRow &row = attributes.getRow(
                dXreimpl::AttributeKey(depthmapX::getMapAtIndex(map.getAllShapes(), connected_cursor)->first));
            if (seen[connected_cursor] > segdepth) {
                float length = seglengths[connected_cursor];
                int axialref = axialrefs[connected_cursor];
                seen[connected_cursor] = segdepth;
                audittrail[connected_cursor] =
                    TopoMetSegmentRef(connected_cursor, here.dir, here.dist + length, here.ref);
                // puts in a suitable bin ahead of us...
                open++;
                //
                if (axialrefs[here.ref] == axialref) {
                    list[bin].push_back(connected_cursor);
                    row.setValue(depthcol.c_str(), segdepth);
                } else {
                    list[(bin + 1) % 2].push_back(connected_cursor);
                    seen[connected_cursor] =
                        segdepth +
                        1; // this is so if another node is connected directly to this one but is found later it is
                           // still handled -- note it can result in the connected cursor being added twice
                    row.setValue(depthcol.c_str(), segdepth + 1);
                }
            }
            iter++;
        }
    }

    map.setDisplayedAttribute(attributes.getColumnIndex(depthcol.c_str()));

    return retvar;
}
