# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os
import shutil
import subprocess
import platform
from enum import Enum

def runTest():
   pass 

class TestResult(Enum):
    PASS = 1
    SKIP = 2
    FAIL = 3

# For terminal colours https://stackoverflow.com/a/287944
class bcolours:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


def prepareDirectory(dirname):
    if os.path.exists( dirname ):
        shutil.rmtree(dirname)
    os.makedirs(dirname)

def runExecutable( workingDir, arguments, step = 0 ):
    """ Prepares a clean run directoy and runs the process in this """
    with cd(workingDir):
        outfilename = "out-" + str(step) + ".txt"
        errfilename = "err-" + str(step) + ".txt"
        with open(outfilename, "w") as outfile:
            result = subprocess.run(arguments, stdout = outfile, stderr = subprocess.STDOUT )
        output = ""
        if os.path.exists(outfilename):
            with open( outfilename, "r" ) as f:
                output = f.read()
        if os.path.exists( errfilename ):
            with open( errfilename, "r") as f:
                error = f.read();
        return (result.returncode == 0, output)

def getExecutable(basedir):
    sys = platform.system()
    machine = platform.machine()
    if sys == "Windows":
        return os.path.join(basedir, sys + "_" + machine, "dmcli.exe")
    else:
        return os.path.join(basedir, sys + "_" + machine, "dmcli")

def getTestExecutable(basedir):
    sys = platform.system()
    if sys == "Windows":
        return os.path.join(basedir, "dmcli", "release", "dmcli.exe")
    else:
        return os.path.join(basedir, "dmcli", "dmcli")


