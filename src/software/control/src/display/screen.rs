// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::color::{self, Color};

use telemetry::alarm::AlarmCode;
use telemetry::structures::{AlarmPriority, MachineStateSnapshot};

use crate::config::environment::*;

use super::fonts::Fonts;
use super::widget::{
    AlarmsWidgetConfig, BackgroundWidgetConfig, BrandingWidgetConfig, ControlWidget,
    ControlWidgetType, ErrorWidgetConfig, GraphWidgetConfig, HeartbeatWidgetConfig,
    InitializingWidgetConfig, NoDataWidgetConfig, StatusWidgetConfig, StopWidgetConfig,
    TelemetryWidgetConfig,
};

widget_ids!(pub struct Ids {
  alarm_container,
  alarm_title,
  alarm_empty,
  alarm_alarms[],
  alarm_codes_containers[],
  alarm_codes[],
  alarm_messages_containers[],
  alarm_messages[],

  background,

  branding,
  pressure_graph,

  branding_image,
  branding_text,

  status_wrapper,
  status_unit_box,
  status_unit_text,
  status_power_box,
  status_power_text,

  heartbeat_ground,
  heartbeat_surround,
  heartbeat_inner,

  cycles_parent,
  cycles_title,
  cycles_value_measured,
  cycles_value_arrow,
  cycles_value_target,
  cycles_unit,

  peak_parent,
  peak_title,
  peak_value_measured,
  peak_value_arrow,
  peak_value_target,
  peak_unit,

  plateau_parent,
  plateau_title,
  plateau_value_measured,
  plateau_value_arrow,
  plateau_value_target,
  plateau_unit,

  peep_parent,
  peep_title,
  peep_value_measured,
  peep_value_arrow,
  peep_value_target,
  peep_unit,

  ratio_parent,
  ratio_title,
  ratio_value_measured,
  ratio_value_arrow,
  ratio_value_target,
  ratio_unit,

  tidal_parent,
  tidal_title,
  tidal_value_measured,
  tidal_value_arrow,
  tidal_value_target,
  tidal_unit,

  stopped_background,
  stopped_container_borders,
  stopped_container,
  stopped_title,
  stopped_message,

  no_data,
  error,

  initializing_logo,
});

pub struct Screen<'a> {
    ids: &'a Ids,
    machine_snapshot: Option<&'a MachineStateSnapshot>,
    ongoing_alarms: Option<&'a [(&'a AlarmCode, &'a AlarmPriority)]>,
    widgets: ControlWidget<'a>,
}

pub struct ScreenDataBranding<'a> {
    pub firmware_version: &'a str,
    pub image_id: conrod_core::image::Id,
    pub width: f64,
    pub height: f64,
}

pub struct ScreenDataGraph {
    pub image_id: conrod_core::image::Id,
    pub width: f64,
    pub height: f64,
}

pub struct ScreenDataTelemetry {
    pub arrow_image_id: conrod_core::image::Id,
}

pub struct ScreenBootLoader {
    pub image_id: conrod_core::image::Id,
    pub width: f64,
    pub height: f64,
}

