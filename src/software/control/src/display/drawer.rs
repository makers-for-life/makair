// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, Sender};

use chrono::offset::Utc;
use chrono::Duration;
use conrod_core::Ui;
use glium::glutin::{ContextBuilder, EventsLoop, WindowBuilder};
use glium::{texture, Surface};
use image::{buffer::ConvertBuffer, RgbImage, RgbaImage};
use plotters::prelude::*;
use std::collections::VecDeque;
use telemetry::serial::core::ErrorKind;
use telemetry::structures::{DataSnapshot, MachineStateSnapshot};
use telemetry::{self, TelemetryChannelType};

use crate::chip::Chip;
use crate::physics::types::DataPressure;
use crate::serial::poller::SerialPollerBuilder;
use crate::APP_ARGS;

use super::events::{DisplayEventsBuilder, DisplayEventsHandleOutcome};
use super::fonts::Fonts;
use super::support::GliumDisplayWinitWrapper;
use super::widgets::{
    create_error_widget, create_no_data_widget, create_stopped_widget, create_widgets, Ids,
};

const GRAPH_RENDER_SECONDS: usize = 40;
const TELEMETRY_POINTS_PER_SECOND: usize = 10 * 100;
const GRAPH_NUMBER_OF_POINTS: usize = GRAPH_RENDER_SECONDS * TELEMETRY_POINTS_PER_SECOND;
const FRAMERATE: u64 = 30;

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    renderer: conrod_glium::Renderer,
    display: GliumDisplayWinitWrapper,
    interface: Ui,
    events_loop: EventsLoop,
    fonts: Fonts,
    chip: Chip,
    ui_state: UIState,
}

#[derive(Debug, Clone)]
enum UIState {
    Running,
    Stopped,
    WaitingData,
    Error(String),
}

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
        let display = GliumDisplayWinitWrapper(display);

        // Create renderer
        let renderer = conrod_glium::Renderer::new(&display.0).unwrap();

        // Create drawer
        DisplayDrawer {
            renderer,
            display,
            interface,
            events_loop,
            fonts,
            chip: Chip::new(),
            ui_state: UIState::WaitingData,
        }
    }
}

