#!/usr/bin/env python3
import unittest
import gstc

class TestGstcBusReadMethods(unittest.TestCase):

    def test_bus_read_eos(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.pipeline_play ("p0"), 0)
        self.assertEqual(self.gstd_client.event_eos("p0"), 0)
        self.assertEqual(self.gstd_client.bus_filter("p0", "eos"), 0)
        ret = self.gstd_client.bus_read("p0")
        self.assertEqual(ret['code'], 0)
        self.assertEqual(ret['response']['type'], 'eos')
        self.assertEqual(self.gstd_client.pipeline_stop ("p0"), 0)
        self.assertEqual(self.gstd_client.pipeline_delete ("p0"), 0)
if __name__ == '__main__':
    unittest.main()
