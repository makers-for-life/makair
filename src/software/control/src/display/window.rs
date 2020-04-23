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
use piston_window::PistonWindow;
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

        // Create window
        let mut window: PistonWindow = WindowSettings::new(
            "MakAir",
            [DISPLAY_WINDOW_SIZE_WIDTH, DISPLAY_WINDOW_SIZE_HEIGHT],
        )
        .graphics_api(OpenGL::V3_2)
        .decorated(false)
        .exit_on_esc(true)
        .build()
        .unwrap();

        // // Create window contents drawer
        let mut drawer = DisplayDrawerBuilder::new();

        // Cycle through window contents drawing
        while let Some(event) = window.next() {
            drawer.cycle(&mut window, &event);
        }
    }
}
