// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use chrono::{DateTime, NaiveDateTime, Utc};
use conrod_core::Ui;
use glium::texture;
use image::{buffer::ConvertBuffer, load_from_memory, RgbImage, RgbaImage};
use plotters::prelude::*;
use telemetry::alarm::AlarmCode;
use telemetry::structures::{AlarmPriority, MachineStateSnapshot};

use crate::EmbeddedImages;

use crate::config::environment::*;

use crate::chip::ChipState;
use crate::physics::types::DataPressure;

use super::fonts::Fonts;
use super::screen::{
    Ids, Screen, ScreenBootLoader, ScreenDataBranding, ScreenDataGraph, ScreenDataTelemetry,
};
use super::support::GliumDisplayWinitWrapper;

pub struct DisplayRendererBuilder;

pub struct DisplayRenderer {
    fonts: Fonts,
}

const GRAPH_WIDTH: u32 = DISPLAY_WINDOW_SIZE_WIDTH - DISPLAY_GRAPH_OFFSET_WIDTH;
const GRAPH_HEIGHT: u32 = DISPLAY_WINDOW_SIZE_HEIGHT - DISPLAY_GRAPH_OFFSET_HEIGHT;

const FIRMWARE_VERSION_NONE: &str = "n/a";

lazy_static! {
    static ref IMAGE_TOP_LOGO_RGBA_RAW: Vec<u8> =
        load_from_memory(EmbeddedImages::get("top-logo.png").unwrap().to_mut())
            .unwrap()
            .into_rgba()
            .into_raw();
    static ref IMAGE_BOOTLOADER_LOGO_RGBA_RAW: Vec<u8> =
        load_from_memory(EmbeddedImages::get("bootloader-logo.png").unwrap().to_mut())
            .unwrap()
            .into_rgba()
            .into_raw();
    static ref IMAGE_TELEMETRY_ARROW_RGBA_RAW: Vec<u8> =
        load_from_memory(EmbeddedImages::get("telemetry-arrow.png").unwrap().to_mut())
            .unwrap()
            .into_rgba()
            .into_raw();
}

#[allow(clippy::new_ret_no_self)]
impl DisplayRendererBuilder {
    pub fn new(fonts: Fonts) -> DisplayRenderer {
        DisplayRenderer { fonts }
    }
}

impl DisplayRenderer {
    pub fn render(
        &mut self,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
        ongoing_alarms: &[(&AlarmCode, &AlarmPriority)],
        display: &GliumDisplayWinitWrapper,
        interface: &mut Ui,
        chip_state: &ChipState,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let image_map = conrod_core::image::Map::<texture::Texture2d>::new();

        // The `WidgetId` for our background and `Image` widgets.
        let ids = Ids::new(interface.widget_id_generator());

        match chip_state {
            ChipState::Initializing => self.initializing(ids, display, interface, image_map),
            ChipState::WaitingData => self.empty(ids, interface, image_map),
            ChipState::Running | ChipState::Stopped => self.data(
                ids,
                display,
                interface,
                image_map,
                data_pressure,
                machine_snapshot,
                ongoing_alarms,
                chip_state,
            ),
            ChipState::Error(e) => self.error(ids, interface, image_map, e.clone()),
        }
    }

    fn empty(
        &mut self,
        ids: Ids,
        interface: &mut Ui,
        image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = interface.set_widgets();

        let mut screen = Screen::new(ui, &ids, &self.fonts, None, None);

        screen.render_no_data();

        image_map
    }

    fn initializing(
        &mut self,
        ids: Ids,
        display: &GliumDisplayWinitWrapper,
        interface: &mut Ui,
        mut image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let bootloader_logo_image_texture = self.draw_bootloader_logo(display);
        let (bootloader_logo_width, bootloader_logo_height) = (
            bootloader_logo_image_texture.get_width(),
            bootloader_logo_image_texture.get_height().unwrap(),
        );
        let image_id = image_map.insert(bootloader_logo_image_texture);

        let ui = interface.set_widgets();

        let screen_boot_loader = ScreenBootLoader {
            image_id,
            width: bootloader_logo_width as _,
            height: bootloader_logo_height as _,
        };

        let mut screen = Screen::new(ui, &ids, &self.fonts, None, None);

        screen.render_initializing(screen_boot_loader);

        image_map
    }

