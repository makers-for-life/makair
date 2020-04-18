#[macro_use]
extern crate log;

use telemetry::*;

fn main() {
    env_logger::init();

    if let Some(port_id) = std::env::args().nth(1) {
        if !port_id.is_empty() {
            gather_telemetry(&port_id);
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
