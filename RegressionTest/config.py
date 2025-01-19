# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import json
import os.path
import cmdlinewrapper
from performanceregressionconfig import PerformanceRegressionConfig
from knownresulttestconfig import KnownResultTestConfig


class ConfigError(Exception):
    def __init__(self, message):
        self.message = message

def buildCmd(testcaseSteps):
    cmds = [];
    for testcase in testcaseSteps:
        cmd = cmdlinewrapper.DepthmapCmd()
        cmd.infile = testcase["infile"]
        cmd.outfile = testcase["outfile"]
        cmd.mode = testcase["mode"]
        if "simple" in testcase and not testcase["simple"]  == "false":
            cmd.simpleMode = True
        cmd.extraArgs = testcase.get("extraArgs", {})
        cmds.append(cmd)
    return cmds

class RegressionConfig():
    def __init__(self, filename):
        with open(filename, "r") as f:
            config = json.load(f)
        configdir = os.path.dirname(filename)
        self.rundir = config["rundir"]
        if isinstance(self.rundir, list):
            self.rundir = os.path.join(*self.rundir)
        self.basebinlocation = config["basebinlocation"]
        self.testbinlocation = config["testbinlocation"]
        self.allowSkipCases = config["allowskip"]
        self.performanceRegression = PerformanceRegressionConfig(config.get("performance", None))
        self.knownResultTesting = KnownResultTestConfig(config.get("knownresult", None))
        self.testcases = {}
        for (name, tc) in config["testcases"].items():
            self.testcases[name] = {
                "steps": buildCmd(tc['steps']),
                "minVersion": tc['minVersion'],
                "compareFiles": tc['compareFiles']
            }


