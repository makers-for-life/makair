#!/usr/bin/env sh
export RUST_LOG="info,telemetry=debug"

cargo run --features="build-binary"
