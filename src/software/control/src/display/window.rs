// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use piston_window::{self, PistonWindow, WindowSettings};

use super::drawer::DisplayDrawerBuilder;
use crate::config::environment::{DISPLAY_WINDOW_SIZE_HEIGHT, DISPLAY_WINDOW_SIZE_WIDTH};

pub struct DisplayWindowBuilder;
pub struct DisplayWindow;

impl DisplayWindowBuilder {
    pub fn new() -> DisplayWindow {
        DisplayWindow {}
    }
}

impl DisplayWindow {
    pub fn spawn(&self) {
        debug!("spawning window");

        // Create window
        let mut window: PistonWindow = WindowSettings::new(
            "MakAir",
            [DISPLAY_WINDOW_SIZE_WIDTH, DISPLAY_WINDOW_SIZE_HEIGHT],
        )
        // TODO: will be fullscreen
        // .fullscreen(true)
        .decorated(false)
        .exit_on_esc(true)
        .build()
        .unwrap();

        // Create window contents drawer
        let mut drawer = DisplayDrawerBuilder::new();

        // Cycle through window contents drawing
        while let Some(event) = window.next() {
            drawer.cycle(&mut window, &event);
        }
    }
}
