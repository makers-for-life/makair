// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use glium::glutin::WindowBuilder;
use piston_window::image::Image;
use piston_window::{Event, Glyphs, PistonWindow, Text, Texture};

use std::path::{Path, PathBuf};

use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};
use log::{info, warn};
use std::{thread, time};

use image::{buffer::ConvertBuffer, ImageBuffer, Rgb, RgbImage, RgbaImage};
use rand::Rng;

use conrod_core::{color, widget, Colorable, Positionable, Sizeable, Ui, Widget};
use glium::glutin::{ContextBuilder, EventsLoop};
use glium::Surface;

use crate::APP_ARGS;

use super::support::{self, EventLoop};
use std::sync::mpsc::{Receiver, Sender, TryRecvError};
use telemetry::{gather_telemetry, structures::TelemetryMessage};

struct DisplayDrawerLoaderBuilder;

pub struct DisplayDrawerLoader {
    // pub top_logo: (Image, Texture<Texture as Resources::Texture>),
}

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    pub loader: DisplayDrawerLoader,

    gl: conrod_glium::Renderer,
    rotation: f64,
    display: support::GliumDisplayWinitWrapper,
    interface: conrod_core::Ui,
    events_loop: EventsLoop,
    event_loop: EventLoop,
}

type DataPressure = Vec<(DateTime<Local>, u16)>;

impl DisplayDrawerLoaderBuilder {
    fn new() -> DisplayDrawerLoader {
        DisplayDrawerLoader {
            // TODO
            // top_logo: Self::load_top_logo(),
        }
    }

    fn acquire_path(name: &str) -> PathBuf {
        Path::new(&format!("./res/{}.png", name)).to_path_buf()
    }

    fn load_top_logo() -> Image {
        // (
        // TODO: proper size & position
        Image::new().rect(piston_window::rectangle::square(0.0, 0.0, 200.0))
        // TODO: acquire path from fn
        // Texture::from_path(Self::acquire_path("top-logo")).unwrap(),
        // )
    }
}

impl DisplayDrawerBuilder {
    pub fn new(
        window: WindowBuilder,
        context: ContextBuilder,
        events_loop: EventsLoop,
        interface: Ui,
    ) -> DisplayDrawer {
        // Create display
        let display = support::GliumDisplayWinitWrapper(
            glium::Display::new(window, context, &events_loop).unwrap(),
        );

        // Create renderer
        let mut renderer = conrod_glium::Renderer::new(&display.0).unwrap();

        DisplayDrawer {
            loader: DisplayDrawerLoaderBuilder::new(),
            gl: renderer,
            display: display,
            rotation: 0.0,
            interface: interface,
            events_loop: events_loop,
            event_loop: EventLoop::new(),
        }
    }
}

impl DisplayDrawer {
    pub fn cycle(&mut self) {
        let mut data_pressure = Vec::new();

        let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
            std::sync::mpsc::channel();

        std::thread::spawn(move || {
            gather_telemetry(&APP_ARGS.port, tx);
        });

        let mut last_point = Local::now();
        let mut last_cycles = 0;

        'main: loop {
            // TODO: only update when needed
            self.event_loop.needs_update();
            'thread_rcv: loop {
                match rx.try_recv() {
                    Ok(msg) => match msg {
                        TelemetryMessage::DataSnapshot { pressure, .. } => {
                            let now = Local::now();
                            let last = now - last_point;
                            if last > chrono::Duration::milliseconds(32) {
                                last_point = now;
                                Self::add_pressure(&mut data_pressure, pressure);
                            }
                        }
                        TelemetryMessage::MachineStateSnapshot { cpm_command, .. } => {
                            last_cycles = cpm_command;
                        }
                        _ => {}
                    },
                    Err(TryRecvError::Empty) => {
                        break 'thread_rcv;
                    }
                    Err(TryRecvError::Disconnected) => {
                        panic!("Channel to serial port thread was closed");
                    }
                }
            }

            // Handle all events.
            for event in self.event_loop.next(&mut self.events_loop) {
                // Use the `winit` backend feature to convert the winit event to a conrod one.
                if let Some(event) = support::convert_event(event.clone(), &self.display) {
                    self.interface.handle_event(event);
                    self.event_loop.needs_update();
                }

                // Break from the loop upon `Escape` or closed window.
                match event.clone() {
                    glium::glutin::Event::WindowEvent { event, .. } => match event {
                        glium::glutin::WindowEvent::CloseRequested
                        | glium::glutin::WindowEvent::KeyboardInput {
                            input:
                                glium::glutin::KeyboardInput {
                                    virtual_keycode: Some(glium::glutin::VirtualKeyCode::Escape),
                                    ..
                                },
                            ..
                        } => break 'main,
                        _ => (),
                    },
                    _ => (),
                }
            }

