#!/bin/bash
# SPDX-FileCopyrightText: 2017 Christian Sailer
#
# SPDX-License-Identifier: GPL-3.0-or-later


source /opt/qt511/bin/qt511-env.sh
if [ ! -d build ]; then
    mkdir build
fi
cd build
# build
echo Building
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . || exit 1
# if succeeds, run unit tests
source ./runTests.sh
