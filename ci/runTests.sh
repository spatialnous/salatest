#!/bin/bash
# SPDX-FileCopyrightText: 2017 Christian Sailer
# SPDX-FileCopyrightText: 2024-2026 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

EXESUFFIX=""

# The .exe suffix depends on the OS...
case "$OSTYPE" in
    cygwin|msys|win32) EXESUFFIX=".exe" ;;
esac

# MinGW links against the toolchain's libstdc++/libgcc, but the Windows loader
# never searches the toolchain directory. Put it on PATH so the tests can start.
if [ -n "$EXESUFFIX" ] && [ -f CMakeCache.txt ]; then
    cxx=$(grep -m1 '^CMAKE_CXX_COMPILER:' CMakeCache.txt | cut -d= -f2)
    cxxdir=$(cygpath -u "$(dirname "$cxx")")
    if [ -f "$cxxdir/libstdc++-6.dll" ]; then
        export PATH="$cxxdir:$PATH"
        echo "added $cxxdir to PATH for the MinGW runtime"
    fi
fi

echo Running unit tests
ls -l ./bin/
command -v ldd >/dev/null && ldd ./bin/dmcliTest$EXESUFFIX

for t in dmcliTest salaTest genlibTest moduleTest; do
    args=""
    [ "$t" = moduleTest ] && args="--allow-running-no-tests"
    echo "--- $t ---"
    "./bin/$t$EXESUFFIX" $args
    rc=$?
    echo "$t exit code: $rc"
    [ $rc -ne 0 ] && exit 1
done

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
