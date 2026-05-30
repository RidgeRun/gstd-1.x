/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2026 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

use std::fmt;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Status {
    Ok,
    NullArgument,
    Unreachable,
    Timeout,
    Oom,
    TypeError,
    Malformed,
    NotFound,
    SendError,
    RecvError,
    SocketError,
    ThreadError,
    BusTimeout,
    SocketTimeout,
    LongResponse,
    Unknown(i32),
}

impl Status {
    pub fn from_code(code: i32) -> Status {
        match code {
            0 => Status::Ok,
            -1 => Status::NullArgument,
            -2 => Status::Unreachable,
            -3 => Status::Timeout,
            -4 => Status::Oom,
            -5 => Status::TypeError,
            -6 => Status::Malformed,
            -7 => Status::NotFound,
            -8 => Status::SendError,
            -9 => Status::RecvError,
            -10 => Status::SocketError,
            -11 => Status::ThreadError,
            -12 => Status::BusTimeout,
            -13 => Status::SocketTimeout,
            -14 => Status::LongResponse,
            other => Status::Unknown(other),
        }
    }

    pub fn code(self) -> i32 {
        match self {
            Status::Ok => 0,
            Status::NullArgument => -1,
            Status::Unreachable => -2,
            Status::Timeout => -3,
            Status::Oom => -4,
            Status::TypeError => -5,
            Status::Malformed => -6,
            Status::NotFound => -7,
            Status::SendError => -8,
            Status::RecvError => -9,
            Status::SocketError => -10,
            Status::ThreadError => -11,
            Status::BusTimeout => -12,
            Status::SocketTimeout => -13,
            Status::LongResponse => -14,
            Status::Unknown(code) => code,
        }
    }

    pub fn is_ok(self) -> bool {
        matches!(self, Status::Ok)
    }
}

impl fmt::Display for Status {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let name = match *self {
            Status::Ok => "GSTC_OK",
            Status::NullArgument => "GSTC_NULL_ARGUMENT",
            Status::Unreachable => "GSTC_UNREACHABLE",
            Status::Timeout => "GSTC_TIMEOUT",
            Status::Oom => "GSTC_OOM",
            Status::TypeError => "GSTC_TYPE_ERROR",
            Status::Malformed => "GSTC_MALFORMED",
            Status::NotFound => "GSTC_NOT_FOUND",
            Status::SendError => "GSTC_SEND_ERROR",
            Status::RecvError => "GSTC_RECV_ERROR",
            Status::SocketError => "GSTC_SOCKET_ERROR",
            Status::ThreadError => "GSTC_THREAD_ERROR",
            Status::BusTimeout => "GSTC_BUS_TIMEOUT",
            Status::SocketTimeout => "GSTC_SOCKET_TIMEOUT",
            Status::LongResponse => "GSTC_LONG_RESPONSE",
            Status::Unknown(_) => "GSTC_UNKNOWN",
        };

        write!(f, "{} ({})", name, self.code())
    }
}

impl std::error::Error for Status {}
