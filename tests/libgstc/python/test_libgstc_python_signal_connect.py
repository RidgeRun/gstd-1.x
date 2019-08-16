#!/usr/bin/env python3
import unittest
import gstc

class TestGstcSignalConnectMethods(unittest.TestCase):

    def test_libgstc_python_signal_connect(self):
        pipeline = "videotestsrc ! identity signal-handoffs=true name=identity ! xvimagesink"
        gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(gstd_client.pipeline_play ("p0"), 0)
        ret = gstd_client.signal_connect("p0", "identity", "handoff")
        self.assertEqual(ret['response']['name'], 'handoff')

if __name__ == '__main__':
    unittest.main()
