#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListSignalsMethods(unittest.TestCase):

    def test_list_signals(self):
        pipeline = "videotestsrc name=v0 ! identity name=i0 ! fakesink name=x0"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        self.assertEqual(self.gstd_client.list_signals ("p0", "i0"), [{'name': 'handoff'}])
        self.gstd_client.pipeline_delete ("p0")

if __name__ == '__main__':
    unittest.main()
