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

import asyncio
import socket
from contextlib import asynccontextmanager

"""
GstClient - Ipc Class
"""


class Ipc:

    """
    Implementation of IPC that uses TCP sockets to communicate with Gstd

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
        terminator=b'\x00',
    ):
        """
        Initialize new Ipc

        Parameters
        ----------
        logger : CustomLogger
            Custom logger where all log messages from this class are going
            to be reported
        ip : string
            The IP where Gstd is running
        port : int
            The port where Gstd is running
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

    @asynccontextmanager
    async def gstd_conn(self):
        kwargs = {
            'host': self._ip,
            'port': self._port
        }
        if self._maxsize is not None:
            kwargs['limit'] = self._maxsize
        reader, writer = await asyncio.open_connection(**kwargs)
        try:
            yield reader, writer
        finally:
            if not writer.is_closing():
                writer.close()
            await writer.wait_closed()

    async def send(self, line, timeout=None):
        """
        Create a socket and sends a message through it

        Parameters
        ----------
        line : string
            Message to send through the socket
        timeout : float
            Timeout in seconds to wait for a response. 0: non-blocking, None: blocking (default)

        Raises
        -------
        BufferError: When the incoming buffer is too big
        TimeoutError : When the server takes too long to respond
        ConnectionRefusedError : When the socket fails to communicate

        Returns
        -------
        data : string
            Decoded JSON string with the response
        """
        self._logger.debug('GSTD socket sending line: {}'.format(line))
        try:
            async with self.gstd_conn() as (reader, writer):
                writer.write(' '.join(line).encode('utf-8'))
                await writer.drain()
                fut = reader.readuntil(separator=self._terminator)
                data = await asyncio.wait_for(fut, timeout=timeout)
                if not data:
                    raise socket.error("Socket read error happened")
                data = data[:-1].decode('utf-8')
                return data
        except BufferError as e:
            error_msg = 'Server response too long'
            self._logger.error(error_msg)
            raise BufferError(error_msg)\
                from e
        except TimeoutError as e:
            error_msg = 'Server took too long to respond'
            self._logger.error(error_msg)
            raise TimeoutError(error_msg)\
                from e
        except socket.error as e:
            error_msg = 'Server did not respond. Is it up?'
            self._logger.error(error_msg)
            raise ConnectionRefusedError(error_msg)\
                from e
