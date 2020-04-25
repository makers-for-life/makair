// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

#[macro_use]
extern crate log;
#[macro_use]
extern crate nom;

mod parsers;
pub mod structures;

pub use serial;
use serial::prelude::*;
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use std::io::BufWriter;
use std::io::Read;
use std::io::Write;
use std::sync::mpsc::Sender;

use parsers::*;
use structures::*;

pub type TelemetryChannelType = Result<TelemetryMessage, serial::core::Error>;

pub fn gather_telemetry(
    port_id: &str,
    tx: Sender<TelemetryChannelType>,
    mut file_buf: Option<BufWriter<File>>,
) {
    loop {
        info!("Opening {}", &port_id);
        match serial::open(&port_id) {
            Err(e) => {
                error!("{:?}", e);
                tx.send(Err(e)).unwrap();
                std::thread::sleep(std::time::Duration::from_secs(1));
            }
            Ok(mut port) => {
                match port.reconfigure(&|settings| {
                    settings.set_char_size(serial::Bits8);
                    settings.set_parity(serial::ParityNone);
                    settings.set_stop_bits(serial::Stop1);
                    settings.set_flow_control(serial::FlowNone);
                    settings.set_baud_rate(serial::Baud115200)
                }) {
                    Err(e) => {
                        error!("{}", e);
                        tx.send(Err(e)).unwrap();
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
                                            if let Some(file_buffer) = file_buf.as_mut() {
                                                // Write a new line with the base64 value of the message
                                                let base64 = base64::encode(&buffer);
                                                file_buffer.write_all(base64.as_bytes()).unwrap();
                                                file_buffer.write_all(b"\n").unwrap();
                                            }

                                            tx.send(Ok(message)).unwrap();
                                            buffer = Vec::from(rest);
                                        }
                                        // There are not enough bytes, let's wait until we get more
                                        Err(nom::Err::Incomplete(_)) => {
                                            // Do nothing
                                            if let Some(file_buffer) = file_buf.as_mut() {
                                                file_buffer.flush().unwrap();
                                            }
                                        }
                                        // We can't do anything with the begining of the buffer, let's drop its first byte
                                        Err(e) => {
                                            debug!("{:?}", &e);
                                            if !buffer.is_empty() {
                                                if let Some(file_buffer) = file_buf.as_mut() {
                                                    file_buffer.flush().unwrap();
                                                }

                                                buffer.remove(0);
                                            }
                                        }
                                    }
                                }
                                // We failed to get a new byte from serial
                                Err(e) => {
                                    if let Some(file_buffer) = file_buf.as_mut() {
                                        file_buffer.flush().unwrap();
                                    }
                                    if e.kind() == std::io::ErrorKind::TimedOut { // It's OK it's just a timeout; let's try again
                                         // Do nothing
                                    } else {
                                        // It's another error, let's print it and wait a bit before retrying the whole process
                                        error!("{:?}", &e);
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

pub fn display_message(message: TelemetryChannelType) {
    match message {
        Ok(TelemetryMessage::BootMessage(BootMessage { value128, .. })) => {
            debug!("####################################################################################");
            debug!("######### CONTROLLER STARTED #########");
            debug!("####################################################################################");
            info!("{:?}", &message.unwrap());
            debug!("####################################################################################");
            if value128 != 128u8 {
                error!("value128 should be equal to 128 (found {:b} = {}); check serial port configuration", &value128, &value128);
            }
        }
        Ok(TelemetryMessage::StoppedMessage(_)) => {
            debug!("stopped");
        }
        Ok(TelemetryMessage::DataSnapshot(_)) => {
            info!("    {:?}", &message.unwrap());
        }
        Ok(TelemetryMessage::MachineStateSnapshot(_)) => {
            debug!("------------------------------------------------------------------------------------");
            info!("{:?}", &message.unwrap());
            debug!("------------------------------------------------------------------------------------");
        }
        Ok(TelemetryMessage::AlarmTrap(AlarmTrap { triggered, .. })) => {
            let prefix = if triggered { "NEW ALARM" } else { "STOPPED" };
            debug!("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            info!("{} {:?}", &prefix, &message.unwrap());
            debug!("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
        Err(e) => {
            warn!("An error occurred: {:?}", e);
        }
    }
}

pub fn gather_telemetry_from_file(file: File, tx: Sender<TelemetryChannelType>) {
    let reader = BufReader::new(file);
    let mut buffer = Vec::new();

    let stopped_message_period = std::time::Duration::from_millis(100);
    let data_message_period = std::time::Duration::from_millis(10);

    info!("Start playing telemetry messages");

    for line in reader.lines() {
        if let Ok(line_str) = line {
            if let Ok(mut bytes) = base64::decode(line_str) {
                buffer.append(&mut bytes);

                while !buffer.is_empty() {
                    // Let's try to parse the buffer
                    match parse_telemetry_message(&buffer) {
                        // It worked! Let's extract the message and replace the buffer with the rest of the bytes
                        Ok((rest, message)) => {
                            match message {
                                TelemetryMessage::StoppedMessage { .. } => {
                                    std::thread::sleep(stopped_message_period);
                                }
                                TelemetryMessage::DataSnapshot { .. } => {
                                    std::thread::sleep(data_message_period);
                                }
                                _ => (),
                            }
                            tx.send(Ok(message)).unwrap();
                            buffer = Vec::from(rest);
                        }
                        // There are not enough bytes, let's wait until we get more
                        Err(nom::Err::Incomplete(_)) => {
                            break;
                        }
                        // We can't do anything with the begining of the buffer, let's drop its first byte
                        Err(e) => {
                            debug!("{:?}", &e);
                            if !buffer.is_empty() {
                                buffer.remove(0);
                            }
                        }
                    }
                }
            }
        }
    }
}
