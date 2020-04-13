use serial::prelude::*;
use std::borrow::BorrowMut;
use std::io::Read;

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
        let mut header_str = String::new();
        match port.borrow_mut().take(2).read_to_string(&mut header_str) {
            Ok(_) => {
                match header_str.as_ref() {
                    "D:" => {
                        let mut buffer = Vec::new();
                        for byte in port.borrow_mut().bytes() {
                            match byte {
                                Ok(b) => {
                                    if b == b'\n' {
                                        break;
                                    } else {
                                        buffer.push(b);
                                    }
                                }
                                Err(_) => break,
                            }
                        }
                        let fields: Vec<&[u8]> = buffer.split(|b| b == &b'\t').collect();
                        if fields.len() != 2 {
                            eprintln!("Wrong number of fields in data snapshot: {}", fields.len());
                        } else {
                            let device_id_bytes = fields.get(0).unwrap();
                            let device_id = format!("{}-{}-{}", device_id_bytes[0], device_id_bytes[1], device_id_bytes[2]);
                            let millis = fields.get(1).unwrap()[0];

                            println!("---- DATA SNAPSHOT ----\n");
                            dbg!(&buffer);
                            dbg!(std::str::from_utf8(&buffer));
                            dbg!(&fields);
                            println!("Device ID = {}", device_id);
                            println!("millis() = {}", millis);
                            println!("\n-----------------------\n\n");
                        }
                    }
                    _ => {
                        // Do nothing
                    }
                }
            }
            Err(e) => {
                if e.kind() == std::io::ErrorKind::TimedOut {
                    // Do nothing
                } else {
                    eprintln!("{:?}", e);
                    return;
                }
            }
        }
    }
}
