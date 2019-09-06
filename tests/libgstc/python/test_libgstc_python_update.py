#!/usr/bin/env python3
import unittest
import gstc

class TestGstcUpdateMethods(unittest.TestCase):

    def test_libgstc_python_update(self):
        pipeline = "videotestsrc name=v0 pattern=snow ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        self.assertEqual(self.gstd_client.pipeline_create ("p0", pipeline), 0)
        self.assertEqual(self.gstd_client.update("pipelines/p0/elements/v0/properties/pattern", "ball"), 0)
        ret = self.gstd_client.read("pipelines/p0/elements/v0/properties/pattern")
        self.assertEqual(ret['code'], 0)
        self.assertEqual(ret['response']['value'], "Moving ball")
        self.assertEqual(self.gstd_client.pipeline_stop ("p0"), 0)
        self.assertEqual(self.gstd_client.pipeline_delete ("p0"), 0)

if __name__ == '__main__':
    unittest.main()

