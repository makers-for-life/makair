// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::color::{self, Color};

use telemetry::structures::MachineStateSnapshot;

use crate::config::environment::DISPLAY_WIDGET_SPACING_FROM_BOTTOM;

use super::fonts::Fonts;
use super::widget::{
    BackgroundWidgetConfig, ControlWidget, ControlWidgetType, ErrorWidgetConfig, GraphWidgetConfig,
    NoDataWidgetConfig, StopWidgetConfig, TelemetryWidgetConfig,
};

widget_ids!(pub struct Ids {
  alarms,
  background,
  pressure_graph,

  cycles_parent,
  cycles_title,
  cycles_value,
  cycles_unit,

  peak_parent,
  peak_title,
  peak_value,
  peak_unit,

  plateau_parent,
  plateau_title,
  plateau_value,
  plateau_unit,

  peep_parent,
  peep_title,
  peep_value,
  peep_unit,

  ratio_parent,
  ratio_title,
  ratio_value,
  ratio_unit,

  tidal_parent,
  tidal_title,
  tidal_value,
  tidal_unit,

  no_data,
  stopped,
  error
});

pub struct Screen<'a> {
    ids: &'a Ids,
    machine_snapshot: Option<&'a MachineStateSnapshot>,
    widgets: ControlWidget<'a>,
}

impl<'a> Screen<'a> {
    pub fn new(
        ui: conrod_core::UiCell<'a>,
        ids: &'a Ids,
        fonts: &'a Fonts,
        machine_snapshot: Option<&'a MachineStateSnapshot>,
    ) -> Screen<'a> {
        Screen {
            ids,
            machine_snapshot,
            widgets: ControlWidget::new(ui, fonts),
        }
    }

    pub fn render_with_data(&mut self, image_id: conrod_core::image::Id, width: f64, height: f64) {
        self.render_background();
        self.render_graph(image_id, width, height);
        self.render_telemetry();
    }

    pub fn render_background(&mut self) {
        let config = BackgroundWidgetConfig::new(color::BLACK, self.ids.background);

        self.widgets.render(ControlWidgetType::Background(config));
    }

    pub fn render_graph(&mut self, image_id: conrod_core::image::Id, width: f64, height: f64) {
        let config = GraphWidgetConfig::new(width, height, image_id, self.ids.pressure_graph);

        self.widgets.render(ControlWidgetType::Graph(config));
    }

    pub fn render_stop(&mut self) {
        let config = StopWidgetConfig::new(self.ids.stopped);

        self.widgets.render(ControlWidgetType::Stop(config));
    }

    pub fn render_no_data(&mut self) {
        let config = NoDataWidgetConfig::new(self.ids.no_data);

        self.widgets.render(ControlWidgetType::NoData(config));
    }

    pub fn render_error(&mut self, error: String) {
        let config = ErrorWidgetConfig::new(error, self.ids.error);

        self.render_background();

        self.widgets.render(ControlWidgetType::Error(config));
    }

    pub fn render_telemetry(&mut self) {
        let mut last_widget_position = 0.0;
        let machine_snapshot = self.machine_snapshot.unwrap();

        let peak_config = TelemetryWidgetConfig {
            title: "P(peak)",
            value: format!(
                "{} ← ({})",
                (machine_snapshot.previous_peak_pressure as f64 / 10.0).round(),
                machine_snapshot.peak_command
            ),
            unit: "cmH20",
            ids: (
                self.ids.peak_parent,
                self.ids.peak_title,
                self.ids.peak_value,
                self.ids.peak_unit,
            ),
            x_position: last_widget_position,
            y_position: DISPLAY_WIDGET_SPACING_FROM_BOTTOM,
            background_color: Color::Rgba(39.0 / 255.0, 66.0 / 255.0, 100.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(peak_config));

        // Initialize the plateau widget
        let plateau_config = TelemetryWidgetConfig {
            title: "P(plateau)",
            value: format!(
                "{} ← ({})",
                (machine_snapshot.previous_plateau_pressure as f64 / 10.0).round(),
                machine_snapshot.plateau_command
            ),
            unit: "cmH20",
            ids: (
                self.ids.plateau_parent,
                self.ids.plateau_title,
                self.ids.plateau_value,
                self.ids.plateau_unit,
            ),
            x_position: last_widget_position,
            y_position: 0.0,
            background_color: Color::Rgba(66.0 / 255.0, 44.0 / 255.0, 85.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(plateau_config));

        // Initialize the PEEP widget
        let peep_config = TelemetryWidgetConfig {
            title: "P(expiratory)",
            value: format!(
                "{} ← ({})",
                (machine_snapshot.previous_peep_pressure as f64 / 10.0).round(),
                machine_snapshot.peep_command
            ),
            unit: "cmH20",
            ids: (
                self.ids.peep_parent,
                self.ids.peep_title,
                self.ids.peep_value,
                self.ids.peep_unit,
            ),
            x_position: last_widget_position,
            y_position: 0.0,
            background_color: Color::Rgba(76.0 / 255.0, 73.0 / 255.0, 25.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(peep_config));

        // Initialize the cycles widget
        let cycles_config = TelemetryWidgetConfig {
            title: "Cycles/minute",
            value: format!("{}", machine_snapshot.cpm_command),
            unit: "/minute",
            ids: (
                self.ids.cycles_parent,
                self.ids.cycles_title,
                self.ids.cycles_value,
                self.ids.cycles_unit,
            ),
            x_position: last_widget_position,
            y_position: 0.0,
            background_color: Color::Rgba(47.0 / 255.0, 74.0 / 255.0, 16.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(cycles_config));

        // Initialize the ratio widget
        let ratio_config = TelemetryWidgetConfig {
            title: "Insp-exp ratio",
            value: "0:0".to_string(), //TODO
            unit: "insp:exp.",
            ids: (
                self.ids.ratio_parent,
                self.ids.ratio_title,
                self.ids.ratio_value,
                self.ids.ratio_unit,
            ),
            x_position: last_widget_position,
            y_position: 0.0,
            background_color: Color::Rgba(52.0 / 255.0, 52.0 / 255.0, 52.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(ratio_config));

        // Initialize the tidal widget
        let tidal_config = TelemetryWidgetConfig {
            title: "Tidal volume",
            value: "0".to_string(), //TODO
            unit: "mL (milliliters)",
            ids: (
                self.ids.tidal_parent,
                self.ids.tidal_title,
                self.ids.tidal_value,
                self.ids.tidal_unit,
            ),
            x_position: last_widget_position,
            y_position: 0.0,
            background_color: Color::Rgba(52.0 / 255.0, 52.0 / 255.0, 52.0 / 255.0, 1.0),
        };

        self.widgets
            .render(ControlWidgetType::Telemetry(tidal_config));
    }
}
