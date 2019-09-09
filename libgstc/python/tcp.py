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

import socket

class ipc(object):
    def __init__(self, logger, ip, port, maxsize=8192, terminator='\x00'.encode('utf-8')):
        self.logger = logger
        self.ip = ip
        self.port = port
        self.maxsize = maxsize
        self.terminator = terminator

    def send(self, line):
        self.logger.debug('GSTD socket sending line: %s', line)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.ip, self.port))
            s.send(' '.join(line).encode('utf-8'))
            data = self.recvall(s)
            data = data.decode('utf-8')
            s.close()
        except socket.error:
            s.close()
            self.logger.error('GSTD socket error')
            data = None
        self.logger.debug('GSTD socket received answer:\n %s', data)
        return data

    def recvall(self, sock):
        buf = b''
        count = self.maxsize
        try:
            while count:
                newbuf = sock.recv(self.maxsize//8)
                if not newbuf: return None
                if self.terminator in newbuf:
                    # this is the last item
                    buf += newbuf[:newbuf.find(self.terminator)]
                    break
                else:
                    buf += newbuf
                    count -= len(newbuf)
        except socket.error:
            buf = json.dumps({"error":"socket error", "msg": traceback.format_exc(), "code": -1 })
        return buf
