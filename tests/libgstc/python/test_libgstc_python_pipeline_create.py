#!/usr/bin/env python3
import unittest
import gstc

class TestGstcPipelineCreateMethods(unittest.TestCase):

    def test_libgstc_python_pipeline_create(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(gstd_client.pipeline_create ("p0", pipeline), 0)
        ret = gstd_client.read("pipelines")
        self.assertEqual(ret['code'], 0)
        self.assertEqual(ret['response']['nodes'][0]['name'], "p0")

if __name__ == '__main__':
    unittest.main()

