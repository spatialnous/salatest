# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os.path
import cmdlinewrapper
import difflib
import pprint

import runhelpers
from runhelpers import TestResult
from dxversion import dXversion


class DepthmapRunner():
    def __init__(self, runFunc, binary ):
        self.__runFunc = runFunc
        self.__binary = binary
        
    def runDepthmap(self, cmdWrapper, runDir, extraArgs = [], step = 0):
        dirdepth = len(runDir.split(os.path.sep))
        args = [os.path.join(*[".."] * dirdepth, self.__binary)]
        args.extend(cmdWrapper.toCmdArray())
        args.extend(extraArgs)
        return self.__runFunc(runDir, args, step)

def diffBinaryFiles(file1, file2):
    with open(file1, "rb") as f:
        content1 = f.read()
    with open(file2, "rb") as f:
        content2 = f.read()
    gen = difflib.diff_bytes(difflib.unified_diff, [content1], [content2])
    return not(list(gen))

class DepthmapRegressionRunner():
    def __init__(self, runFunc, baseBinary, testBinary, workingDir):
        self.__baseRunner = DepthmapRunner(runFunc, baseBinary)
        self.__testRunner = DepthmapRunner(runFunc, testBinary)
        self.__workingDir = workingDir

    def makeBaseDir(self, name):
        return os.path.join(self.__workingDir, name + "_base")

    def makeTestDir(self, name):
        return os.path.join(self.__workingDir, name + "_test")

    def runTestCaseBase(self, name, cmds, compareFiles, extraArgs = []):
        baseDir = self.makeBaseDir(name)
        for step, cmd in enumerate(cmds):
            (baseSuccess, baseOut) = self.__baseRunner.runDepthmap(cmd, baseDir, extraArgs, step)
            if not baseSuccess:
                print("Baseline run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(baseOut)
                return (None, None, "Baseline run failed at step: " + str(step))

        for compareFile in compareFiles:
            baseFile = os.path.join(baseDir, compareFile)
            if not os.path.exists(baseFile):
                message = "Baseline output {0} does not exist".format(baseFile)
                print (message)
                return (None, None, message)

        return (baseDir, compareFiles, None)

    def runTestCaseTest(self, name, cmds, compareFiles, extraArgs = []):
        testDir = self.makeTestDir(name)
        for step, cmd in enumerate(cmds):
            (testSuccess, testOut) = self.__testRunner.runDepthmap(cmd, testDir, extraArgs, step)
            if not testSuccess:
                print("Test run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(testOut)
                return (None, None, "Test run failed at step: " + str(step))

        for compareFile in compareFiles:
            testFile = os.path.join(testDir, compareFile)
            if not os.path.exists(testFile):
                message = "Test output {0} does not exist".format(testFile)
                print(message)
                return (None, None, message)

        return (testDir, compareFiles, None)

    def runTestCase(self, name, cmds, compareFiles, extraArgs = {"base": [], "test": []}):
        runhelpers.prepareDirectory(self.makeBaseDir(name))
        runhelpers.prepareDirectory(self.makeTestDir(name))
        return self.runTestCaseImpl(name, cmds, compareFiles, extraArgs)
    
    def runTestCaseImpl(self, name, cmds, compareFiles, extraArgs = {"base": [], "test": []}):
        baseDir, baseOutFiles, message = self.runTestCaseBase(name, cmds, compareFiles, extraArgs["base"])
        if baseOutFiles is None:
            return (False, message)

        testDir, testOutFiles, message = self.runTestCaseTest(name, cmds, compareFiles, extraArgs["test"])
        if testOutFiles is None:
            return (False, message)

        for baseOutFile, testOutFile in zip(baseOutFiles, testOutFiles):
            baseFile = os.path.join(baseDir, baseOutFile)
            testFile = os.path.join(testDir, testOutFile)
            
            if not diffBinaryFiles(baseFile, testFile):
                message = "Test outputs differ"
                print (message)
                return (False, message)

        return (True, "")


