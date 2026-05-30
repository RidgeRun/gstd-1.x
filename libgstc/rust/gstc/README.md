# gstc (Rust)

Rust client library for RidgeRun GstD.

## Build and run

### Cargo

A `Cargo.toml` is maintained for crate workflows, for example, (`cargo run`, `cargo test`, `cargo fmt`, `cargo clippy`). The crate can be added as a dependency in another Cargo project:

```bash
[dependencies]
gstc = { path = "<path_to_this_repo>" }
```

To build the library (.rlib), run:
```bash
cargo build
```
The library will be located at:
 ```bash
target/debug/libgstc.rlib
```

To build the examples, refer to the following file:
 ```bash
examples/libgstc/rust/README.md
```

### Meson (project-integrated build)

This approach was tested with the recently released Workspaces support for
Meson version 1.11.0. The following example was tried:
```bash
rust = import('rust')
cargo_ws = rust.workspace()
cargo_dep = ws.subproject('serde_json').dependency()
```

However, Meson ran into an issue building the dependency as a subproject:
```bash
error: environment variable `OUT_DIR` not defined at compile time
   --> ../subprojects/serde_core-1.0.228/src/crate_root.rs:165:26
    |
165 |         include!(concat!(env!("OUT_DIR"), "/private.rs"));
    |                          ^^^^^^^^^^^^^^^
    |
   ::: ../subprojects/serde_core-1.0.228/src/lib.rs:111:1
    |
111 | crate_root!();
    | ------------- in this macro invocation
    |
    = help: Cargo sets build script variables at run time. Use `std::env::var("OUT_DIR")` instead
    = note: this error originates in the macro `env` which comes from the expansion of the macro `crate_root` (in Nightly builds, run with -Z macro-backtrace for more info)

error: aborting due to 1 previous error
```
