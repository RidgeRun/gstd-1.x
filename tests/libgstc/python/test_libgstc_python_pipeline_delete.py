#!/usr/bin/env python3
import unittest
import gstc

class TestGstcPipelineDeleteMethods(unittest.TestCase):

    def test_libgstc_python_pipeline_delete(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(gstd_client.pipeline_create ("p0", pipeline), 0)
        ret_prev = gstd_client.read("pipelines")
        self.assertEqual(ret_prev['code'], 0)
        len_prev = len(ret_prev['response']['nodes'])
        self.assertEqual(gstd_client.pipeline_delete ("p0"), 0)
        ret_post = gstd_client.read("pipelines")
        self.assertEqual(ret_post['code'], 0)
        len_post = len(ret_post['response']['nodes'])
        self.assertTrue(len_prev > len_post)

if __name__ == '__main__':
    unittest.main()
