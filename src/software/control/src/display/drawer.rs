// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, Sender, TryRecvError};

use chrono::offset::Local;
use chrono::prelude::*;
use chrono::Duration;
use conrod_core::{color, widget, Colorable, Positionable, Sizeable, Ui, Widget};
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};
use glium::Surface;
use image::{buffer::ConvertBuffer, RgbImage, RgbaImage};
use plotters::prelude::*;
use telemetry::{self, structures::TelemetryMessage};

use crate::APP_ARGS;

use super::loader::{DisplayLoader, DisplayLoaderBuilder};
use super::support::{self, EventLoop};

lazy_static! {
    static ref SERIAL_RECEIVE_CHUNK_TIME: Duration = Duration::milliseconds(32);
}

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    loader: DisplayLoader,
    renderer: conrod_glium::Renderer,
    display: support::GliumDisplayWinitWrapper,
    interface: conrod_core::Ui,
    events_loop: EventsLoop,
    event_loop: EventLoop,
}

enum HandleLoopOutcome {
    Break,
    Continue,
}

type DataPressure = Vec<(DateTime<Local>, u16)>;

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
        let renderer = conrod_glium::Renderer::new(&display.0).unwrap();

        // Create drawer
        DisplayDrawer {
            loader: DisplayLoaderBuilder::new(),
            renderer: renderer,
            display: display,
            interface: interface,
            events_loop: events_loop,
            event_loop: EventLoop::new(),
        }
    }
}

impl DisplayDrawer {
    pub fn run(&mut self) {
        // TODO: move more of this into the "serial" module

        let mut data: DataPressure = Vec::new();

        // Start gathering telemetry
        let rx = self.start_telemetry();

        // Start drawer loop
        // Flow: cycles through telemetry events, and refreshes the view every time there is an \
        //   update on the machines state.
        'main: loop {
            // TODO: only update when needed
            self.event_loop.needs_update();

            // Receive telemetry data (from the input serial from the motherboard)
            let last_cycles = self.step_loop_telemetry(&rx, &mut data);

            // Handle incoming events
            match self.step_loop_events() {
                HandleLoopOutcome::Break => break 'main,
                HandleLoopOutcome::Continue => {}
            }

            // Refresh the pressure data interface, if we have any data in the buffer
            if data.len() > 0 {
                self.step_loop_refresh(&data, last_cycles);
            }
        }
    }

    // TODO: refactor this
    fn render(
        &mut self,
        data_pressure: &DataPressure,
        cycles: u8,
    ) -> conrod_core::image::Map<glium::texture::Texture2d> {
        let mut buffer = vec![0; (780 * 200 * 4) as usize];
        let root = BitMapBackend::with_buffer(&mut buffer, (780, 200)).into_drawing_area();
        root.fill(&WHITE).unwrap();

        let oldest = data_pressure.first().unwrap().0 - Duration::seconds(40);
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

    fn start_telemetry(&self) -> Receiver<TelemetryMessage> {
        // Start gathering telemetry
        let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
            std::sync::mpsc::channel();

        std::thread::spawn(move || {
            telemetry::gather_telemetry(&APP_ARGS.port, tx);
        });

        rx
    }

    // TODO: refactor, rename and relocate this
    fn add_pressure(&self, data: &mut DataPressure, new_point: u16) {
        data.insert(0, (Local::now(), new_point / 10));
        let oldest = data.first().unwrap().0 - Duration::seconds(40);
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

    // TODO: relocate this
    fn step_loop_telemetry(
        &mut self,
        rx: &Receiver<TelemetryMessage>,
        data: &mut DataPressure,
    ) -> u8 {
        let (mut last_point, mut last_cycles) = (Local::now(), 0);

        loop {
            match rx.try_recv() {
                Ok(message) => match message {
                    // TODO: add more message types
                    TelemetryMessage::DataSnapshot { pressure, .. } => {
                        let now = Local::now();
                        let last = now - last_point;

                        // Last received chunk is not too recent? Add the pressure measurement \
                        //   point that was received.
                        if last > *SERIAL_RECEIVE_CHUNK_TIME {
                            last_point = now;

                            self.add_pressure(data, pressure);
                        }
                    }

                    TelemetryMessage::MachineStateSnapshot { cpm_command, .. } => {
                        last_cycles = cpm_command;
                    }

                    _ => {}
                },

                Err(TryRecvError::Empty) => {
                    break;
                }

                Err(TryRecvError::Disconnected) => {
                    panic!("channel to serial port thread was closed");
                }
            }
        }

        last_cycles
    }

    // TODO: relocate this
    fn step_loop_events(&mut self) -> HandleLoopOutcome {
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
                    } => {
                        return HandleLoopOutcome::Break;
                    }

                    _ => (),
                },

                _ => (),
            }
        }

        return HandleLoopOutcome::Continue;
    }

    fn step_loop_refresh(&mut self, data: &DataPressure, last_cycles: u8) {
        let image_map = self.render(data, last_cycles);

        if let Some(primitives) = self.interface.draw_if_changed() {
            self.renderer.fill(&self.display.0, primitives, &image_map);

            let mut target = self.display.0.draw();

            target.clear_color(0.0, 0.0, 0.0, 1.0);

            self.renderer
                .draw(&self.display.0, &mut target, &image_map)
                .unwrap();

            target.finish().unwrap();
        }
    }
}
