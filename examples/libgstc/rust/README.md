# Rust `libgstc` Examples

These examples show how to control GStreamer Daemon (`gstd`) using the Rust `gstc` client library.

## Prerequisites

- `gstd` must be running and listening on `127.0.0.1:5000`
- Rust examples assume the daemon is reachable at that address and port
- Some examples require GStreamer plugins such as `autovideosink`, `playbin`, `qtmux`, `avenc_mpeg4`, and `lamemp3enc`

## How to Build
To build the examples, run the following command from the repository root:
```bash
cargo build --examples
```
The examples will be available at the following path:
 ```bash
target/debug/examples/
```

## How To Run

You can run the examples directly as an executable or
with Cargo from the Repository root:

```bash
cargo run --example simple_pipeline
```

Examples that take an argument can be run like this:

```bash
cargo run --example gapless_playback -- /path/to/video.mp4
```

## Examples

### `simple_pipeline`

Creates a `videotestsrc ! autovideosink` pipeline, starts playback, waits for Enter, then stops and deletes the pipeline.

### `pipeline_lifecycle`

Creates a `videotestsrc ! fakesink` pipeline, sets it to `PLAYING`, polls until the daemon reports the expected state, then stops and deletes the pipeline.

### `wait_on_bus`

Creates a finite pipeline with `videotestsrc num-buffers=300`, waits for an EOS message on the bus, prints the raw bus message, then cleans up the pipeline.

### `dynamic_property_change`

Creates a `videotestsrc` pipeline and changes the `pattern` property once per second while the pipeline is running. Press Enter to stop it.

### `gapless_playback`

Plays a media file with `playbin`, waits for EOS, then seeks back to the start to continue playback. Press Enter to stop it.

Run with:

```bash
cargo run --example gapless_playback -- /path/to/video.mp4
```

### `mp4_recording`

Creates a live audio/video recording pipeline that writes to `mp4_recording.mp4`. When you press Enter, it injects EOS, waits for the EOS bus message, then stops and deletes the pipeline.

