#!/usr/bin/env python3
import unittest
import gstc

class TestGstcCreateMethods(unittest.TestCase):

    def test_create_pipeline(self):
        pipeline = "videotestsrc name=v0 ! fakesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        ret = self.gstd_client.read("pipelines")
        initial_n_pipes = len(ret['response']['nodes'])
        self.assertEqual(self.gstd_client.create ("pipelines", "p0", pipeline), 0)
        ret = self.gstd_client.read("pipelines")
        final_n_pipes = len(ret['response']['nodes'])
        self.assertEqual(final_n_pipes, initial_n_pipes+1)
if __name__ == '__main__':
    unittest.main()
