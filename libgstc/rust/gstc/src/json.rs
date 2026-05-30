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
use serde_json::Value;

pub(crate) fn json_get_int(json: &str, name: &str) -> Result<i32, Status> {
    let value = parse_json(json)?;
    let number = value.get(name).ok_or(Status::NotFound)?;
    let number = number.as_i64().ok_or(Status::TypeError)?;

    number.try_into().map_err(|_| Status::TypeError)
}

pub(crate) fn json_is_null_field(json: &str, name: &str) -> Result<bool, Status> {
    let value = parse_json(json)?;
    Ok(value.get(name).ok_or(Status::NotFound)?.is_null())
}

pub(crate) fn json_child_string(
    json: &str,
    parent_name: &str,
    data_name: &str,
) -> Result<String, Status> {
    let value = parse_json(json)?;
    let parent = value.get(parent_name).ok_or(Status::NotFound)?;

    parent
        .get(data_name)
        .ok_or(Status::NotFound)?
        .as_str()
        .map(ToOwned::to_owned)
        .ok_or(Status::TypeError)
}

pub(crate) fn json_child_char_array(
    json: &str,
    parent_name: &str,
    array_name: &str,
    element_name: &str,
) -> Result<Vec<String>, Status> {
    let value = parse_json(json)?;
    let parent = value.get(parent_name).ok_or(Status::NotFound)?;
    let array = parent
        .get(array_name)
        .ok_or(Status::TypeError)?
        .as_array()
        .ok_or(Status::TypeError)?;

    array
        .iter()
        .map(|item| {
            item.get(element_name)
                .ok_or(Status::NotFound)?
                .as_str()
                .map(ToOwned::to_owned)
                .ok_or(Status::TypeError)
        })
        .collect()
}

fn parse_json(json: &str) -> Result<Value, Status> {
    serde_json::from_str(json).map_err(|_| Status::Malformed)
}
