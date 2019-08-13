#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDebugResetMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_debug_reset_true(self):
        self.assertEqual(self.gstd_client.debug_reset("true"), 0)

    def test_debug_reset_false(self):
        self.assertEqual(self.gstd_client.debug_reset("false"), 0)
if __name__ == '__main__':
    unittest.main()
