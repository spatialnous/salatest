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
        
    def runDepthmap(self, cmdWrapper, runDir, extraArgs = []):
        dirdepth = len(runDir.split(os.path.sep)) - 1
        args = [os.path.join("..", *[".."] * dirdepth, self.__binary)]
        args.extend(cmdWrapper.toCmdArray())
        args.extend(extraArgs)
        return self.__runFunc(runDir, args)

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

    def runTestCaseBase(self, name, cmds, extraArgs = []):
        baseDir = self.makeBaseDir(name)
        for step,cmd in enumerate(cmds):
            (baseSuccess, baseOut) = self.__baseRunner.runDepthmap(cmd, baseDir, extraArgs)
            if not baseSuccess:
                print("Baseline run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(baseOut)
                return (False, "Baseline run failed at step: " + str(step))
        
        baseFile = os.path.join(baseDir, cmds[-1].outfile)
        if not os.path.exists(baseFile):
            message = "Baseline output {0} does not exist".format(baseFile)
            print (message)
            return (None, None, message)

        return (baseDir, cmds[-1].outfile, None)

    def runTestCaseTest(self, name, cmds, extraArgs = []):
        testDir = self.makeTestDir(name)
        for step,cmd in enumerate(cmds):
            (testSuccess, testOut) = self.__testRunner.runDepthmap(cmd, testDir, extraArgs)
            if not testSuccess:
                print("Test run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(testOut)
                return (False, "Test run failed at step: " + str(step))

        testFile = os.path.join(testDir, cmds[-1].outfile)
        if not os.path.exists(testFile):
            message = "Test output {0} does not exist".format(testFile)
            print(message)
            return (None, None, message)

        return (testDir, cmds[-1].outfile, None)

    def runTestCase(self, name, cmds, extraArgs = {"base": [], "test": []}):
        runhelpers.prepareDirectory(self.makeBaseDir(name))
        runhelpers.prepareDirectory(self.makeTestDir(name))
        return self.runTestCaseImpl(name, cmds, extraArgs)
    
    def runTestCaseImpl(self, name, cmds, extraArgs = {"base": [], "test": []}):
        baseDir, baseOutFile, message = self.runTestCaseBase(name, cmds, extraArgs["base"])
        if baseOutFile is None:
            return (False, message)
        baseFile = os.path.join(baseDir, baseOutFile)

        testDir, testOutFile, message = self.runTestCaseBase(name, cmds, extraArgs["test"])
        if testOutFile is None:
            return (False, message)
        testFile = os.path.join(testDir, testOutFile)
        
        if not diffBinaryFiles(baseFile, testFile):
            message = "Test outputs differ"
            print (message)
            return (False, message)

        return (True, "")
    
    def runTestCaseNoBase(self, name, cmds, extraArgs = {"base": [], "test": []}):
        baseDir = self.makeBaseDir(name)
        for step,cmd in enumerate(cmds):
            (baseSuccess, baseOut) = self.__baseRunner.runDepthmap(cmd, baseDir, extraArgs["base"])
            if not baseSuccess:
                print("Baseline run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(baseOut)
                return (False, "Baseline run failed at step: " + str(step))

        testDir = self.makeTestDir(name)
        for step,cmd in enumerate(cmds):
            (testSuccess, testOut) = self.__testRunner.runDepthmap(cmd, testDir, extraArgs["test"])
            if not testSuccess:
                print("Test run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(testOut)
                return (False, "Test run failed at step: " + str(step))

        baseFile = os.path.join(baseDir, cmds[-1].outfile)
        testFile = os.path.join(testDir, cmds[-1].outfile)
        if not os.path.exists(baseFile):
            message = "Baseline output {0} does not exist".format(baseFile)
            print (message)
            return (False, message)
        if not os.path.exists(testFile):
            message = "Test output {0} does not exist".format(testFile)
            print(message)
            return (False, message)
        
        if not diffBinaryFiles(baseFile, testFile):
            message = "Test outputs differ"
            print (message)
            return (False, message)

        return (True, "")


