#!/usr/bin/env python3
import unittest
import gstc

class TestGstcSignalConnectMethods(unittest.TestCase):

    def test_libgstc_python_signal_connect(self):
        pipeline = "videotestsrc ! identity signal-handoffs=true name=identity ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        self.gstd_client.pipeline_play ("p0")
        ret = self.gstd_client.signal_connect("p0", "identity", "handoff")
        self.assertEqual(ret['name'], 'handoff')
        self.gstd_client.pipeline_stop ("p0")
        self.gstd_client.pipeline_delete ("p0")

if __name__ == '__main__':
    unittest.main()
