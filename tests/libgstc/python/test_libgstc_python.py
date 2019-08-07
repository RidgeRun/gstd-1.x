#!/usr/bin/env python3
import unittest
import gstc
import os

class TestGstcMethods(unittest.TestCase):

    def test_init(self):
        gstd_client = gstc.client()

    def test_init_loglevel(self):
        gstd_client = gstc.client(loglevel='DEBUG')

    def test_init_logfile(self):
        f = open("dummy.log","w+")
        f.close()
        num_lines_init = sum(1 for line in open('dummy.log'))
        gstd_client = gstc.client(logfile='dummy.log', loglevel='DEBUG')
        num_lines_final = sum(1 for line in open('dummy.log'))
        self.assertNotEqual(num_lines_init, num_lines_final)
        os.remove("dummy.log")

    def test_bus_filter(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(gstd_client.pipeline_play ("p0"), 0)
        self.assertEqual(gstd_client.event_eos("p0"), 0)
        self.assertEqual(gstd_client.bus_filter("p0", "eos"), 0)
        ret = gstd_client.bus_read("p0")
        self.assertEqual(ret['code'], 0)
        self.assertEqual(ret['response']['type'], 'eos')
        self.assertEqual(gstd_client.pipeline_stop ("p0"), 0)

if __name__ == '__main__':
    unittest.main()
