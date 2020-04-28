// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::UiBuilder;
use glium::glutin::Icon;
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};
use image::load_from_memory;

use crate::EmbeddedFonts;
use crate::EmbeddedImages;

use crate::config::environment::*;
use crate::APP_ARGS;

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

lazy_static! {
    static ref IMAGE_WINDOW_ICON_RGBA_RAW: Vec<u8> =
        load_from_memory(EmbeddedImages::get("window-icon.png").unwrap().to_mut())
            .unwrap()
            .into_rgba()
            .into_raw();
    static ref FONT_NOTOSANS_BOLD: conrod_core::text::Font = conrod_core::text::Font::from_bytes(
        EmbeddedFonts::get("notosans_bold.ttf")
            .unwrap()
            .into_owned()
    )
    .unwrap();
    static ref FONT_NOTOSANS_REGULAR: conrod_core::text::Font =
        conrod_core::text::Font::from_bytes(
            EmbeddedFonts::get("notosans_regular.ttf")
                .unwrap()
                .into_owned()
        )
        .unwrap();
}

impl DisplayWindow {
    pub fn spawn(&self) {
        debug!("spawning window");

        // Create event loop
        let events_loop = EventsLoop::new();

        // Create window
        let window = WindowBuilder::new()
            .with_title("MakAir")
            .with_window_icon(Some(
                Icon::from_rgba(
                    IMAGE_WINDOW_ICON_RGBA_RAW.to_vec(),
                    WINDOW_ICON_WIDTH,
                    WINDOW_ICON_HEIGHT,
                )
                .unwrap(),
            ))
            .with_dimensions((DISPLAY_WINDOW_SIZE_WIDTH, DISPLAY_WINDOW_SIZE_HEIGHT).into())
            .with_decorations(false)
            .with_resizable(false)
            .with_always_on_top(true);

        let window = if APP_ARGS.fullscreen {
            let primary_monitor = events_loop.get_primary_monitor();
            window.with_fullscreen(Some(primary_monitor))
        } else {
            window
        };

        // Create context
        let context = ContextBuilder::new().with_multisampling(4).with_vsync(true);

        // Create the interface
        let mut interface = UiBuilder::new([
            DISPLAY_WINDOW_SIZE_WIDTH as f64,
            DISPLAY_WINDOW_SIZE_HEIGHT as f64,
        ])
        .build();

        // Load all required fonts to interface
        let bold_font = interface.fonts.insert(FONT_NOTOSANS_BOLD.clone());

        // last loaded font is the one that will be used by default
        let regular_font = interface.fonts.insert(FONT_NOTOSANS_REGULAR.clone());

        let fonts = Fonts::new(regular_font, bold_font);

        // Create window contents drawer
        let mut drawer = DisplayDrawerBuilder::new(window, context, events_loop, interface, fonts);

        drawer.run();
    }
}
