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


class TestGstcDebugThresholdMethods(GstdTestRunner):

    async def test_debug_threshold_none(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('0')

    async def test_debug_threshold_error(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('1')

    async def test_debug_threshold_warning(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('2')

    async def test_debug_threshold_fixme(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('3')

    async def test_debug_threshold_info(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('4')

    async def test_debug_threshold_debug(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('5')

    async def test_debug_threshold_log(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('6')

    async def test_debug_threshold_trace(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('7')

    async def test_debug_threshold_memdump(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('8')

    async def test_debug_threshold_invalid(self):
        self.gstd_logger = CustomLogger('test_libgstc', loglevel='DEBUG')
        self.gstd_client = GstdClient(port=self.port, logger=self.gstd_logger)
        await self.gstd_client.debug_threshold('9')


if __name__ == '__main__':
    unittest.main()
