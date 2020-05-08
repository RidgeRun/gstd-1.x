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


"""
GSTC - GstClientError Class
"""


class GstClientError(Exception):

    def __init__(self, description, code):
        super(GstClientError, self).__init__(description)
        self.description = description
        self.code = code


"""
GSTC - GstdError Class
"""


class GstdError(GstClientError):
    """Raised when Gstd IPC fails

    Attributes:
        description -- GstcError description message
    """

    def __init__(self, description, code):
        super(GstdError, self).__init__(description, code)
        self.description = description
        self.code = code


"""
GSTC - GstcError Class
"""


class GstcError(GstClientError):
    """Raised when the Gstd python client fails internally

    Attributes:
        description -- GstcError description message
    """

    def __init__(self, description, code):
        super(GstcError, self).__init__(description, code)
        self.description = description
        self.code = code
