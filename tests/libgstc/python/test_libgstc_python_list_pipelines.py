#!/usr/bin/env python3
import unittest
import gstc

class TestGstcListPipelinesMethods(unittest.TestCase):

    def test_list_pipelines(self):
        pipeline = "videotestsrc name=v0 ! xvimagesink"
        self.gstd_client = gstc.client(loglevel='DEBUG')
        initial_n_pipes = len(self.gstd_client.list_pipelines())
        self.assertEqual(self.gstd_client.create ("pipelines", "p0", pipeline), 0)
        final_n_pipes = len(self.gstd_client.list_pipelines())
        self.assertEqual(final_n_pipes, initial_n_pipes+1)
if __name__ == '__main__':
    unittest.main()
