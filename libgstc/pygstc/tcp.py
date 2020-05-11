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

import json
import select
import socket

"""
GSTC - Ipc Class
"""


class Ipc:

    """
    Implementation of IPC that uses TCP sockets to communicate with GSTD

    Methods
    ----------
    send(line, timeout)
        Create a socket and sends a message through it
    """

    def __init__(
        self,
        logger,
        ip,
        port,
        maxsize=None,
        terminator='\x00'.encode('utf-8'),
    ):
        """
        Initialize new Ipc

        Parameters
        ----------
        logger : CustomLogger
            Custom logger where all log messages from this class are going
            to be reported
        ip : string
            The IP where GSTD is running
        port : int
            The port where GSTD is running
        maxsize : int
            Size of the message to read on each iteration
        terminator : string
            Message terminator character
        """

        self._logger = logger
        self._ip = ip
        self._port = port
        self._socket_read_size = 1024
        self._maxsize = maxsize
        self._terminator = terminator

    def send(self, line, timeout=0):
        """
        Create a socket and sends a message through it

        Parameters
        ----------
        line : string
            Message to send through the socket
        timeout : int
            Timeout in seconds to wait for a response. 0: infinite

        Returns
        -------
        data : string
            Decoded JSON string with the response
        """
        data = None
        self._logger.debug('GSTD socket sending line: %s' % line)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self._ip, self._port))
            s.send(' '.join(line).encode('utf-8'))
            data = self._recvall(s, timeout)
            if not data:
                return json.dumps({'error': 'socket error',
                                   'description': 'socket read error in '
                                   + str(' '.join(line)), 'code': -1})
            data = data.decode('utf-8')
            s.close()
        except socket.error:
            s.close()
            self._logger.error('Server did not respond. Is it up?')
            raise ConnectionRefusedError('Server did not respond. Is it up?')\
                from socket.error
        return data

    def _recvall(self, sock, timeout):
        """
        Wait for a response message from the socket

        Parameters
        ----------
        sock : string
            The socket to poll
        timeout : int
            Timeout in seconds to wait for a response. 0: infinite

        Returns
        -------
        buf : string
            Raw socket response
        """

        buf = b''
        count = 0
        try:
            while True:
                if self._maxsize:
                    if count >= self._maxsize:
                        break

                # if timeout is set, perform non-blocking read

                if timeout:
                    sock.setblocking(0)
                    ready = select.select([sock], [], [], timeout)
                    if ready[0]:
                        newbuf = sock.recv(self._socket_read_size)
                    else:
                        buf = None
                        sock.close()
                        break
                else:
                    newbuf = sock.recv(self._socket_read_size)
                    if not newbuf:
                        buf = None
                        break
                if self._terminator in newbuf:

                    # this is the last item

                    buf += newbuf[:newbuf.find(self._terminator)]
                    break
                else:
                    buf += newbuf
                    count += len(newbuf)
        except socket.error:
            buf = None
        return buf