impl DisplayDrawer {
    pub fn run(&mut self) {
        // Create handlers
        let mut serial_poller = SerialPollerBuilder::new();
        let mut events_handler = DisplayEventsBuilder::new();

        // Allow enough space to hold X seconds of points and 1 second of latency between clean-ups
        let mut data: DataPressure = VecDeque::with_capacity(GRAPH_NUMBER_OF_POINTS + 100);

        // Start gathering telemetry
        let rx = self.start_telemetry();

        let mut last_machine_snapshot = MachineStateSnapshot::default();

        // Start drawer loop
        // Flow: cycles through telemetry events, and refreshes the view every time there is an \
        //   update on the machines state.
        let mut last_render = Utc::now();

        'main: loop {
            // Receive telemetry data (from the input serial from the motherboard)
            match serial_poller.poll(&rx, &mut self.chip) {
                Ok(polled) => {
                    if let Some(data_snapshots) = polled.data_snapshots {
                        self.ui_state = UIState::Running;

                        data_snapshots
                            .iter()
                            .for_each(|snapshot| self.add_pressure(&mut data, snapshot));
                    } else if polled.stopped_message.is_some() {
                        // If we received some data, ignore the stopped event
                        // On the next iteration, we will either just receive the stopped event
                        // or some more data without it
                        self.ui_state = UIState::Stopped;
                    }

                    if let Some(machine_snapshot) = polled.machine_snapshot {
                        last_machine_snapshot = machine_snapshot;
                    }
                }

                Err(error) => match error.kind() {
                    ErrorKind::NoDevice => self.ui_state = UIState::WaitingData,
                    err => self.ui_state = UIState::Error(format!("{:?}", err)),
                },
            }

            match self.ui_state {
                UIState::WaitingData | UIState::Stopped => {
                    data.clear();
                    last_machine_snapshot = MachineStateSnapshot::default();
                }
                _ => (),
            };

            // Handle incoming events
            match events_handler.handle(&self.display, &mut self.interface, &mut self.events_loop) {
                DisplayEventsHandleOutcome::Break => break 'main,
                DisplayEventsHandleOutcome::Continue => {}
            }

            // Refresh the pressure data interface, if we have any data in the buffer
            let now = Utc::now();

            if (now - last_render) > Duration::milliseconds((1000 / FRAMERATE) as _) {
                if !data.is_empty() {
                    let older = now - chrono::Duration::seconds(40);
                    while data.back().map(|p| p.0 < older).unwrap_or(false) {
                        data.pop_back();
                    }
                }

                last_render = now;

                self.refresh(&data, &last_machine_snapshot);
            } else {
                std::thread::sleep(std::time::Duration::from_millis(10));
            }
        }
    }

    // TODO: refactor this
    fn render(
        &mut self,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let image_map = conrod_core::image::Map::<texture::Texture2d>::new();

        // The `WidgetId` for our background and `Image` widgets.
        let ids = Ids::new(self.interface.widget_id_generator());

        // .clone() makes the borrow checker happy
        match self.ui_state.clone() {
            UIState::WaitingData => self.render_empty(ids, image_map),
            UIState::Stopped => self.render_stopped(ids, image_map),
            UIState::Running => self.render_data(ids, image_map, data_pressure, machine_snapshot),
            UIState::Error(e) => self.render_error(ids, image_map, e),
        }
    }

    fn render_empty(
        &mut self,
        ids: Ids,
        image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = self.interface.set_widgets();
        create_no_data_widget(ui, ids, &self.fonts);
        image_map
    }

    fn render_stopped(
        &mut self,
        ids: Ids,
        image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = self.interface.set_widgets();
        create_stopped_widget(ui, ids, &self.fonts);
        image_map
    }

    fn render_error(
        &mut self,
        ids: Ids,
        image_map: conrod_core::image::Map<texture::Texture2d>,
        error: String,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = self.interface.set_widgets();
        create_error_widget(ui, ids, &self.fonts, error);
        image_map
    }

    #[allow(clippy::ptr_arg)]
    fn render_data(
        &mut self,
        ids: Ids,
        mut image_map: conrod_core::image::Map<texture::Texture2d>,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = self.interface.set_widgets();
        let mut buffer = vec![0; (780 * 200 * 4) as usize];

        let root = BitMapBackend::with_buffer(&mut buffer, (780, 200)).into_drawing_area();
        root.fill(&BLACK).unwrap();

        let newest = data_pressure.front().unwrap().0;
        let oldest = newest - chrono::Duration::seconds(40);

        let mut chart = ChartBuilder::on(&root)
            .margin(10)
            .x_label_area_size(0)
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

        let image_id = image_map.insert(image_texture);

        create_widgets(ui, ids, image_id, (w, h), &self.fonts, &machine_snapshot);

        image_map
    }

    fn start_telemetry(&self) -> Receiver<TelemetryChannelType> {
        // Start gathering telemetry
        let (tx, rx): (Sender<TelemetryChannelType>, Receiver<TelemetryChannelType>) =
            std::sync::mpsc::channel();

        match &APP_ARGS.source {
            crate::Source::Port(port) => {
                std::thread::spawn(move || {
                    telemetry::gather_telemetry(&port, tx, None);
                });
            }

            crate::Source::File(path) => {
                std::thread::spawn(move || loop {
                    let file = std::fs::File::open(path).unwrap();
                    telemetry::gather_telemetry_from_file(file, tx.clone());
                });
            }
        }

        rx
    }

    #[allow(clippy::ptr_arg)]
    fn refresh(&mut self, data: &DataPressure, snapshot: &MachineStateSnapshot) {
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

    fn add_pressure(&self, data: &mut DataPressure, snapshot: &DataSnapshot) {
        assert!(self.chip.boot_time.is_some());

        let snapshot_time =
            self.chip.boot_time.unwrap() + Duration::microseconds(snapshot.systick as i64);

        let point = snapshot.pressure / 10;

        data.push_front((snapshot_time, point));
    }
}
