# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import config
import depthmaprunner
import performancerunner
import knownresulttestrunner
import os
import sys

import runhelpers
from runhelpers import TestResult
from dxversion import dXversion

defaultConfigFile = "regressionconfig.json"

class RegressionTestRunner():
    def __init__(self, configfile, runfunc):
        self.config = config.RegressionConfig(configfile)
        self.testBinary = runhelpers.getTestExecutable(self.config.testbinlocation)
        print("Binary under test is " + self.testBinary)
        self.baseBinary = runhelpers.getExecutable(self.config.basebinlocation)
        print("Baseline binary is " + self.baseBinary)
        self.runfunc = runfunc
        
        self.__baseCanIDD = self.canIgnoreDisplayData(".", self.baseBinary)
        self.__testCanIDD = self.canIgnoreDisplayData(".", self.testBinary)
        self.__baseVersion = self.getVersion(".", self.baseBinary)
        self.__testVersion = self.getVersion(".", self.testBinary)

    def getVersion(self, runDir, binary):
        if (os.path.isfile(os.path.join(runDir, binary))):
            return dXversion(runhelpers.runExecutable(runDir, [binary, "-v"])[1].strip())
        else:
            return dXversion("0.8.0")
        
    def getHelpText(self, runDir, binary):
        if (os.path.isfile(os.path.join(runDir, binary))):
            return runhelpers.runExecutable(runDir, [binary, "-h"])[1]
        else:
            return ""
        
    def canIgnoreDisplayData(self, runDir, binary):
        return "-idd" in self.getHelpText(runDir, binary)
        
    def canMimicVersion(self, runDir, binary):
        return "-mmv" in self.getHelpText(runDir, binary)
          
    def getExtraArgs(self):
        extraBaseArgs = []
        extraTestArgs = []
        if self.__baseCanIDD and self.__testCanIDD:
            extraBaseArgs = ["-idd"]
            extraTestArgs = ["-idd"]
            print("Both test and base binaries can drop display data. Dropping");
        elif self.canMimicVersion(".", self.testBinary):
            extraTestArgs = ["-mmv", str(self.__baseVersion)]
            print("Test binary can mimic older versions. Mimicking base's \"" + str(self.__baseVersion) + "\"");
        return {"base": extraBaseArgs, "test": extraTestArgs}

    def run(self):
        if not os.path.exists(self.config.rundir):
            os.makedirs(self.config.rundir)
        usesBaseBinary = True
        if self.config.performanceRegression.enabled:
            print("Performance regression runs enabled")
            runner = performancerunner.PerformanceRunner(self.runfunc, self.baseBinary, self.testBinary, self.config.rundir,self.config.performanceRegression )
        elif self.config.knownResultTesting.enabled:
            print("Known result test runs enabled")
            runner = knownresulttestrunner.KnownResultTestRunner(self.runfunc, self.testBinary, self.config.rundir,self.config.knownResultTesting )
            usesBaseBinary = False
        else:
            print("Default regression runs - no performance")
            runner = depthmaprunner.DepthmapRegressionRunner( self.runfunc, self.baseBinary, self.testBinary, self.config.rundir )
        if self.config.allowSkipCases:
            print("Skipping cases allowed")
        else:
            print("Skipping cases not allowed")
        extraArgs = self.getExtraArgs();
        good = True
        for name, case in self.config.testcases.items():
            print("Running test case " + name)

            cmds = case["steps"]
            minVersion = dXversion(case["minVersion"])

            if usesBaseBinary and self.__baseVersion < minVersion:
                good = self.config.allowSkipCases
                reason = ("Baseline binary can not run for test: " + name
                    + " (Binary version \"" + str(self.__baseVersion)
                    + "\" < Test minimum version \"" + str(minVersion) + "\")");
                print ("Skipping:\n" + reason)
            else:
                success, output = runner.runTestCase(name, cmds, extraArgs)
                if not success:
                    good = False
                    print ("Failed:\n" + output)
                else:
                    print("ok")
        return good        

if __name__ == "__main__":
    print("Starting up RegressionTestRunner")
    configFile = defaultConfigFile
    if len(sys.argv) == 2:
        configFile = sys.argv[1]
    print("Config file in use is: " + configFile)
    r = RegressionTestRunner(configFile, runhelpers.runExecutable)
    print("Setup complete, starting run")
    if not r.run():
        exit(-1)

