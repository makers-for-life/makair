// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use plotters::drawing::bitmap_pixel::BGRXPixel;
use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};
use log::{info, warn};
use std::{thread, time};

use std::collections::vec_deque::VecDeque;

use conrod_core::{Ui, UiBuilder};
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};
use glutin_window::GlutinWindow as Window;
use graphics::draw_state::DrawState;
use graphics::rectangle;
use graphics::rectangle::rectangle_by_corners;
use graphics::{clear, Image};
use image::ImageBuffer;
use image::{Rgb, RgbaImage};
use opengl_graphics::{GlGraphics, OpenGL, Texture, TextureSettings};
use piston::event_loop::{EventSettings, Events};
use piston::input::{RenderArgs, RenderEvent, UpdateArgs, UpdateEvent};
use piston::window::WindowSettings;
use rand::Rng;
use std::path::Path;

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

        // Create event loop
        let mut events_loop = EventsLoop::new();

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

        interface
            .fonts
            .insert_from_file("./res/fonts/notosans_regular.ttf")
            .unwrap();

        // Create window contents drawer
        let mut drawer = DisplayDrawerBuilder::new(window, context, events_loop, interface);

        drawer.cycle();
    }
}
