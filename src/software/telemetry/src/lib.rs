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

use serial::prelude::*;
use std::io::Read;
use std::sync::mpsc::Sender;

use parsers::*;
use structures::*;

pub fn gather_telemetry(port_id: &str, tx: Sender<TelemetryMessage>) {
    loop {
        info!("Opening {}", &port_id);
        match serial::open(&port_id) {
            Err(e) => {
                error!("{}", e);
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
                                            tx.send(message).unwrap();
                                            buffer = Vec::from(rest);
                                        }
                                        // There are not enough bytes, let's wait until we get more
                                        Err(nom::Err::Incomplete(_)) => {
                                            // Do nothing
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
                                // We failed to get a new byte from serial
                                Err(e) => {
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

pub fn display_message(message: TelemetryMessage) {
    match message {
        TelemetryMessage::BootMessage {
            min8,
            max8,
            min32,
            max32,
            ..
        } => {
            debug!("####################################################################################");
            debug!("######### CONTROLLER STARTED #########");
            debug!("####################################################################################");
            info!("{:?}", &message);
            debug!("####################################################################################");
            if min8 != 0u8 {
                warn!("min8 should be equal to 0 (found {})", &min8);
            }
            if max8 != 255u8 {
                warn!("max8 should be equal to 255 (found {})", &max8);
            }
            if min32 != 0u32 {
                warn!("min32 should be equal to 0 (found {})", &min32);
            }
            if max32 != 4_294_967_295_u32 {
                warn!("max32 should be equal to 0 (found {})", &max32);
            }
        }
        TelemetryMessage::DataSnapshot { .. } => {
            info!("    {:?}", &message);
        }
        TelemetryMessage::MachineStateSnapshot { .. } => {
            debug!("------------------------------------------------------------------------------------");
            info!("{:?}", &message);
            debug!("------------------------------------------------------------------------------------");
        }
        TelemetryMessage::AlarmTrap { triggered, .. } => {
            let prefix = if triggered { "NEW ALARM" } else { "STOPPED" };
            debug!("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            info!("{} {:?}", &prefix, &message);
            debug!("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
    }
}
