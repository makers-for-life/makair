// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

#[macro_use]
extern crate log;
#[macro_use]
extern crate clap;
#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate conrod_core;
extern crate conrod_winit;
extern crate image;

mod config;
mod display;
mod serial;

use std::ops::Deref;
use std::str::FromStr;

use clap::{App, Arg};
use log::LevelFilter;

use config::logger::ConfigLogger;
use display::window::DisplayWindowBuilder;

struct AppArgs {
    log: String,
    port: String,
}

lazy_static! {
    static ref APP_ARGS: AppArgs = make_app_args();
}

fn make_app_args() -> AppArgs {
    let matches = App::new(crate_name!())
        .version(crate_version!())
        .author(crate_authors!())
        .about(crate_description!())
        .arg(
            Arg::with_name("log")
                .short("l")
                .long("log")
                .help("Log level")
                .default_value("debug")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("port")
                .short("p")
                .long("port")
                .help("Serial port ID")
                .takes_value(true),
        )
        .get_matches();

    // Generate owned app arguments
    AppArgs {
        log: String::from(matches.value_of("log").expect("invalid log value")),
        port: matches
            .value_of("port")
            .expect("please provide a serial port value")
            .parse::<u8>()
            .expect("serial port should be a number")
            .to_string(),
    }
}

fn ensure_states() {
    // Ensure all statics are valid (a `deref` is enough to lazily initialize them)
    let _ = APP_ARGS.deref();
}

fn main() {
    let _logger =
        ConfigLogger::init(LevelFilter::from_str(&APP_ARGS.log).expect("invalid log level"));

    info!("starting up");

    // Ensure all states are bound
    ensure_states();

    // Spawn window manager
    DisplayWindowBuilder::new().spawn();

    info!("stopped");
}
