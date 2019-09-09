#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListElementsMethods(unittest.TestCase):

    def test_list_elements(self):
        pipeline = "videotestsrc name=v0 ! fakesink name=x0"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        self.assertEqual(self.gstd_client.list_elements ("p0"), [{'name': 'x0'}, {'name': 'v0'}])
        self.gstd_client.pipeline_delete ("p0")

if __name__ == '__main__':
    unittest.main()
