#!/usr/bin/env python3
"""
GStreamer Daemon - gst-launch on steroids
Python client library abstracting gstd interprocess communication

Copyright (c) 2015-2019 RidgeRun, LLC (http://www.ridgerun.com)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
"""

import unittest
import gstc
import os
import subprocess

class TestGstcInitMethods(unittest.TestCase):
    def setUp(self):
        subprocess.Popen(["gstd", "-p", "5000", "-n", "2"])

    def test_init(self):
        self.gstd_client = gstc.client()

    def test_init_loglevel(self):
        self.gstd_client = gstc.client(loglevel='DEBUG')

    def test_init_logfile(self):
        f = open("dummy.log","w+")
        f.close()
        f = open('dummy.log')
        num_lines_init = sum(1 for line in f)
        f.close()
        self.gstd_client = gstc.client(logfile='dummy.log', loglevel='DEBUG')
        f = open('dummy.log')
        num_lines_final = sum(1 for line in f)
        f.close()
        self.assertNotEqual(num_lines_init, num_lines_final)
        os.remove("dummy.log")

if __name__ == '__main__':
    unittest.main()
