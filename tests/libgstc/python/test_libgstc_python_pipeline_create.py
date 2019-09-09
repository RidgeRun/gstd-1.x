#!/usr/bin/env python3
import unittest
import gstc

class TestGstcPipelineCreateMethods(unittest.TestCase):

    def test_libgstc_python_pipeline_create(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        ret = self.gstd_client.read("pipelines")
        self.assertEqual(ret['nodes'][0]['name'], "p0")
        self.gstd_client.pipeline_delete ("p0")

if __name__ == '__main__':
    unittest.main()

