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

fn main() -> Result<(), Status> {
    let client = Client::new("127.0.0.1", 5000, 5000, false)?;

    let pipeline_name = "video";
    let pipeline_desc = "videotestsrc num-buffers=300 ! videoconvert ! autovideosink";

    client.pipeline_create(pipeline_name, pipeline_desc)?;
    client.pipeline_play(pipeline_name)?;

    let bus_message = client.pipeline_bus_wait(pipeline_name, "eos", -1)?;
    if bus_message.status != Status::Ok {
        let _ = client.pipeline_stop(pipeline_name);
        let _ = client.pipeline_delete(pipeline_name);
        return Err(bus_message.status);
    }

    println!("received EOS: {}", bus_message.raw_response);

    client.pipeline_stop(pipeline_name)?;
    client.pipeline_delete(pipeline_name)?;

    Ok(())
}
