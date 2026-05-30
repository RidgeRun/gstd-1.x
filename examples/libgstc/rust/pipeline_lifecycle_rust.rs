/*
 * This file is part of GStreamer Daemon
 * Copyright 2015-2026 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

use gstc::{Client, Status};
use std::thread;
use std::time::{Duration, Instant};

fn wait_for_state(
    client: &Client,
    pipeline_name: &str,
    expected: &str,
    timeout: Duration,
) -> Result<String, Status> {
    let start = Instant::now();
    loop {
        let state = client.pipeline_get_state(pipeline_name)?;
        if state == expected {
            return Ok(state);
        }

        if start.elapsed() >= timeout {
            return Err(Status::Timeout);
        }

        thread::sleep(Duration::from_millis(100));
    }
}

fn main() -> Result<(), Status> {
    let client = Client::new("127.0.0.1", 5000, 5000, false)?;

    client.pipeline_create("pipe", "videotestsrc is-live=true ! fakesink")?;
    client.pipeline_play("pipe")?;

    let state = wait_for_state(&client, "pipe", "PLAYING", Duration::from_secs(5))?;
    println!("pipeline state: {}", state);

    client.pipeline_stop("pipe")?;
    client.pipeline_delete("pipe")?;

    Ok(())
}
