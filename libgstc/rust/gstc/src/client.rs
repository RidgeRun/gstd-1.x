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

use crate::json::{json_child_char_array, json_child_string, json_get_int, json_is_null_field};
use crate::transport::{ConnectionSettings, Transport};
use crate::Status;
use std::sync::Mutex;
use std::thread;
use std::thread::JoinHandle;

pub struct Client {
    transport: Mutex<Transport>,
}

#[derive(Debug, Clone)]
pub struct BusMessage {
    pub status: Status,
    pub raw_response: String,
}

impl Client {
    pub fn new(
        address: impl Into<String>,
        port: u16,
        wait_time_ms: i32,
        keep_connection_open: bool,
    ) -> Result<Self, Status> {
        let settings = ConnectionSettings {
            address: address.into(),
            port,
            wait_time_ms,
            keep_connection_open,
        };

        let transport = Transport::new(settings)?;

        Ok(Self {
            transport: Mutex::new(transport),
        })
    }

    pub fn ping(&self) -> Result<(), Status> {
        self.cmd_send("read /")
    }

    pub fn debug(&self, threshold: &str, colors: bool, reset: bool) -> Result<(), Status> {
        self.cmd_update("/debug/enable", "true")?;
        self.cmd_update("/debug/threshold", threshold)?;
        self.cmd_update("/debug/color", bool_str(colors))?;
        self.cmd_update("/debug/reset", bool_str(reset))
    }

    pub fn pipeline_create(&self, pipeline_name: &str, pipeline_desc: &str) -> Result<(), Status> {
        self.cmd_create(
            "/pipelines",
            &format!("{} {}", pipeline_name, pipeline_desc),
        )
    }

    pub fn pipeline_create_ref(
        &self,
        pipeline_name: &str,
        pipeline_desc: &str,
    ) -> Result<(), Status> {
        self.cmd_send(&format!(
            "pipeline_create_ref {} {}",
            pipeline_name, pipeline_desc
        ))
    }

    pub fn pipeline_delete(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_delete("/pipelines", pipeline_name)
    }

    pub fn pipeline_delete_ref(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_send(&format!("pipeline_delete_ref {}", pipeline_name))
    }

    pub fn pipeline_play(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_update(&format!("/pipelines/{}/state", pipeline_name), "playing")
    }

    pub fn pipeline_play_ref(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_send(&format!("pipeline_play_ref {}", pipeline_name))
    }

    pub fn pipeline_pause(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_update(&format!("/pipelines/{}/state", pipeline_name), "paused")
    }

    pub fn pipeline_stop(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_update(&format!("/pipelines/{}/state", pipeline_name), "null")
    }

    pub fn pipeline_stop_ref(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_send(&format!("pipeline_stop_ref {}", pipeline_name))
    }

    pub fn pipeline_get_graph(&self, pipeline_name: &str) -> Result<String, Status> {
        self.cmd_read(
            &format!("/pipelines/{}/graph", pipeline_name),
            self.default_wait_time_ms()?,
        )
    }

    pub fn pipeline_get_state(&self, pipeline_name: &str) -> Result<String, Status> {
        let response = self.cmd_read(
            &format!("/pipelines/{}/state", pipeline_name),
            self.default_wait_time_ms()?,
        )?;

        json_child_string(&response, "response", "value")
    }

    pub fn pipeline_verbose(&self, pipeline_name: &str, value: bool) -> Result<(), Status> {
        self.cmd_update(
            &format!("/pipelines/{}/verbose", pipeline_name),
            bool_str(value),
        )
    }

    pub fn element_get(
        &self,
        pipeline_name: &str,
        element: &str,
        property: &str,
    ) -> Result<String, Status> {
        let response = self.cmd_read(
            &format!(
                "/pipelines/{}/elements/{}/properties/{}",
                pipeline_name, element, property
            ),
            self.default_wait_time_ms()?,
        )?;

        json_child_string(&response, "response", "value")
    }

