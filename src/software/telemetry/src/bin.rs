// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

#[macro_use]
extern crate log;

use std::sync::mpsc::{Receiver, Sender, TryRecvError};

use telemetry::structures::*;
use telemetry::*;

fn main() {
    env_logger::init();

    if let Some(port_id) = std::env::args().nth(1) {
        if !port_id.is_empty() {
            let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
                std::sync::mpsc::channel();
            std::thread::spawn(move || {
                gather_telemetry(&port_id, tx);
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
        } else {
            help();
        }
    } else {
        help();
    }
}

fn help() {
    error!("You need to specify a serial port address as first argument");
}
