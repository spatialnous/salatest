# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2025 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

from statistics import mean
from collections import OrderedDict

import depthmaprunner
import difflib
import os
import csv

import runhelpers

def diffBinaryFiles(file1, file2):
    with open(file1, "rb") as f:
        content1 = f.read()
    with open(file2, "rb") as f:
        content2 = f.read()
    gen = difflib.diff_bytes(difflib.unified_diff, [content1], [content2])
    return not(list(gen))

class KnownResultTestRunner(depthmaprunner.DepthmapRegressionRunner):
    def __init__(self, runFunc, testBinary, workingDir, config):
        depthmaprunner.DepthmapRegressionRunner.__init__(self,runFunc,None,testBinary,workingDir)
        self.config = config

    def runTestCase(self, name, cmds, extraArgs = {"test": []}):
        if not os.path.exists(self.config.resultsdir):
            return (False, "Config does not specify expected results directory")

        knownResultDir = os.path.join(self.config.resultsdir, name)

        if not os.path.exists(knownResultDir):
            return (False, "Test {0} does not have known results (expected folder {1})".format(name, knownResultDir))

        runhelpers.prepareDirectory(self.makeTestDir(name))

        nameTemplate = "timings_{0}_{1}.csv"

        testDir, testOutFile, message = self.runTestCaseTest(name, cmds, extraArgs["test"])

        if testOutFile is None:
            return (False, "Run {0} failed with message: {1}".format(i, message))

        if testOutFile.endswith(".graph"):
            return (False, "Can not run with graph files ({0})".format(result))
        
        testFile = os.path.join(testDir, testOutFile)
        knownResultFile = os.path.join(knownResultDir, testOutFile)

        if not os.path.exists(knownResultFile):
            return (False, "Test {0} does not have known results (expected file {1})".format(name, knownResultFile))

        if not diffBinaryFiles(knownResultFile, testFile):
            message = "Test outputs differ"
            print (message)
            return (False, message)

        return (True, "")
