// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

#[macro_use]
extern crate clap;

use clap::Clap;
use std::sync::mpsc::{Receiver, Sender, TryRecvError};

use telemetry::structures::*;
use telemetry::*;

#[derive(Clap)]
#[clap(version = crate_version!(), author = crate_authors!())]
struct Opts {
    #[clap(subcommand)]
    mode: Mode,
}

#[derive(Clap)]
enum Mode {
    /// Reads telemetry from a serial port, parses it and streams result to stdout
    #[clap(version = crate_version!(), author = crate_authors!())]
    Debug(Debug),
}

#[derive(Clap)]
struct Debug {
    /// Address of the port to use
    #[clap(short = "p")]
    port: String,
}

fn main() {
    env_logger::init();
    let opts: Opts = Opts::parse();

    match opts.mode {
        Mode::Debug(cfg) => {
            let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
                std::sync::mpsc::channel();
            std::thread::spawn(move || {
                gather_telemetry(&cfg.port, tx);
            });
            loop {
                match rx.try_recv() {
                    Ok(msg) => {
                        display_message(msg);
                    }
                    Err(TryRecvError::Empty) => {
                        std::thread::sleep(std::time::Duration::from_millis(10));
                    }
                    Err(TryRecvError::Disconnected) => {
                        panic!("Channel to serial port thread was closed");
                    }
                }
            }
        }
    }
}
