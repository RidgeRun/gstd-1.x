#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListSignalsMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_list_signals(self):
        pipeline = "videotestsrc name=v0 ! identity name=i0 ! xvimagesink name=x0"
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.list_signals ("p0", "i0"), [{'name': 'handoff'}])
if __name__ == '__main__':
    unittest.main()