            if data_pressure.len() == 0 {
                continue;
            }

            let image_map = self.render(&data_pressure, last_cycles);

            // Draw the `Ui` if it has changed.
            if let Some(primitives) = self.interface.draw_if_changed() {
                self
                    .gl
                    .fill(&self.display.0, primitives, &image_map);
                let mut target = self.display.0.draw();
                target.clear_color(0.0, 0.0, 0.0, 1.0);
                self
                    .gl
                    .draw(&self.display.0, &mut target, &image_map)
                    .unwrap();
                target.finish().unwrap();
            }
        }
    }

    fn render(
        &mut self,
        data_pressure: &DataPressure,
        cycles: u8,
    ) -> conrod_core::image::Map<glium::texture::Texture2d> {
        const GREEN: [f32; 4] = [0.0, 1.0, 0.0, 1.0];
        const RED: [f32; 4] = [1.0, 0.0, 0.0, 1.0];

        //let square = rectangle::square(0.0, 0.0, 50.0);
        let rotation = self.rotation;
        //let (x, y) = (
        //args.window_size[0] / 2.0,
        //args.window_size[1] / 2.0 + args.window_size[1] / 4.0,
        //);

        let mut buffer = vec![0; (780 * 200 * 4) as usize];
        let root = BitMapBackend::with_buffer(&mut buffer, (780, 200)).into_drawing_area();
        //let root = BitMapBackend::new(Path::new("/tmp/foo.png"), (780, 200)).into_drawing_area();
        root.fill(&WHITE).unwrap();

        let oldest = data_pressure.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = data_pressure.first().unwrap().0;

        let mut chart = ChartBuilder::on(&root)
            .x_label_area_size(0)
            .y_label_area_size(40)
            .build_ranged(oldest..newest, 0..70)
            .unwrap();
        chart.configure_mesh().draw().unwrap();
        chart
            .draw_series(LineSeries::new(
                data_pressure.iter().map(|x| (x.0, x.1 as i32)),
                ShapeStyle::from(&BLACK).filled(),
            ))
            .unwrap();

        drop(chart);
        drop(root);
        let rgba_image: RgbaImage = RgbImage::from_raw(780, 200, buffer).unwrap().convert();
        let image_dimensions = rgba_image.dimensions();
        let raw_image = glium::texture::RawImage2d::from_raw_rgba_reversed(
            &rgba_image.into_raw(),
            image_dimensions,
        );
        let image_texture = glium::texture::Texture2d::new(&self.display.0, raw_image).unwrap();

        let (w, h) = (
            image_texture.get_width(),
            image_texture.get_height().unwrap(),
        );
        let mut image_map = conrod_core::image::Map::new();
        let image = image_map.insert(image_texture);

        // The `WidgetId` for our background and `Image` widgets.
        widget_ids!(struct Ids { background, content, cycles });
        let ids = Ids::new(self.interface.widget_id_generator());

        {
            let interface = &mut self.interface.set_widgets();
            // Draw a light blue background.
            widget::Canvas::new()
                .color(color::WHITE)
                .set(ids.background, interface);
            // Instantiate the `Image` at its full size in the middle of the window.
            widget::Image::new(image)
                .w_h(w as f64, h as f64)
                .mid_top()
                .set(ids.content, interface);

            widget::Text::new(&format!("Cycles per minute: {}", cycles))
                .color(color::LIGHT_BLUE)
                .bottom_left()
                .font_size(32)
                .set(ids.cycles, interface);
        }

        image_map
    }

    fn update(&mut self) {
        // Rotate 2 radians per second.
        //self.rotation += 2.0 * args.dt;
    }

    fn add_pressure(data: &mut DataPressure, new_point: u16) {
        data.insert(0, (Local::now(), new_point / 10));
        let oldest = data.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = data.first().unwrap().0;
        let mut i = 0;
        while i != data.len() {
            if oldest > (&mut data[i]).0 || newest < (&mut data[i]).0 {
                let _ = data.remove(i);
            } else {
                i += 1;
            }
        }
    }
}
