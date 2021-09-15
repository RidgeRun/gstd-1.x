#!/usr/bin/env python3
# GStreamer Daemon - gst-launch on steroids
# Python client library abstracting gstd interprocess communication

# Copyright (c) 2015-2020 RidgeRun, LLC (http://www.ridgerun.com)

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following
# disclaimer in the documentation and/or other materials provided
# with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

import unittest

from gstd_runner import GstdTestRunner
from pygstc.gstc import *
from pygstc.logger import *


class TestGstcDeleteMethods(GstdTestRunner):

    async def test_delete_pipeline(self):
        pipeline = 'videotestsrc name=v0 ! fakesink'
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        ret = await self.gstd_client.read('pipelines')
        initial_n_pipes = len(ret['nodes'])
        await self.gstd_client.create('pipelines', 'p0', pipeline)
        ret = await self.gstd_client.read('pipelines')
        final_n_pipes = len(ret['nodes'])
        self.assertEqual(initial_n_pipes + 1, final_n_pipes)
        await self.gstd_client.delete('pipelines', 'p0')
        ret = await self.gstd_client.read('pipelines')
        final_n_pipes = len(ret['nodes'])
        self.assertEqual(initial_n_pipes, final_n_pipes)


if __name__ == '__main__':
    unittest.main()