    pub fn element_set(
        &self,
        pipeline_name: &str,
        element: &str,
        property: &str,
        value: &str,
    ) -> Result<(), Status> {
        self.cmd_update(
            &format!(
                "/pipelines/{}/elements/{}/properties/{}",
                pipeline_name, element, property
            ),
            value,
        )
    }

    pub fn element_properties_list(
        &self,
        pipeline_name: &str,
        element: &str,
    ) -> Result<Vec<String>, Status> {
        let response = self.cmd_read(
            &format!(
                "/pipelines/{}/elements/{}/properties",
                pipeline_name, element
            ),
            self.default_wait_time_ms()?,
        )?;

        json_child_char_array(&response, "response", "nodes", "name")
    }

    pub fn pipeline_flush_start(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_create(
            &format!("/pipelines/{}/event", pipeline_name),
            "flush_start",
        )
    }

    pub fn pipeline_flush_stop(&self, pipeline_name: &str, reset: bool) -> Result<(), Status> {
        self.cmd_create(
            &format!("/pipelines/{}/event", pipeline_name),
            &format!("flush_stop {}", bool_str(reset)),
        )
    }

    pub fn pipeline_inject_eos(&self, pipeline_name: &str) -> Result<(), Status> {
        self.cmd_create(&format!("/pipelines/{}/event", pipeline_name), "eos")
    }

    #[allow(clippy::too_many_arguments)]
    pub fn pipeline_seek(
        &self,
        pipeline_name: &str,
        rate: f64,
        format: i32,
        flags: i32,
        start_type: i32,
        start: i64,
        stop_type: i32,
        stop: i64,
    ) -> Result<(), Status> {
        self.cmd_create(
            &format!("/pipelines/{}/event", pipeline_name),
            &format!(
                "seek {:.6} {} {} {} {} {} {}",
                rate, format, flags, start_type, start, stop_type, stop
            ),
        )
    }

    pub fn pipeline_list_elements(&self, pipeline_name: &str) -> Result<Vec<String>, Status> {
        let response = self.cmd_read(
            &format!("/pipelines/{}/elements/", pipeline_name),
            self.default_wait_time_ms()?,
        )?;

        json_child_char_array(&response, "response", "nodes", "name")
    }

    pub fn pipeline_list(&self) -> Result<Vec<String>, Status> {
        let response = self.cmd_read("/pipelines", self.default_wait_time_ms()?)?;
        json_child_char_array(&response, "response", "nodes", "name")
    }

    pub fn pipeline_bus_wait(
        &self,
        pipeline_name: &str,
        message_name: &str,
        timeout_ns: i64,
    ) -> Result<BusMessage, Status> {
        self.cmd_update(
            &format!("/pipelines/{}/bus/types", pipeline_name),
            message_name,
        )?;
        self.cmd_update(
            &format!("/pipelines/{}/bus/timeout", pipeline_name),
            &format!("{}", timeout_ns),
        )?;

        let raw = self.cmd_read(&format!("/pipelines/{}/bus/message", pipeline_name), -1)?;

        match json_is_null_field(&raw, "response") {
            Ok(true) => Err(Status::BusTimeout),
            Ok(false) => Ok(BusMessage {
                status: Status::Ok,
                raw_response: raw,
            }),
            Err(err) => Err(err),
        }
    }

    pub fn pipeline_bus_wait_async<F>(
        &self,
        pipeline_name: String,
        message_name: String,
        timeout_ns: i64,
        callback: F,
    ) -> Result<JoinHandle<Status>, Status>
    where
        F: FnOnce(BusMessage) + Send + 'static,
    {
        let mut settings = self.transport_settings()?;
        settings.keep_connection_open = false;

        let handle = thread::Builder::new()
            .name("gstc-bus-wait".to_string())
            .spawn(move || {
                let client = match Client::new(
                    settings.address,
                    settings.port,
                    settings.wait_time_ms,
                    settings.keep_connection_open,
                ) {
                    Ok(client) => client,
                    Err(err) => return err,
                };

                match client.pipeline_bus_wait(&pipeline_name, &message_name, timeout_ns) {
                    Ok(message) => {
                        let status = message.status;
                        callback(message);
                        status
                    }
                    Err(err) => err,
                }
            })
            .map_err(|_| Status::ThreadError)?;

        Ok(handle)
    }

