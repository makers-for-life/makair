#[macro_use]
extern crate nom;

mod parsers;
mod telemetry;

use serial::prelude::*;
use std::io::Read;

use parsers::*;
use telemetry::*;

fn main() {
    let port_id = "COM30";

    loop {
        println!("Opening {}", &port_id);
        match serial::open(&port_id) {
            Err(e) => {
                eprintln!("{}", e);
                std::thread::sleep(std::time::Duration::from_secs(1));
            }
            Ok(mut port) => {
                match port.reconfigure(&|settings| settings.set_baud_rate(serial::Baud115200)) {
                    Err(e) => {
                        eprintln!("{}", e);
                        std::thread::sleep(std::time::Duration::from_secs(1));
                    }
                    Ok(_) => {
                        let mut buffer = Vec::new();
                        for b in port.bytes() {
                            match b {
                                // We got a new byte
                                Ok(byte) => {
                                    // We add it to the buffer
                                    buffer.push(byte);

                                    // Let's try to parse the buffer
                                    match parse_telemetry_message(&buffer) {
                                        // It worked! Let's extract the message and replace the buffer with the rest of the bytes
                                        Ok((rest, message)) => {
                                            match message {
                                                TelemetryMessage::DataSnapshot { .. } => {
                                                    println!("    {:?}", &message)
                                                }
                                                TelemetryMessage::MachineStateSnapshot {
                                                    ..
                                                } => {
                                                    println!("------------------------------------------------------------------------------------");
                                                    println!("{:?}", &message);
                                                    println!("------------------------------------------------------------------------------------");
                                                }
                                            }
                                            buffer = Vec::from(rest);
                                        }
                                        // There are not enough bytes, let's wait until we get more
                                        Err(nom::Err::Incomplete(_)) => {
                                            // Do nothing
                                        }
                                        // We can't do anything with the begining of the buffer, let's drop its first byte
                                        Err(e) => {
                                            eprintln!("{:?}", &e);
                                            if !buffer.is_empty() {
                                                buffer.remove(0);
                                            }
                                        }
                                    }
                                }
                                // We failed to get a new byte from serial
                                Err(e) => {
                                    if e.kind() == std::io::ErrorKind::TimedOut { // It's OK it's just a timeout; let's try again
                                         // Do nothing
                                    } else {
                                        // It's another error, let's print it and wait a bit before retrying the whole process
                                        eprintln!("{:?}", &e);
                                        std::thread::sleep(std::time::Duration::from_secs(1));
                                        break;
                                    }
                                }
                            };
                        }
                    }
                }
            }
        }
    }
}
