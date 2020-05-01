// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

#[macro_use]
extern crate clap;
#[macro_use]
extern crate log;

use clap::Clap;
use std::fs::File;
use std::fs::OpenOptions;
use std::io::BufWriter;
use std::sync::mpsc::{Receiver, Sender, TryRecvError};

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

    /// Reads telemetry from a serial port and save bytes to a file
    #[clap(version = crate_version!(), author = crate_authors!())]
    Record(Record),

    /// Reads telemetry from a recorded file, parses it and streams result to stdout
    #[clap(version = crate_version!(), author = crate_authors!())]
    Play(Play),
}

#[derive(Clap)]
struct Debug {
    /// Address of the port to use
    #[clap(short = "p")]
    port: String,
}

#[derive(Clap)]
struct Record {
    /// Address of the port to use
    #[clap(short = "p")]
    port: String,

    /// Path of the file to write to
    #[clap(short = "o")]
    output: String,
}

#[derive(Clap)]
struct Play {
    /// Path of the recorded file
    #[clap(short = "i")]
    input: String,
}

fn main() {
    env_logger::init();
    let opts: Opts = Opts::parse();

    match opts.mode {
        Mode::Debug(cfg) => debug(cfg),
        Mode::Record(cfg) => record(cfg),
        Mode::Play(cfg) => play(cfg),
    }
}

fn debug(cfg: Debug) {
    let (tx, rx): (Sender<TelemetryChannelType>, Receiver<TelemetryChannelType>) =
        std::sync::mpsc::channel();
    std::thread::spawn(move || {
        gather_telemetry(&cfg.port, tx, None);
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
                panic!("channel to serial port thread was closed");
            }
        }
    }
}

fn record(cfg: Record) {
    let file = OpenOptions::new()
        .write(true)
        .create_new(true)
        .open(&cfg.output)
        .expect("failed to create recording file");
    let file_buffer = BufWriter::new(file);

    let (tx, rx): (Sender<TelemetryChannelType>, Receiver<TelemetryChannelType>) =
        std::sync::mpsc::channel();
    std::thread::spawn(move || {
        gather_telemetry(&cfg.port, tx, Some(file_buffer));
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
                panic!("channel to serial port thread was closed");
            }
        }
    }
}

fn play(cfg: Play) {
    let file = File::open(cfg.input).expect("failed to play recorded file");
    let (tx, rx): (Sender<TelemetryChannelType>, Receiver<TelemetryChannelType>) =
        std::sync::mpsc::channel();
    std::thread::spawn(move || {
        gather_telemetry_from_file(file, tx);
    });
    loop {
        match rx.try_recv() {
            Ok(msg) => {
                display_message(msg);
            }
            Err(TryRecvError::Empty) => {
                std::thread::sleep(std::time::Duration::from_millis(1));
            }
            Err(TryRecvError::Disconnected) => {
                warn!("end of recording");
                std::process::exit(0);
            }
        }
    }
}
