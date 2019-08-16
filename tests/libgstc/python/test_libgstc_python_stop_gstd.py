#!/usr/bin/env python3
import unittest
import subprocess

class TestGstcStopGstdMethods(unittest.TestCase):

    def test_libgstc_python_stop_gstd(self):
        subprocess.Popen(["gstd", "-k"])

if __name__ == '__main__':
    unittest.main()
