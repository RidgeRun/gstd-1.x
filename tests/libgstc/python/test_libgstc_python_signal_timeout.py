#!/usr/bin/env python3
import unittest
import threading
import gstc

class TestGstcSignalTimeoutMethods(unittest.TestCase):

    def test_libgstc_python_signal_timeout(self):
        pipeline = "videotestsrc ! identity name=identity ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.signal_timeout("p0", "identity", "handoff", "1"), 0)
        ret_con = self.gstd_client.signal_connect("p0", "identity", "handoff")
        self.assertEqual(ret_con['response'], None)
        self.assertEqual(self.gstd_client.pipeline_stop ("p0"), 0)
        self.assertEqual(self.gstd_client.pipeline_delete ("p0"), 0)

if __name__ == '__main__':
    unittest.main()

