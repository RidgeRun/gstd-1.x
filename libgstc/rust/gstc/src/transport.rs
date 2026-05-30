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

use crate::Status;
use std::io::{Read, Write};
use std::net::{Shutdown, TcpStream};
use std::time::Duration;

const MAX_RESPONSE_LENGTH: usize = 10 * 1024 * 1024;

#[derive(Clone, Debug)]
pub(crate) struct ConnectionSettings {
    pub(crate) address: String,
    pub(crate) port: u16,
    pub(crate) wait_time_ms: i32,
    pub(crate) keep_connection_open: bool,
}

pub(crate) struct Transport {
    settings: ConnectionSettings,
    stream: Option<TcpStream>,
}

impl Transport {
    pub(crate) fn new(settings: ConnectionSettings) -> Result<Self, Status> {
        let mut transport = Self {
            settings,
            stream: None,
        };

        if transport.settings.keep_connection_open {
            let stream = transport.open_socket()?;
            transport.stream = Some(stream);
        }

        Ok(transport)
    }

    pub(crate) fn wait_time_ms(&self) -> i32 {
        self.settings.wait_time_ms
    }

    pub(crate) fn clone_settings(&self) -> ConnectionSettings {
        self.settings.clone()
    }

    fn open_socket(&self) -> Result<TcpStream, Status> {
        TcpStream::connect((self.settings.address.as_str(), self.settings.port))
            .map_err(|_| Status::Unreachable)
    }

    pub(crate) fn send_command(
        &mut self,
        request: &str,
        timeout_ms: i32,
    ) -> Result<String, Status> {
        if request.is_empty() {
            return Err(Status::NullArgument);
        }

        if self.settings.keep_connection_open {
            if let Some(stream) = self.stream.as_mut() {
                Self::write_then_read(stream, request, timeout_ms)
            } else {
                Err(Status::SocketError)
            }
        } else {
            let mut stream = self.open_socket()?;
            let ret = Self::write_then_read(&mut stream, request, timeout_ms);
            let _ = stream.shutdown(Shutdown::Both);
            ret
        }
    }

    fn write_then_read(
        stream: &mut TcpStream,
        request: &str,
        timeout_ms: i32,
    ) -> Result<String, Status> {
        let timeout = if timeout_ms < 0 {
            None
        } else {
            Some(Duration::from_millis(timeout_ms as u64))
        };

        stream
            .set_read_timeout(timeout)
            .map_err(|_| Status::SocketError)?;

        stream
            .write_all(request.as_bytes())
            .map_err(|_| Status::SendError)?;

        let mut response = Vec::<u8>::new();
        let mut buffer = [0_u8; 1024];

        loop {
            let n = match stream.read(&mut buffer) {
                Ok(0) => return Err(Status::RecvError),
                Ok(n) => n,
                Err(err) if err.kind() == std::io::ErrorKind::TimedOut => {
                    return Err(Status::SocketTimeout)
                }
                Err(err) if err.kind() == std::io::ErrorKind::WouldBlock => {
                    return Err(Status::SocketTimeout)
                }
                Err(_) => return Err(Status::RecvError),
            };

            if let Some(zero_idx) = buffer[..n].iter().position(|b| *b == 0) {
                response.extend_from_slice(&buffer[..zero_idx]);
                break;
            }

            response.extend_from_slice(&buffer[..n]);
            if response.len() >= MAX_RESPONSE_LENGTH {
                return Err(Status::LongResponse);
            }
        }

        if response.len() >= MAX_RESPONSE_LENGTH {
            return Err(Status::LongResponse);
        }

        String::from_utf8(response).map_err(|_| Status::Malformed)
    }
}
