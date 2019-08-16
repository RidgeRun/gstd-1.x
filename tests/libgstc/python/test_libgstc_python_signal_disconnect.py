#!/usr/bin/env python3
import unittest
import threading
import gstc
import time
import os

ret_val=""

def signal_connect_test():
    global ret_val
    gstd_client = gstc.client(loglevel='DEBUG', port=5001)
    ret_val = gstd_client.signal_connect("p0", "identity", "handoff")

class TestGstcSignalDisconnectMethods(unittest.TestCase):

    def test_libgstc_python_signal_disconnect(self):
        global ret_val
        pipeline = "videotestsrc is-live=true ! identity sleep-time=10000000 signal-handoffs=false name=identity ! xvimagesink"
        gstd_client = gstc.client(loglevel='DEBUG', nports=2)
        self.assertEqual(gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(gstd_client.pipeline_play ("p0"), 0)
        ret_thr = threading.Thread(target=signal_connect_test)
        ret_thr.start()
        time.sleep(1)
        self.assertEqual(gstd_client.signal_disconnect("p0", "identity", "handoff"), 0)
        time.sleep(1)
        self.assertEqual(ret_val['response'], None)

if __name__ == '__main__':
    unittest.main()
