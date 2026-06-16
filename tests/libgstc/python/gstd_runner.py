#!/usr/bin/env python3
# GStreamer Daemon - gst-launch on steroids
# Python client library abstracting gstd interprocess communication

# Copyright (c) 2015-2022 RidgeRun, LLC (http://www.ridgerun.com)

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

import asyncio
import pathlib
import socket
import unittest


DEFAULT_TEAR_DOWN_TIMEOUT = 1

class GstdTestRunner(unittest.IsolatedAsyncioTestCase):

    def get_open_port(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(("",0))
        s.listen(1)
        port = s.getsockname()[1]
        s.close()
        return port

    async def asyncSetUp(self):
        self.port = self.get_open_port()
        self.gstd_path = (pathlib.Path(__file__).parent.parent.parent.parent
            .joinpath('gstd').joinpath('gstd').resolve())
        self.gstd = await asyncio.create_subprocess_exec(self.gstd_path, '-p', str(self.port))
        asyncio.get_event_loop().call_later(5, self.gstd.kill)
        connected = False
        while not connected:
            try:
                reader, writer = await asyncio.open_connection(port=self.port)
                writer.close()
                await writer.wait_closed()
                connected = True
            except OSError:
                pass

    async def asyncTearDown(self):
        if self.gstd.returncode is None:
            self.gstd.terminate()
            try:
                await asyncio.wait_for(self.gstd.wait(), timeout=DEFAULT_TEAR_DOWN_TIMEOUT)
            except asyncio.TimeoutError:
                self.gstd.kill()
                await self.gstd.wait()


if __name__ == '__main__':
    unittest.main()
