#!/usr/bin/env python3
import unittest
import gstc

class TestGstcEventEosMethods(unittest.TestCase):

    def test_event_eos(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        self.gstd_client.pipeline_play ("p0")
        self.gstd_client.event_eos("p0")
        self.gstd_client.bus_filter("p0", "eos")
        ret = self.gstd_client.bus_read("p0")
        self.assertEqual(ret['type'], 'eos')
        self.gstd_client.pipeline_stop ("p0")
        self.gstd_client.pipeline_delete ("p0")

if __name__ == '__main__':
    unittest.main()