    pub fn pipeline_emit_action(
        &self,
        pipeline_name: &str,
        element: &str,
        action: &str,
    ) -> Result<(), Status> {
        self.cmd_create(
            &format!(
                "/pipelines/{}/elements/{}/actions/{}",
                pipeline_name, element, action
            ),
            action,
        )
    }

    pub fn pipeline_list_signals(
        &self,
        pipeline_name: &str,
        element: &str,
    ) -> Result<Vec<String>, Status> {
        let response = self.cmd_read(
            &format!("/pipelines/{}/elements/{}/signals", pipeline_name, element),
            self.default_wait_time_ms()?,
        )?;

        json_child_char_array(&response, "response", "nodes", "name")
    }

    pub fn pipeline_signal_connect(
        &self,
        pipeline_name: &str,
        element: &str,
        signal: &str,
        timeout: i32,
    ) -> Result<String, Status> {
        self.cmd_update(
            &format!(
                "/pipelines/{}/elements/{}/signals/{}/timeout",
                pipeline_name, element, signal
            ),
            &format!("{}", timeout),
        )?;

        self.cmd_read(
            &format!(
                "/pipelines/{}/elements/{}/signals/{}/callback",
                pipeline_name, element, signal
            ),
            self.default_wait_time_ms()?,
        )
    }

    pub fn pipeline_signal_disconnect(
        &self,
        pipeline_name: &str,
        element: &str,
        signal: &str,
    ) -> Result<(), Status> {
        self.cmd_read(
            &format!(
                "/pipelines/{}/elements/{}/signals/{}/disconnect",
                pipeline_name, element, signal
            ),
            self.default_wait_time_ms()?,
        )
        .map(|_| ())
    }

    fn cmd_send(&self, request: &str) -> Result<(), Status> {
        self.cmd_send_get_response(request, self.default_wait_time_ms()?)
            .map(|_| ())
    }

    fn cmd_send_get_response(&self, request: &str, timeout_ms: i32) -> Result<String, Status> {
        let response = self.send_request(request, timeout_ms)?;
        let code = json_get_int(&response, "code")?;
        let status = Status::from_code(code);

        if status.is_ok() {
            Ok(response)
        } else {
            Err(status)
        }
    }

    fn cmd_create(&self, where_: &str, what: &str) -> Result<(), Status> {
        self.cmd_send(&format!("create {} {}", where_, what))
    }

    fn cmd_read(&self, what: &str, timeout_ms: i32) -> Result<String, Status> {
        self.cmd_send_get_response(&format!("read {}", what), timeout_ms)
    }

    fn cmd_update(&self, what: &str, how: &str) -> Result<(), Status> {
        self.cmd_send(&format!("update {} {}", what, how))
    }

    fn cmd_delete(&self, where_: &str, what: &str) -> Result<(), Status> {
        self.cmd_send(&format!("delete {} {}", where_, what))
    }

    fn send_request(&self, request: &str, timeout_ms: i32) -> Result<String, Status> {
        let mut guard = self.transport.lock().map_err(|_| Status::SocketError)?;
        guard.send_command(request, timeout_ms)
    }

    fn default_wait_time_ms(&self) -> Result<i32, Status> {
        let guard = self.transport.lock().map_err(|_| Status::SocketError)?;
        Ok(guard.wait_time_ms())
    }

    fn transport_settings(&self) -> Result<ConnectionSettings, Status> {
        let guard = self.transport.lock().map_err(|_| Status::SocketError)?;
        Ok(guard.clone_settings())
    }
}

fn bool_str(value: bool) -> &'static str {
    if value {
        "true"
    } else {
        "false"
    }
}
