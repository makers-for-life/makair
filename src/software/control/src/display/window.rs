// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::UiBuilder;
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};

use crate::config::environment::{DISPLAY_WINDOW_SIZE_HEIGHT, DISPLAY_WINDOW_SIZE_WIDTH};

use super::drawer::DisplayDrawerBuilder;
use super::fonts::Fonts;

pub struct DisplayWindowBuilder;
pub struct DisplayWindow;

impl DisplayWindowBuilder {
    #[allow(clippy::new_ret_no_self)]
    pub fn new() -> DisplayWindow {
        DisplayWindow {}
    }
}

impl DisplayWindow {
    pub fn spawn(&self) {
        debug!("spawning window");

        // Create event loop
        let events_loop = EventsLoop::new();

        // Create window
        let window = WindowBuilder::new()
            .with_title("MakAir")
            .with_dimensions((DISPLAY_WINDOW_SIZE_WIDTH, DISPLAY_WINDOW_SIZE_HEIGHT).into())
            .with_decorations(false)
            .with_resizable(false)
            .with_always_on_top(true);

        // Create context
        let context = ContextBuilder::new().with_multisampling(4).with_vsync(true);

        // Create the interface
        let mut interface = UiBuilder::new([
            DISPLAY_WINDOW_SIZE_WIDTH as f64,
            DISPLAY_WINDOW_SIZE_HEIGHT as f64,
        ])
        .build();

        // Load all required fonts to interface
        let bold_font = interface
            .fonts
            .insert_from_file("./res/fonts/notosans_bold.ttf")
            .unwrap();

        // last loaded font is the one that will be used by default
        let regular_font = interface
            .fonts
            .insert_from_file("./res/fonts/notosans_regular.ttf")
            .unwrap();

        let fonts = Fonts::new(regular_font, bold_font);

        // Create window contents drawer
        let mut drawer = DisplayDrawerBuilder::new(window, context, events_loop, interface, fonts);

        drawer.run();
    }
}