impl<'a> Screen<'a> {
    pub fn new(
        ui: conrod_core::UiCell<'a>,
        ids: &'a Ids,
        fonts: &'a Fonts,
        machine_snapshot: Option<&'a MachineStateSnapshot>,
        ongoing_alarms: Option<&'a [(&'a AlarmCode, &'a AlarmPriority)]>,
    ) -> Screen<'a> {
        Screen {
            ids,
            machine_snapshot,
            ongoing_alarms,
            widgets: ControlWidget::new(ui, fonts),
        }
    }

    pub fn render_with_data(
        &mut self,
        branding_data: ScreenDataBranding<'a>,
        graph_data: ScreenDataGraph,
        telemetry_data: ScreenDataTelemetry,
    ) {
        // Render common background
        self.render_background();

        // Render top elements
        self.render_branding(
            branding_data.firmware_version,
            RUNTIME_VERSION,
            branding_data.image_id,
            branding_data.width,
            branding_data.height,
        );
        self.render_alarms();
        self.render_status();
        self.render_heartbeat();

        // Render middle elements
        self.render_graph(graph_data.image_id, graph_data.width, graph_data.height);

        // Render bottom elements
        self.render_telemetry(telemetry_data);
    }

    pub fn render_background(&mut self) {
        let config = BackgroundWidgetConfig::new(color::BLACK, self.ids.background);

        self.widgets.render(ControlWidgetType::Background(config));
    }

    pub fn render_branding(
        &mut self,
        version_firmware: &'a str,
        version_control: &'a str,
        image_id: conrod_core::image::Id,
        width: f64,
        height: f64,
    ) {
        let config = BrandingWidgetConfig::new(
            version_firmware,
            version_control,
            width,
            height,
            image_id,
            (self.ids.branding_image, self.ids.branding_text),
        );

        self.widgets.render(ControlWidgetType::Branding(config));
    }

    pub fn render_alarms(&mut self) {
        let config = AlarmsWidgetConfig {
            parent: self.ids.background,
            container: self.ids.alarm_container,
            title: self.ids.alarm_title,
            empty: self.ids.alarm_empty,
            alarm_widgets: &self.ids.alarm_alarms,
            alarm_codes_containers: &self.ids.alarm_codes_containers,
            alarm_codes: &self.ids.alarm_codes,
            alarm_messages_containers: &self.ids.alarm_messages_containers,
            alarm_messages: &self.ids.alarm_messages,
            alarms: self.ongoing_alarms.unwrap(),
        };

        self.widgets.render(ControlWidgetType::Alarms(config));
    }

    pub fn render_status(&mut self) {
        let config = StatusWidgetConfig::new(
            self.ids.background,
            self.ids.status_wrapper,
            self.ids.status_unit_box,
            self.ids.status_unit_text,
            self.ids.status_power_box,
            self.ids.status_power_text,
        );

        self.widgets.render(ControlWidgetType::Status(config));
    }

    pub fn render_heartbeat(&mut self) {
        let config = HeartbeatWidgetConfig::new(
            self.ids.background,
            self.ids.heartbeat_ground,
            self.ids.heartbeat_surround,
            self.ids.heartbeat_inner,
        );

        self.widgets.render(ControlWidgetType::Heartbeat(config));
    }

    pub fn render_graph(&mut self, image_id: conrod_core::image::Id, width: f64, height: f64) {
        let config = GraphWidgetConfig::new(
            width,
            height,
            image_id,
            self.ids.background,
            self.ids.pressure_graph,
        );

        self.widgets.render(ControlWidgetType::Graph(config));
    }

    pub fn render_stop(
        &mut self,
        branding_data: ScreenDataBranding<'a>,
        graph_data: ScreenDataGraph,
        telemetry_data: ScreenDataTelemetry,
    ) {
        self.render_with_data(branding_data, graph_data, telemetry_data);

        let config = StopWidgetConfig {
            parent: self.ids.background,
            background: self.ids.stopped_background,
            container_borders: self.ids.stopped_container_borders,
            container: self.ids.stopped_container,
            title: self.ids.stopped_title,
            message: self.ids.stopped_message,
        };

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

    pub fn render_initializing(&mut self, config: ScreenBootLoader) {
        let config = InitializingWidgetConfig::new(
            self.ids.initializing_logo,
            config.width,
            config.height,
            config.image_id,
        );

        self.render_background();

        self.widgets.render(ControlWidgetType::Initializing(config));
    }

    pub fn render_telemetry(&mut self, telemetry_data: ScreenDataTelemetry) {
        let mut last_widget_position = 0.0;
        let machine_snapshot = self.machine_snapshot.unwrap();

        let peak_config = TelemetryWidgetConfig {
            title: "P(peak)",
            value_measured: Some(
                (machine_snapshot.previous_peak_pressure as f64 / 10.0)
                    .round()
                    .to_string(),
            ),
            value_target: Some(machine_snapshot.peak_command.to_string()),
            value_arrow: telemetry_data.arrow_image_id,
            unit: "cmH20",
            ids: (
                self.ids.background,
                self.ids.peak_parent,
                self.ids.peak_title,
                self.ids.peak_value_measured,
                self.ids.peak_value_arrow,
                self.ids.peak_value_target,
                self.ids.peak_unit,
            ),
            x_position: last_widget_position,
            y_position: TELEMETRY_WIDGET_SPACING_FROM_BOTTOM,
            background_color: Color::Rgba(39.0 / 255.0, 66.0 / 255.0, 100.0 / 255.0, 1.0),
        };

        last_widget_position = self
            .widgets
            .render(ControlWidgetType::Telemetry(peak_config));

        // Initialize the plateau widget
        let plateau_config = TelemetryWidgetConfig {
            title: "P(plateau)",
            value_measured: Some(
                (machine_snapshot.previous_plateau_pressure as f64 / 10.0)
                    .round()
                    .to_string(),
            ),
            value_target: Some(machine_snapshot.plateau_command.to_string()),
            value_arrow: telemetry_data.arrow_image_id,
            unit: "cmH20",
            ids: (
                self.ids.peak_parent,
                self.ids.plateau_parent,
                self.ids.plateau_title,
                self.ids.plateau_value_measured,
                self.ids.plateau_value_arrow,
                self.ids.plateau_value_target,
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
            value_measured: Some(
                (machine_snapshot.previous_peep_pressure as f64 / 10.0)
                    .round()
                    .to_string(),
            ),
            value_target: Some(machine_snapshot.peep_command.to_string()),
            value_arrow: telemetry_data.arrow_image_id,
            unit: "cmH20",
            ids: (
                self.ids.plateau_parent,
                self.ids.peep_parent,
                self.ids.peep_title,
                self.ids.peep_value_measured,
                self.ids.peep_value_arrow,
                self.ids.peep_value_target,
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
            value_measured: None,
            value_target: Some(machine_snapshot.cpm_command.to_string()),
            value_arrow: telemetry_data.arrow_image_id,
            unit: "/minute",
            ids: (
                self.ids.peep_parent,
                self.ids.cycles_parent,
                self.ids.cycles_title,
                self.ids.cycles_value_measured,
                self.ids.cycles_value_arrow,
                self.ids.cycles_value_target,
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
            value_measured: None,
            value_target: Some("0:0".to_string()),
            value_arrow: telemetry_data.arrow_image_id,
            unit: "insp:exp.",
            ids: (
                self.ids.cycles_parent,
                self.ids.ratio_parent,
                self.ids.ratio_title,
                self.ids.ratio_value_measured,
                self.ids.ratio_value_arrow,
                self.ids.ratio_value_target,
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
            value_measured: Some("0".to_string()),
            value_target: None,
            value_arrow: telemetry_data.arrow_image_id,
            unit: "mL (milliliters)",
            ids: (
                self.ids.ratio_parent,
                self.ids.tidal_parent,
                self.ids.tidal_title,
                self.ids.tidal_value_measured,
                self.ids.tidal_value_arrow,
                self.ids.tidal_value_target,
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
