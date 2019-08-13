#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListElementsMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_list_elements(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink name=x0"
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.list_elements ("p0"), [{'name': 'x0'}, {'name': 'v0'}])
if __name__ == '__main__':
    unittest.main()
