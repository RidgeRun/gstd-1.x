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
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::thread;
use std::time::Duration;

fn main() -> Result<(), Status> {
    let client = Client::new("127.0.0.1", 5000, -1, true)?;

    client.pipeline_create("pipe", "videotestsrc name=vts ! autovideosink")?;
    println!("Pipeline created successfully!");

    client.pipeline_play("pipe")?;
    println!("Pipeline set to playing!");

    println!("Press enter to stop the pipeline...");
    let stop_flag = Arc::new(AtomicBool::new(false));
    let thread_stop_flag = Arc::clone(&stop_flag);
    thread::spawn(move || {
        let mut line = String::new();
        let _ = io::stdin().read_line(&mut line);
        thread_stop_flag.store(true, Ordering::Relaxed);
    });

    let mut format = 0;
    loop {
        client.element_set("pipe", "vts", "pattern", &format.to_string())?;
        format = (format + 1) % 10;

        if stop_flag.load(Ordering::Relaxed) {
            break;
        }

        thread::sleep(Duration::from_secs(1));
    }

    client.pipeline_stop("pipe")?;
    println!("Pipeline set to null!");

    client.pipeline_delete("pipe")?;
    println!("Pipeline deleted!");

    Ok(())
}
