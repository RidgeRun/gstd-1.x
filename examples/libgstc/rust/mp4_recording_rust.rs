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
use std::{env, io};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let client = Client::new("127.0.0.1", 5000, -1, true)?;
    let output = env::current_dir().map(|dir| dir.join("mp4_recording.mp4"))?;
    let output = output.to_string_lossy();

    client.pipeline_create(
        "pipe",
        &format!(
            "qtmux name=mux ! filesink location=\"{}\" \
        videotestsrc is-live=true ! avenc_mpeg4 ! mux. \
        audiotestsrc is-live=true ! lamemp3enc ! mux.",
            output
        ),
    )?;
    println!("Pipeline created successfully!");
    println!("Recording to: {}", output);

    client.pipeline_play("pipe")?;
    println!("Pipeline set to playing!");

    println!("Press enter to stop pipeline...");
    let mut line = String::new();
    let _ = io::stdin().read_line(&mut line);

    client.pipeline_inject_eos("pipe")?;
    println!("EOS sent!");

    print!("Waiting for EOS... ");
    match client.pipeline_bus_wait("pipe", "eos", 10_000_000_000) {
        Ok(_bus_message) => {
            println!("received!");
        }
        Err(Status::BusTimeout) => {
            println!("timeout!");
            eprintln!("EOS not received, file may be unreadable");
        }
        Err(status) => {
            println!("error!");
            eprintln!("An error occurred waiting for EOS: {}", status.code());
        }
    }

    client.pipeline_stop("pipe")?;
    println!("Pipeline set to null!");

    client.pipeline_delete("pipe")?;
    println!("Pipeline deleted!");
    println!("Recording finalized at: {}", output);

    Ok(())
}
