#!/bin/bash
# SPDX-FileCopyrightText: 2017 Christian Sailer
# SPDX-FileCopyrightText: 2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later


EXEPREFIXDIR=""
EXESUFFIX=""
if [[ "$OSTYPE" == "cygwin" ]]; then
    EXEPREFIXDIR="/Release"
    EXESUFFIX=".exe"
elif [[ "$OSTYPE" == "msys" ]]; then
    EXEPREFIXDIR="/Release"
    EXESUFFIX=".exe"
fi

echo Running unit tests
./dmcliTest$EXEPREFIXDIR/dmcliTest$EXESUFFIX && \
    ./salaTest$EXEPREFIXDIR/salaTest$EXESUFFIX && \
    ./genlibTest$EXEPREFIXDIR/genlibTest$EXESUFFIX && \
    ./moduleTest$EXEPREFIXDIR/moduleTest$EXESUFFIX --allow-running-no-tests || exit 1
# if that succeeds, run regression tests
echo testing regression test framework
cd ..
ln -s build build-RegressionTest
cd RegressionTest/test && echo pwd && python3 -u test_main.py || exit 1
cd .. && pwd

regression_error=0

echo running known result tests
python3 -u RegressionTestRunner.py knownresulttests_latticemap.json || regression_error=1
python3 -u RegressionTestRunner.py knownresulttests_axial.json || regression_error=1
python3 -u RegressionTestRunner.py knownresulttests_segment.json || regression_error=1
python3 -u RegressionTestRunner.py knownresulttests_isovist.json || regression_error=1
python3 -u RegressionTestRunner.py knownresulttests_conversion.json || regression_error=1
python3 -u RegressionTestRunner.py knownresulttests_agents.json || regression_error=1
echo running standard regression tests
python3 -u RegressionTestRunner.py || regression_error=1
echo running agent test
python3 -u RegressionTestRunner.py regressionconfig_agents.json || regression_error=1
# search the modules directory for regression tests and run them
for subdir in ../modules/*/; do
    regressionFile="${subdir}RegressionTest/regressionconfig.json"
    if [ -e "$regressionFile" ]; then
        python3 -u RegressionTestRunner.py "${regressionFile}" || regression_error=1
    fi 
done

if [ $regression_error -eq 1 ]; then
  echo "One or more regression tests failed."
  exit 1
fi