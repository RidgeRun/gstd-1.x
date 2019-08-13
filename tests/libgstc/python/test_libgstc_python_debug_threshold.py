#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDebugThresholdMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_debug_threshold_none(self):
        self.assertEqual(self.gstd_client.debug_threshold("0"), 0)

    def test_debug_threshold_error(self):
        self.assertEqual(self.gstd_client.debug_threshold("1"), 0)

    def test_debug_threshold_warning(self):
        self.assertEqual(self.gstd_client.debug_threshold("2"), 0)

    def test_debug_threshold_fixme(self):
        self.assertEqual(self.gstd_client.debug_threshold("3"), 0)

    def test_debug_threshold_info(self):
        self.assertEqual(self.gstd_client.debug_threshold("4"), 0)

    def test_debug_threshold_debug(self):
        self.assertEqual(self.gstd_client.debug_threshold("5"), 0)

    def test_debug_threshold_log(self):
        self.assertEqual(self.gstd_client.debug_threshold("6"), 0)

    def test_debug_threshold_trace(self):
        self.assertEqual(self.gstd_client.debug_threshold("7"), 0)

    def test_debug_threshold_memdump(self):
        self.assertEqual(self.gstd_client.debug_threshold("8"), 0)

    def test_debug_threshold_invalid(self):
        self.assertEqual(self.gstd_client.debug_threshold("9"), 0)
if __name__ == '__main__':
    unittest.main()
