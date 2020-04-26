// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::Ui;
use glium::texture;
use image::{buffer::ConvertBuffer, RgbImage, RgbaImage};
use plotters::prelude::*;
use telemetry::structures::MachineStateSnapshot;

use crate::config::environment::{
    DISPLAY_GRAPH_OFFSET_HEIGHT, DISPLAY_GRAPH_OFFSET_WIDTH, DISPLAY_WINDOW_SIZE_HEIGHT,
    DISPLAY_WINDOW_SIZE_WIDTH, GRAPH_DRAW_LABEL_WIDTH, GRAPH_DRAW_LINE_SIZE, GRAPH_DRAW_MARGIN,
    GRAPH_DRAW_RANGE_HIGH, GRAPH_DRAW_RANGE_LOW, GRAPH_DRAW_SECONDS,
};
use crate::physics::types::DataPressure;

use super::drawer::UIState;
use super::fonts::Fonts;
use super::support::GliumDisplayWinitWrapper;
use super::widgets::{self, Ids};

pub struct DisplayRendererBuilder;

pub struct DisplayRenderer {
    fonts: Fonts,
}

const GRAPH_WIDTH: u32 = DISPLAY_WINDOW_SIZE_WIDTH - DISPLAY_GRAPH_OFFSET_WIDTH;
const GRAPH_HEIGHT: u32 = DISPLAY_WINDOW_SIZE_HEIGHT - DISPLAY_GRAPH_OFFSET_HEIGHT;

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
        display: &GliumDisplayWinitWrapper,
        interface: &mut Ui,
        ui_state: &UIState,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let image_map = conrod_core::image::Map::<texture::Texture2d>::new();

        // The `WidgetId` for our background and `Image` widgets.
        let ids = Ids::new(interface.widget_id_generator());

        // .clone() makes the borrow checker happy
        match ui_state.clone() {
            UIState::WaitingData => self.empty(ids, interface, image_map),
            UIState::Stopped => self.stopped(ids, interface, image_map),
            UIState::Running => self.data(
                ids,
                display,
                interface,
                image_map,
                data_pressure,
                machine_snapshot,
            ),
            UIState::Error(e) => self.error(ids, interface, image_map, e),
        }
    }

    fn empty(
        &mut self,
        ids: Ids,
        interface: &mut Ui,
        image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = interface.set_widgets();

        widgets::create_no_data_widget(ui, ids, &self.fonts);

        image_map
    }

    fn stopped(
        &mut self,
        ids: Ids,
        interface: &mut Ui,
        image_map: conrod_core::image::Map<texture::Texture2d>,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        let ui = interface.set_widgets();

        widgets::create_stopped_widget(ui, ids, &self.fonts);

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

        widgets::create_error_widget(ui, ids, &self.fonts, error);

        image_map
    }

    #[allow(clippy::ptr_arg)]
    fn data(
        &mut self,
        ids: Ids,
        display: &GliumDisplayWinitWrapper,
        interface: &mut Ui,
        mut image_map: conrod_core::image::Map<texture::Texture2d>,
        data_pressure: &DataPressure,
        machine_snapshot: &MachineStateSnapshot,
    ) -> conrod_core::image::Map<texture::Texture2d> {
        // Create chart
        let mut buffer = vec![0; (GRAPH_WIDTH * GRAPH_HEIGHT * 4) as usize];

        // Docs: https://docs.rs/plotters/0.2.12/plotters/drawing/struct.BitMapBackend.html
        let root = BitMapBackend::with_buffer(&mut buffer, (GRAPH_WIDTH, GRAPH_HEIGHT))
            .into_drawing_area();
        root.fill(&BLACK).unwrap();

        let newest_time = data_pressure.front().unwrap().0;
        let oldest_time = newest_time - chrono::Duration::seconds(GRAPH_DRAW_SECONDS as _);

        // Docs: https://docs.rs/plotters/0.2.12/plotters/chart/struct.ChartBuilder.html
        let mut chart = ChartBuilder::on(&root)
            .margin(GRAPH_DRAW_MARGIN)
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
            .draw_series(
                LineSeries::new(
                    data_pressure.iter().map(|x| (x.0, x.1 as i32)),
                    ShapeStyle::from(&plotters::style::RGBColor(0, 137, 255)).filled(),
                )
                .point_size(GRAPH_DRAW_LINE_SIZE),
            )
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
        let image_texture = glium::texture::Texture2d::new(&display.0, raw_image).unwrap();

        let (w, h) = (
            image_texture.get_width(),
            image_texture.get_height().unwrap(),
        );

        let image_id = image_map.insert(image_texture);

        // Create widgets
        let ui = interface.set_widgets();

        widgets::create_widgets(ui, ids, image_id, (w, h), &self.fonts, &machine_snapshot);

        image_map
    }

    fn draw_data_chart() {
        // TODO: move chart drawer code block there
    }

    fn draw_data_widgets() {
        // TODO: move widgets drawer code block there
    }
}
