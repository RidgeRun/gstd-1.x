#!/usr/bin/env python3
import unittest
import gstc

class TestGstcDeleteMethods(unittest.TestCase):

    def test_delete_pipeline(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        ret = self.gstd_client.read("pipelines")
        initial_n_pipes = len(ret['nodes'])
        self.gstd_client.create ("pipelines", "p0", pipeline)
        ret = self.gstd_client.read("pipelines")
        final_n_pipes = len(ret['nodes'])
        self.assertEqual(final_n_pipes, initial_n_pipes+1)
        self.gstd_client.delete ("pipelines", "p0")
        ret = self.gstd_client.read("pipelines")
        final_n_pipes = len(ret['nodes'])
        self.assertEqual(final_n_pipes, initial_n_pipes)

if __name__ == '__main__':
    unittest.main()
