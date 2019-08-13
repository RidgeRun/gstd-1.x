#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDebugColorMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_debug_color_true(self):
        self.assertEqual(self.gstd_client.debug_color("true"), 0)

    def test_debug_color_false(self):
        self.assertEqual(self.gstd_client.debug_color("false"), 0)
if __name__ == '__main__':
    unittest.main()
