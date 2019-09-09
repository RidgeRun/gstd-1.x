#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDebugEnableMethods(unittest.TestCase):
    def test_debug_enable_true(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.debug_enable("true")

    def test_debug_enable_false(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.debug_enable("false")

if __name__ == '__main__':
    unittest.main()
