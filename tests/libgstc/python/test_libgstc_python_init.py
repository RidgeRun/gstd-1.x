#!/usr/bin/env python3
import unittest
import gstc
import os
import subprocess

class TestGstcInitMethods(unittest.TestCase):
    def setUp(self):
        subprocess.Popen(["gstd", "-p", "5000", "-n", "2"])

    def test_init(self):
        self.gstd_client = gstc.client()

    def test_init_loglevel(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_init_logfile(self):
        f = open("dummy.log","w+")
        f.close()
        f = open('dummy.log')
        num_lines_init = sum(1 for line in f)
        f.close()
        self.gstd_client = gstc.client(logfile='dummy.log', loglevel='DEBUG')
        f = open('dummy.log')
        num_lines_final = sum(1 for line in f)
        f.close()
        self.assertNotEqual(num_lines_init, num_lines_final)
        os.remove("dummy.log")

if __name__ == '__main__':
    unittest.main()
