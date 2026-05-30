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
use std::io;

fn main() -> Result<(), Status> {
    let client = Client::new("127.0.0.1", 5000, -1, true)?;

    client.ping()?;
    println!("GStreamer daemon is alive!");

    client.pipeline_create("pipe", "videotestsrc ! autovideosink")?;
    println!("Pipeline created successfully!");

    client.pipeline_play("pipe")?;
    println!("Pipeline set to playing!");

    println!("Press enter to stop pipeline...");
    let mut line = String::new();
    let _ = io::stdin().read_line(&mut line);

    client.pipeline_stop("pipe")?;
    println!("Pipeline set to null!");

    client.pipeline_delete("pipe")?;
    println!("Pipeline deleted!");

    Ok(())
}
