// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::{color, widget, Colorable, Positionable, Sizeable, Widget};
use conrod_core::widget::Id as WidgetId;
use telemetry::structures::MachineStateSnapshot;
use super::fonts::Fonts;


pub type WidgetIds = (WidgetId, WidgetId, WidgetId, WidgetId);

widget_ids!(pub struct Ids {
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
  peep_unit
});

pub fn create_widgets(ui: &mut conrod_core::UiCell,
  ids: Ids,
  image: conrod_core::image::Id,
  (w, h): (u32, u32),
  fonts: &Fonts,
  machine_snapshot: &MachineStateSnapshot
) {
  widget::Canvas::new()
      .color(color::BLACK)
      .set(ids.background, ui);
  // Instantiate the `Image` at its full size in the middle of the window.
  widget::Image::new(image)
      .w_h(w as f64, h as f64)
      .mid_top_with_margin(10.0)
      .set(ids.pressure_graph, ui);

  let mut cycles_position = 0.0;

  cycles_position = create_widget(ui,
      "P(peak)",
      &format!("{} <- ({})", machine_snapshot.previous_peak_pressure, machine_snapshot.peak_command),
      "cmH20",
      fonts,
      (
          ids.peak_parent,
          ids.peak_title,
          ids.peak_value,
          ids.peak_unit
      ),
     cycles_position,
     10.0,
     conrod_core::color::Color::Rgba(39.0 / 255.0, 66.0 / 255.0, 100.0 / 255.0, 1.0)
  );

  cycles_position = create_widget(ui,
      "P(plateau)",
      &format!("{} <- ({})", (machine_snapshot.previous_plateau_pressure as f64 / 10 as f64).round(), (machine_snapshot.plateau_command as f64 / 10 as f64).round()),
      "cmH20",
      fonts,
      (
          ids.plateau_parent,
          ids.plateau_title,
          ids.plateau_value,
          ids.plateau_unit
      ),
      cycles_position,
      0.0,
      conrod_core::color::Color::Rgba(66.0 / 255.0, 44.0 / 255.0, 85.0 / 255.0, 1.0)
  );

  cycles_position = create_widget(ui,
      "P(expiratory)",
      &format!("{} <- ({})", (machine_snapshot.previous_peep_pressure as f64 / 10 as f64).round(), (machine_snapshot.peep_command as f64 / 10 as f64).round()),
      "cmH20",
      fonts,
      (
          ids.peep_parent,
          ids.peep_title,
          ids.peep_value,
          ids.peep_unit
      ),
      cycles_position,
      0.0,
      conrod_core::color::Color::Rgba(76.0 / 255.0, 73.0 / 255.0, 25.0 / 255.0, 1.0)
  );

  create_widget(ui,
      "Cycles/minute",
      &format!("{}", machine_snapshot.cpm_command),
      "/minute",
      fonts,
      (
          ids.cycles_parent,
          ids.cycles_title,
          ids.cycles_value,
          ids.cycles_unit
      ),
      cycles_position,
      0.0,
      conrod_core::color::Color::Rgba(47.0 / 255.0, 74.0 / 255.0, 16.0 / 255.0, 1.0)
  );
}


pub fn create_widget(ui: &mut conrod_core::UiCell, title: &str, value: &str, unit: &str,
  fonts: &Fonts,
  ids: WidgetIds,
  x_position: f64, y_position: f64,
  background_color: conrod_core::color::Color) -> f64 {
  let parent = widget::Canvas::new()
      .color(background_color)
      .w_h(120.0, 75.0)
      .bottom_left_with_margins(y_position, x_position + 10.0);

  parent.set(ids.0, ui);

  widget::Text::new(title)
      .color(color::WHITE)
      .top_left_with_margins_on(ids.0, 10.0, 20.0)
      .font_size(12)
      .set(ids.1, ui);

  let mut value_style = conrod_core::widget::primitive::text::Style::default();
  value_style.font_id = Some(Some(fonts.bold));
  value_style.color = Some(color::WHITE);
  value_style.font_size = Some(14);
  widget::Text::new(value)
      .with_style(value_style)
      .mid_left_with_margin_on(ids.0, 20.0)
      .set(ids.2, ui);

  widget::Text::new(unit)
      .color(color::WHITE)
      .bottom_left_with_margins_on(ids.0, 15.0, 20.0)
      .font_size(10)
      .set(ids.3, ui);

  10.0 + 120.0
}