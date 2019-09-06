#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDebugEnableMethods(unittest.TestCase):
    def test_debug_enable_true(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(self.gstd_client.debug_enable("true"), 0)

    def test_debug_enable_false(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(self.gstd_client.debug_enable("false"), 0)

if __name__ == '__main__':
    unittest.main()
