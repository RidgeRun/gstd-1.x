#!/usr/bin/env python3
import unittest
import gstc

class TestGstcPipelineDeleteMethods(unittest.TestCase):

    def test_libgstc_python_pipeline_delete(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.gstd_client.pipeline_create ("p0", pipeline)
        ret_prev = self.gstd_client.read("pipelines")
        len_prev = len(ret_prev['nodes'])
        self.gstd_client.pipeline_delete ("p0")
        ret_post = self.gstd_client.read("pipelines")
        len_post = len(ret_post['nodes'])
        self.assertTrue(len_prev > len_post)

if __name__ == '__main__':
    unittest.main()
