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
extern crate rust_embed;
#[macro_use]
extern crate conrod_core;
extern crate conrod_winit;
extern crate image;

mod chip;
mod config;
mod display;
mod physics;
mod serial;

use std::ops::Deref;
use std::str::FromStr;

use clap::{App, Arg};
use log::LevelFilter;

use config::logger::ConfigLogger;
use display::window::DisplayWindowBuilder;

#[derive(RustEmbed)]
#[folder = "res/images/"]
pub struct EmbeddedImages;

#[derive(RustEmbed)]
#[folder = "res/fonts/"]
pub struct EmbeddedFonts;

struct AppArgs {
    log: String,
    mode: Mode,
    fullscreen: bool,
}

pub enum Mode {
    Port {
        port: String,
        output_dir: Option<String>,
    },
    Input(String),
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
        .arg(
            Arg::with_name("input")
                .short("i")
                .long("input")
                .help("Path to a recorded input file")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("output")
                .short("o")
                .long("output")
                .help("Path to a directory where to record telemetry")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("fullscreen")
                .short("f")
                .long("fullscreen")
                .help("Launch in fullscreen mode"),
        )
        .get_matches();

    let mode = match (matches.value_of("port"), matches.value_of("file")) {
        (Some(p), _) => Mode::Port {
            port: p.to_string(),
            output_dir: matches.value_of("output").map(|str| str.to_string()),
        },
        (None, Some(f)) => Mode::Input(f.to_string()),
        (None, None) => {
            eprintln!("You should provide either a serial port (-p) or an input file (-i)");
            std::process::exit(1);
        }
    };

    // Generate owned app arguments
    AppArgs {
        log: String::from(matches.value_of("log").expect("invalid log value")),
        mode,
        fullscreen: matches.is_present("fullscreen"),
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
