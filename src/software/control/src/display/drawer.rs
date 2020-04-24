// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, Sender, TryRecvError};

use chrono::offset::Utc;
use chrono::{DateTime, Duration};
use conrod_core::Ui;
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};
use glium::Surface;
use image::{buffer::ConvertBuffer, RgbImage, RgbaImage};
use plotters::prelude::*;
use telemetry::{
    self,
    structures::MachineStateSnapshot,
    structures::{DataSnapshot, TelemetryMessage},
};

use crate::APP_ARGS;

use super::fonts::Fonts;
use super::support::{self, EventLoop};
use super::widgets::{create_widgets, Ids};
use crate::chip::Chip;

lazy_static! {
    static ref SERIAL_RECEIVE_CHUNK_TIME: Duration = Duration::milliseconds(32);
}

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    renderer: conrod_glium::Renderer,
    display: support::GliumDisplayWinitWrapper,
    interface: conrod_core::Ui,
    events_loop: EventsLoop,
    event_loop: EventLoop,
    fonts: Fonts,
    chip: Chip, // TODO: should be moved once we move out all the telemetry fetching code out of this display package
}

enum HandleLoopOutcome {
    Break,
    Continue,
}

type DataPressure = Vec<(DateTime<Utc>, u16)>;

impl DisplayDrawerBuilder {
    #[allow(clippy::new_ret_no_self)]
    pub fn new(
        window: WindowBuilder,
        context: ContextBuilder,
        events_loop: EventsLoop,
        interface: Ui,
        fonts: Fonts,
    ) -> DisplayDrawer {
        // Create display
        let display = glium::Display::new(window, context, &events_loop).unwrap();
        let display = support::GliumDisplayWinitWrapper(display);

        // Create renderer
        let renderer = conrod_glium::Renderer::new(&display.0).unwrap();

        // Create drawer
        DisplayDrawer {
            renderer,
            display,
            interface,
            events_loop,
            event_loop: EventLoop::new(),
            fonts,
            chip: Chip::new(),
        }
    }
}

impl DisplayDrawer {
    pub fn run(&mut self) {
        // TODO: move more of this into the "serial" module

        let mut data: DataPressure = Vec::new();

        // Start gathering telemetry
        let rx = self.start_telemetry();

        let mut last_machine_snapshot = MachineStateSnapshot::default();

        // Start drawer loop
        // Flow: cycles through telemetry events, and refreshes the view every time there is an \
        //   update on the machines state.
        'main: loop {
            // TODO: only update when needed
            self.event_loop.needs_update();

            // Receive telemetry data (from the input serial from the motherboard)
            if let Some(machine_snapshot) = self.step_loop_telemetry(&rx, &mut data) {
                last_machine_snapshot = machine_snapshot;
            }

            let older = Utc::now() - chrono::Duration::seconds(40);
            data.retain(|d| d.0 > older);

            // Handle incoming events
            match self.step_loop_events() {
                HandleLoopOutcome::Break => break 'main,
                HandleLoopOutcome::Continue => {}
            }

            // Refresh the pressure data interface, if we have any data in the buffer
            if !data.is_empty() {
                self.step_loop_refresh(&data, &last_machine_snapshot);
            }
        }
    }

    // TODO: refactor this
    #[allow(clippy::ptr_arg)]
    fn render(
        &mut self,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
    ) -> conrod_core::image::Map<glium::texture::Texture2d> {
        let mut buffer = vec![0; (780 * 200 * 4) as usize];
        let root = BitMapBackend::with_buffer(&mut buffer, (780, 200)).into_drawing_area();
        root.fill(&BLACK).unwrap();

        let oldest = data_pressure.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = data_pressure.first().unwrap().0;

        let mut chart = ChartBuilder::on(&root)
            .margin(10)
            .x_label_area_size(10)
            .y_label_area_size(40)
            .build_ranged(oldest..newest, 0..70)
            .unwrap();
        chart
            .configure_mesh()
            .line_style_1(&plotters::style::colors::WHITE.mix(0.5))
            .line_style_2(&plotters::style::colors::BLACK)
            .y_labels(5)
            .y_label_style(
                plotters::style::TextStyle::from(("sans-serif", 20).into_font()).color(&WHITE),
            )
            .draw()
            .unwrap();
        chart
            .draw_series(LineSeries::new(
                data_pressure.iter().map(|x| (x.0, x.1 as i32)),
                ShapeStyle::from(&plotters::style::RGBColor(0, 137, 255))
                    .filled()
                    .stroke_width(1),
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
        let image_id = image_map.insert(image_texture);

        // The `WidgetId` for our background and `Image` widgets.
        let ids = Ids::new(self.interface.widget_id_generator());
        let ui = self.interface.set_widgets();
        create_widgets(ui, ids, image_id, (w, h), &self.fonts, &machine_snapshot);

        image_map
    }

    fn start_telemetry(&self) -> Receiver<TelemetryMessage> {
        // Start gathering telemetry
        let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
            std::sync::mpsc::channel();

        std::thread::spawn(move || {
            telemetry::gather_telemetry(&APP_ARGS.port, tx, None);
        });

        rx
    }

    // TODO: refactor, rename and relocate this
    fn add_pressure(&self, data: &mut DataPressure, snapshot: DataSnapshot) {
        assert!(self.chip.boot_time.is_some());

        let snapshot_time =
            self.chip.boot_time.unwrap() + Duration::microseconds(snapshot.systick as i64);
        if !data.is_empty() {
            let last_point = data.last().unwrap();
            let diff_between = snapshot_time - last_point.0;
            if diff_between < *SERIAL_RECEIVE_CHUNK_TIME {
                return;
            }
        }

        let point = snapshot.pressure / 10;
        data.insert(0, (snapshot_time, point));
    }

    // TODO: relocate this
    fn step_loop_telemetry(
        &mut self,
        rx: &Receiver<TelemetryMessage>,
        data: &mut DataPressure,
    ) -> Option<MachineStateSnapshot> {
        let mut machine_snapshot = None;
        loop {
            match rx.try_recv() {
                Ok(message) => match message {
                    TelemetryMessage::BootMessage(snapshot) => {
                        self.chip.reset(snapshot.systick);
                    }
                    TelemetryMessage::DataSnapshot(snapshot) => {
                        self.chip.update_tick(snapshot.systick);
                        self.add_pressure(data, snapshot);
                    }
                    TelemetryMessage::MachineStateSnapshot(snapshot) => {
                        machine_snapshot = Some(snapshot);
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

        machine_snapshot
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
            if let glium::glutin::Event::WindowEvent { event, .. } = event.clone() {
                match event {
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
                }
            }
        }

        HandleLoopOutcome::Continue
    }

    #[allow(clippy::ptr_arg)]
    fn step_loop_refresh(&mut self, data: &DataPressure, snapshot: &MachineStateSnapshot) {
        let image_map = self.render(data, &snapshot);

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
