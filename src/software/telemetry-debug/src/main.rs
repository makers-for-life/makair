#[macro_use]
extern crate nom;

mod parsers;
mod telemetry;

use serial::prelude::*;
use std::borrow::BorrowMut;
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
                    Ok(_) => read_telemetry(port),
                }
            }
        }
    }
}

fn read_telemetry(mut port: impl serial::SerialPort) {
    loop {
        let mut header_buffer = [0; 3];
        match port.read_exact(&mut header_buffer) {
            Ok(_) => match parse_header(&header_buffer) {
                Ok((header_rest, h)) => {
                    println!("{:?}", &h);
                    let mut buffer = Vec::new();
                    for byte in port.borrow_mut().bytes() {
                        match byte {
                            Ok(b) => {
                                buffer.push(b);
                            }
                            Err(_) => break,
                        }
                    }
                    println!("Buffer: {:?}", &buffer);
                    match h {
                        Header {
                            version: 1,
                            message_type: MessageType::DataSnapshot,
                        } => {
                            match parse_data_snapshot(buffer.as_ref()) {
                                Ok((rest, message)) => {
                                    println!("{:?}", &message);
                                    println!("{:?}", &rest);
                                }
                                Err(e) => {
                                    eprintln!("Parsing error: {:?}", &e);
                                    return;
                                }
                            };
                        }
                        Header {
                            version: 1,
                            message_type: MessageType::MachineStateSnapshot,
                        } => {
                            match parse_machine_state_snapshot(buffer.as_ref()) {
                                Ok((rest, message)) => {
                                    println!("{:?}", &message);
                                    println!("{:?}", &rest);
                                }
                                Err(e) => {
                                    eprintln!("Parsing error: {:?}", &e);
                                    return;
                                }
                            };
                        }
                        _ => {
                            eprintln!("Unsupported version or message type");
                            eprintln!("{:?}", &header_buffer);
                            eprintln!("{:?}", &header_rest);
                            return;
                        }
                    };
                }
                Err(e) => {
                    eprintln!("Parsing error: {:?}", &e);
                    return;
                }
            },
            Err(e) => {
                if e.kind() == std::io::ErrorKind::TimedOut {
                    // Do nothing
                } else {
                    eprintln!("{:?}", &e);
                    return;
                }
            }
        }
    }
}