    fn error(
        &mut self,
        ids: Ids,
        interface: &mut Ui,
        image_map: conrod_core::image::Map<texture::Texture2d>,
        error: String,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = interface.set_widgets();

        let mut screen = Screen::new(ui, &ids, &self.fonts, None, None);

        screen.render_error(error);

        image_map
    }

    #[allow(clippy::ptr_arg, clippy::too_many_arguments)]
    fn data(
        &mut self,
        mut ids: Ids,
        display: &GliumDisplayWinitWrapper,
        interface: &mut Ui,
        mut image_map: conrod_core::image::Map<texture::Texture2d>,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
        ongoing_alarms: &[(&AlarmCode, &AlarmPriority)],
        chip_state: &ChipState,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        // Create branding
        let branding_image_texture = self.draw_branding(display);
        let (branding_width, branding_height) = (
            branding_image_texture.get_width(),
            branding_image_texture.get_height().unwrap(),
        );
        let branding_image_id = image_map.insert(branding_image_texture);

        // Create graph
        let graph_image_texture = self.draw_data_chart(data_pressure, display);
        let (graph_width, graph_height) = (
            graph_image_texture.get_width(),
            graph_image_texture.get_height().unwrap(),
        );
        let graph_image_id = image_map.insert(graph_image_texture);

        // Create telemetry
        let telemetry_arrow_image_texture = self.draw_telemetry_arrow(display);
        let telemetry_arrow_image_id = image_map.insert(telemetry_arrow_image_texture);

        // Create widgets
        let mut ui = interface.set_widgets();

        for i in 0..ongoing_alarms.len() {
            let index = i + 1;
            ids.alarm_alarms
                .resize(index, &mut ui.widget_id_generator());
            ids.alarm_codes_containers
                .resize(index, &mut ui.widget_id_generator());
            ids.alarm_codes.resize(index, &mut ui.widget_id_generator());
            ids.alarm_messages_containers
                .resize(index, &mut ui.widget_id_generator());
            ids.alarm_messages
                .resize(index, &mut ui.widget_id_generator());
        }

        let mut screen = Screen::new(
            ui,
            &ids,
            &self.fonts,
            Some(machine_snapshot),
            Some(ongoing_alarms),
        );

        let screen_data_branding = ScreenDataBranding {
            firmware_version: if machine_snapshot.version.is_empty() {
                FIRMWARE_VERSION_NONE
            } else {
                &machine_snapshot.version
            },
            image_id: branding_image_id,
            width: branding_width as _,
            height: branding_height as _,
        };

        let screen_data_graph = ScreenDataGraph {
            image_id: graph_image_id,
            width: graph_width as _,
            height: graph_height as _,
        };

        let screen_data_telemetry = ScreenDataTelemetry {
            arrow_image_id: telemetry_arrow_image_id,
        };

        match chip_state {
            ChipState::Running => screen.render_with_data(
                screen_data_branding,
                screen_data_graph,
                screen_data_telemetry,
            ),
            ChipState::Stopped => screen.render_stop(
                screen_data_branding,
                screen_data_graph,
                screen_data_telemetry,
            ),
            _ => unreachable!(),
        };

        image_map
    }

    fn draw_bootloader_logo(
        &self,
        display: &GliumDisplayWinitWrapper,
    ) -> glium::texture::Texture2d {
        // Create image from raw buffer
        let raw_image = glium::texture::RawImage2d::from_raw_rgba_reversed(
            &*IMAGE_BOOTLOADER_LOGO_RGBA_RAW,
            (BOOTLOADER_LOGO_WIDTH, BOOTLOADER_LOGO_HEIGHT),
        );

        glium::texture::Texture2d::new(&display.0, raw_image).unwrap()
    }

