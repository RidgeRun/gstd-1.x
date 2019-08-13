#!/usr/bin/env python3
import unittest
import gstc

class TestGstcEventFlushStopMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_event_flush_stop_reset(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.pipeline_play ("p0"), 0)
        self.assertEqual(self.gstd_client.event_flush_stop("p0"), 0)
        self.assertEqual(self.gstd_client.pipeline_stop ("p0"), 0)

    def test_event_flush_stop_no_reset(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        self.assertEqual(self.gstd_client.pipeline_create ("p1", pipeline), 0)
        self.assertEqual(self.gstd_client.pipeline_play ("p1"), 0)
        self.assertEqual(self.gstd_client.event_flush_stop("p1", reset='false'), 0)
        self.assertEqual(self.gstd_client.pipeline_stop ("p1"), 0)

if __name__ == '__main__':
    unittest.main()
