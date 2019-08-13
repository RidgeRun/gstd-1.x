#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListPropertiesMethods(unittest.TestCase):
    def setUp(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_list_properties(self):
        pipeline = "videotestsrc name=v0 ! identity name=i0 ! xvimagesink name=x0"
        identity_properties = [{'name': 'name'}, {'name': 'parent'}, {'name': 'qos'}, {'name': 'sleep-time'}, {'name': 'error-after'}, {'name': 'drop-probability'}, {'name': 'drop-buffer-flags'}, {'name': 'datarate'}, {'name': 'silent'}, {'name': 'single-segment'}, {'name': 'last-message'}, {'name': 'dump'}, {'name': 'sync'}, {'name': 'check-imperfect-timestamp'}, {'name': 'check-imperfect-offset'}, {'name': 'signal-handoffs'}]
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.list_properties ("p0", "i0"), identity_properties)
if __name__ == '__main__':
    unittest.main()