    fn draw_telemetry_arrow(
        &self,
        display: &GliumDisplayWinitWrapper,
    ) -> glium::texture::Texture2d {
        // Create image from raw buffer
        let raw_image = glium::texture::RawImage2d::from_raw_rgba_reversed(
            &*IMAGE_TELEMETRY_ARROW_RGBA_RAW,
            (TELEMETRY_ARROW_WIDTH, TELEMETRY_ARROW_HEIGHT),
        );

        glium::texture::Texture2d::new(&display.0, raw_image).unwrap()
    }

    fn draw_branding(&self, display: &GliumDisplayWinitWrapper) -> glium::texture::Texture2d {
        // Create image from raw buffer
        let raw_image = glium::texture::RawImage2d::from_raw_rgba_reversed(
            &*IMAGE_TOP_LOGO_RGBA_RAW,
            (BRANDING_WIDTH, BRANDING_HEIGHT),
        );

        glium::texture::Texture2d::new(&display.0, raw_image).unwrap()
    }

    fn draw_data_chart(
        &self,
        data_pressure: &DataPressure,
        display: &GliumDisplayWinitWrapper,
    ) -> glium::texture::Texture2d {
        let mut buffer = vec![0; (GRAPH_WIDTH * GRAPH_HEIGHT * 4) as usize];

        // Docs: https://docs.rs/plotters/0.2.12/plotters/drawing/struct.BitMapBackend.html
        let root = BitMapBackend::with_buffer(&mut buffer, (GRAPH_WIDTH, GRAPH_HEIGHT))
            .into_drawing_area();
        root.fill(&BLACK).unwrap();

        let newest_time = data_pressure
            .front()
            .unwrap_or(&(
                DateTime::from_utc(NaiveDateTime::from_timestamp(0, 0), Utc),
                0,
            ))
            .0;
        let oldest_time = newest_time - chrono::Duration::seconds(GRAPH_DRAW_SECONDS as _);

        // Docs: https://docs.rs/plotters/0.2.12/plotters/chart/struct.ChartBuilder.html
        let mut chart = ChartBuilder::on(&root)
            .margin_top(GRAPH_DRAW_MARGIN_TOP)
            .margin_bottom(GRAPH_DRAW_MARGIN_BOTTOM)
            .margin_left(GRAPH_DRAW_MARGIN_LEFT)
            .margin_right(GRAPH_DRAW_MARGIN_RIGHT)
            .x_label_area_size(0)
            .y_label_area_size(GRAPH_DRAW_LABEL_WIDTH)
            .build_ranged(
                oldest_time..newest_time,
                GRAPH_DRAW_RANGE_LOW..GRAPH_DRAW_RANGE_HIGH,
            )
            .unwrap();

        chart
            .configure_mesh()
            .line_style_1(&plotters::style::colors::WHITE.mix(0.04))
            .line_style_2(&plotters::style::colors::BLACK)
            .y_labels(5)
            .y_label_style(
                plotters::style::TextStyle::from(("sans-serif", 13).into_font())
                    .color(&WHITE.mix(0.65)),
            )
            .draw()
            .unwrap();

        // Docs: https://docs.rs/plotters/0.2.12/plotters/prelude/struct.LineSeries.html
        chart
            .draw_series(LineSeries::new(
                data_pressure.iter().map(|x| (x.0, x.1 as i32)),
                ShapeStyle::from(&plotters::style::RGBColor(0, 137, 255))
                    .filled()
                    .stroke_width(GRAPH_DRAW_LINE_SIZE),
            ))
            .unwrap();

        drop(chart);
        drop(root);

        // Convert chart to an image
        // TODO: draw image on a @2x or @4x buffer, then downsize and re-sample as to "simulate" \
        //   anti-aliasing, as by default all graphs are aliased
        // TODO: docs on https://docs.rs/image/0.23.4/image/imageops/fn.resize.html
        let rgba_image: RgbaImage = RgbImage::from_raw(GRAPH_WIDTH, GRAPH_HEIGHT, buffer)
            .unwrap()
            .convert();
        let image_dimensions = rgba_image.dimensions();

        let raw_image = glium::texture::RawImage2d::from_raw_rgba_reversed(
            &rgba_image.into_raw(),
            image_dimensions,
        );

        glium::texture::Texture2d::new(&display.0, raw_image).unwrap()
    }
}
