# SPDX-FileCopyrightText: 2017 Christian Sailer
#
# SPDX-License-Identifier: GPL-3.0-or-later

from context import depthmaprunner

import cmdlinewrapper
from cmdlinewrapper import DepthmapCmd
import unittest
from disposablefile import DisposableFile, DisposableDirectory
import os.path
import os.name

class BinaryDiffTest(unittest.TestCase):
    def test_binaryDiff(self):
        with DisposableFile( "testfile1.bin" ) as f1, DisposableFile( "testfile2.bin") as f2:
            with open( f1.filename(), "wb" ) as f:
                f.write( bytes.fromhex( "2000 0839") )
            with open( f2.filename(), "wb" ) as f:
                f.write( bytes.fromhex( "2000 0838" ))
            self.assertTrue(depthmaprunner.diffBinaryFiles(f1.filename(), f1.filename()))
            self.assertFalse(depthmaprunner.diffBinaryFiles(f1.filename(), f2.filename()))




class DepthmapRunnerTest(unittest.TestCase):
    def runfunc(self, rundir, args):
        self.assertEqual(rundir, self.__rundir)
        self.assertEqual(args, self.__args)

    def testDepthmapRunner(self):
        dcmd = DepthmapCmd()
        dcmd.infile = "foo"
        dcmd.outfile = "bar"
        dcmd.mode = "mode"
        dcmd.modeLines = []

        runner = depthmaprunner.DepthmapRunner(lambda d, a: self.runfunc(d, a), "bin")
        binDir = "../bin"
        if os.name == 'nt':
            binDir = "..\\bin"

        self.__args = [binDir, "-f", "foo", "-o", "bar", "-m", "mode"]
        self.__rundir = "dir"
        runner.runDepthmap(dcmd, "dir")

class DepthmapRegressioRunnerTest(unittest.TestCase):
    def getOutfile(self, args):
        outputfile = None
        for i in range(0, len(args)):
            if args[i] == "-o" and i < len(args):
                outputfile = args[i+1]
                break
        self.assertFalse( outputfile == None )
        return outputfile

    def getTimingsFile(self,args):
        timingsFile = None
        for i in range(0, len(args)):
            if args[i] == "-t" and i < len(args):
                timingsFile = args[i+1]
                break
        return timingsFile

    
    def runfuncSucceedAlwaysSame(self, rundir, args):
        outpath = os.path.join(rundir, self.getOutfile(args))
        with open (outpath, "w") as f:
            f.write("123")
        timingsFile = self.getTimingsFile(args)
        if timingsFile:
            with open (os.path.join(rundir, timingsFile), "w") as f:
                f.write('"action","duration"\n')
        return (True, "")

    def runfuncDifferentResults(self, rundir, args):
        outpath = os.path.join(rundir, self.getOutfile(args))
        with open (outpath, "w") as f:
            f.write(self.__outContent)
        self.__outContent = self.__outContent + "x"    
        return (True, "")
        
    def runfuncWriteNoFile(self, rundir, args, dontWriteFor):
        if not args[0] == dontWriteFor:
            outpath = os.path.join(rundir, self.getOutfile(args))
            with open (outpath, "w") as f:
                f.write("123")
        return (True, "")

    def runfuncFail(self, rundir, args, failFor, shouldOtherRun):
        if args[0] == failFor:
            return (False, "Boom!")
        if shouldOtherRun:
            outpath = os.path.join(rundir, self.getOutfile(args))
            with open (outpath, "w") as f:
                f.write("123")
            return (True, "")
        else:
            self.fail("Should not have been called for " + args[0])
       
            
    def makeCommand(self, infile, outfile, mode):
        cmd = cmdlinewrapper.DepthmapCmd()
        cmd.infile = infile
        cmd.outfile = outfile
        cmd.mode = mode
        return [cmd]


    def testSuccessfullRun(self):
        with DisposableDirectory("testdir", True) as dir:
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncSucceedAlwaysSame(d,a), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertTrue(result)
            
    def testRunWithDiff(self):
        self.__outContent = "abc"
        with DisposableDirectory("testdir", True) as dir:
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncDifferentResults(d,a), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertFalse(result)
            self.assertEqual(message, "Test outputs differ")

    def testBaseRunOutputMissing(self):
        with DisposableDirectory("testdir", True) as dir:
            binPath = os.path.join("..", "..", "basebin")
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncWriteNoFile(d,a, binPath), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertFalse(result)
            self.assertEqual(message, "Baseline output {0} does not exist".format(os.path.join(dir.name(), "testname" + "_base", "outfile.graph")))

    def testTestRunOutputMissing(self):
        with DisposableDirectory("testdir", True) as dir:
            binPath = os.path.join("..", "..", "testbin")
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncWriteNoFile(d,a, binPath), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertFalse(result)
            self.assertEqual(message, "Test output {0} does not exist".format(os.path.join(dir.name(), "testname" + "_test", "outfile.graph")))

    def testBaseRunFail(self):
        with DisposableDirectory("testdir", True) as dir:
            binPath = os.path.join("..", "..", "basebin")
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncFail(d,a, binPath, False), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertFalse(result)
            self.assertEqual(message, "Baseline run failed at step: 0")

    def testTestRunFail(self):
        with DisposableDirectory("testdir", True) as dir:
            binPath = os.path.join("..", "..", "testbin")
            runner = depthmaprunner.DepthmapRegressionRunner(lambda d, a: self.runfuncFail(d,a, binPath, True), "basebin", "testbin", dir.name())
            (result, message) = runner.runTestCase("testname", self.makeCommand("infile.graph", "outfile.graph", "visibility"), ["outfile.graph"])
            self.assertFalse(result)
            self.assertEqual(message, "Test run failed at step: 0")


if __name__=="__main__":
    unittest.main()
