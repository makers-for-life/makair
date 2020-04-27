// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_core::widget::Id as WidgetId;
use conrod_core::{
    color::{self, Color},
    widget, Colorable, Positionable, Sizeable, Widget,
};

use crate::config::environment::{
    BRANDING_IMAGE_MARGIN_LEFT, BRANDING_IMAGE_MARGIN_TOP, BRANDING_TEXT_MARGIN_LEFT,
    BRANDING_TEXT_MARGIN_TOP, DISPLAY_WIDGET_SIZE_HEIGHT, DISPLAY_WIDGET_SIZE_SPACING,
    DISPLAY_WIDGET_SIZE_WIDTH, GRAPH_DRAW_SPACING_FROM_BOTTOM,
};

use super::fonts::Fonts;

pub struct BackgroundWidgetConfig {
    color: conrod_core::color::Color,
    id: WidgetId,
}

impl BackgroundWidgetConfig {
    pub fn new(color: conrod_core::color::Color, id: WidgetId) -> BackgroundWidgetConfig {
        BackgroundWidgetConfig { color, id }
    }
}

pub struct BrandingWidgetConfig<'a> {
    version_firmware: &'a str,
    version_control: &'a str,
    width: f64,
    height: f64,
    image: conrod_core::image::Id,
    pub ids: (WidgetId, WidgetId),
}

pub struct TelemetryWidgetConfig<'a> {
    pub title: &'a str,
    pub value: String,
    pub unit: &'a str,
    pub ids: (WidgetId, WidgetId, WidgetId, WidgetId),
    pub x_position: f64,
    pub y_position: f64,
    pub background_color: Color,
}

pub struct GraphWidgetConfig {
    width: f64,
    height: f64,
    image: conrod_core::image::Id,
    id: WidgetId,
}

impl<'a> BrandingWidgetConfig<'a> {
    pub fn new(
        version_firmware: &'a str,
        version_control: &'a str,
        width: f64,
        height: f64,
        image: conrod_core::image::Id,
        ids: (WidgetId, WidgetId),
    ) -> BrandingWidgetConfig<'a> {
        BrandingWidgetConfig {
            version_firmware,
            version_control,
            width,
            height,
            image,
            ids,
        }
    }
}

impl GraphWidgetConfig {
    pub fn new(
        width: f64,
        height: f64,
        image: conrod_core::image::Id,
        id: WidgetId,
    ) -> GraphWidgetConfig {
        GraphWidgetConfig {
            width,
            height,
            image,
            id,
        }
    }
}

pub struct ErrorWidgetConfig {
    error: String,
    id: WidgetId,
}

impl ErrorWidgetConfig {
    pub fn new(error: String, id: WidgetId) -> ErrorWidgetConfig {
        ErrorWidgetConfig { error, id }
    }
}

pub struct StopWidgetConfig {
    id: WidgetId,
}

impl StopWidgetConfig {
    pub fn new(id: WidgetId) -> StopWidgetConfig {
        StopWidgetConfig { id }
    }
}

pub struct NoDataWidgetConfig {
    id: WidgetId,
}

impl NoDataWidgetConfig {
    pub fn new(id: WidgetId) -> NoDataWidgetConfig {
        NoDataWidgetConfig { id }
    }
}

pub struct InitializingWidgetConfig {
    id: WidgetId,
}

impl InitializingWidgetConfig {
    pub fn new(id: WidgetId) -> InitializingWidgetConfig {
        InitializingWidgetConfig { id }
    }
}

pub enum ControlWidgetType<'a> {
    Background(BackgroundWidgetConfig),
    Error(ErrorWidgetConfig),
    Branding(BrandingWidgetConfig<'a>),
    Initializing(InitializingWidgetConfig),
    Graph(GraphWidgetConfig),
    NoData(NoDataWidgetConfig),
    Stop(StopWidgetConfig),
    Telemetry(TelemetryWidgetConfig<'a>),
}

pub struct ControlWidget<'a> {
    ui: conrod_core::UiCell<'a>,
    fonts: &'a Fonts,
}

impl<'a> ControlWidget<'a> {
    pub fn new(ui: conrod_core::UiCell<'a>, fonts: &'a Fonts) -> ControlWidget<'a> {
        ControlWidget { ui, fonts }
    }

    pub fn render(&mut self, widget_type: ControlWidgetType<'a>) -> f64 {
        match widget_type {
            ControlWidgetType::Background(config) => self.background(config),
            ControlWidgetType::Error(config) => self.error(config),
            ControlWidgetType::Branding(config) => self.branding(config),
            ControlWidgetType::Initializing(config) => self.initializing(config),
            ControlWidgetType::Graph(config) => self.graph(config),
            ControlWidgetType::NoData(config) => self.no_data(config),
            ControlWidgetType::Stop(config) => self.stop(config),
            ControlWidgetType::Telemetry(config) => self.telemetry_widget(config),
        }
    }

    fn background(&mut self, config: BackgroundWidgetConfig) -> f64 {
        widget::Canvas::new()
            .color(config.color)
            .set(config.id, &mut self.ui);

        0 as _
    }

    fn branding(&mut self, config: BrandingWidgetConfig) -> f64 {
        // Display branding image
        widget::Image::new(config.image)
            .w_h(config.width, config.height)
            .top_left_with_margins(BRANDING_IMAGE_MARGIN_TOP, BRANDING_IMAGE_MARGIN_LEFT)
            .set(config.ids.0, &mut self.ui);

        // Display branding text
        let branding_text = format!("F{} | C{}", config.version_firmware, config.version_control);

        widget::Text::new(&branding_text)
            .color(color::WHITE.with_alpha(0.45))
            .top_left_with_margins(BRANDING_TEXT_MARGIN_TOP, BRANDING_TEXT_MARGIN_LEFT)
            .font_size(10)
            .set(config.ids.1, &mut self.ui);

        config.width
    }

    fn graph(&mut self, config: GraphWidgetConfig) -> f64 {
        widget::Image::new(config.image)
            .w_h(config.width, config.height)
            .mid_bottom_with_margin(GRAPH_DRAW_SPACING_FROM_BOTTOM)
            .set(config.id, &mut self.ui);

        config.width
    }

    #[allow(clippy::too_many_arguments)]
    fn create_bottom_left_rounded_rectangle(
        &mut self,
        width: f64,
        height: f64,
        round: f64,
        background_color: Option<Color>,
        id: WidgetId,
        x_position: f64,
        y_position: f64,
    ) {
        let parent_fill = widget::primitive::shape::Style::Fill(background_color);
        let parent_dimensions = [width, height];

        let parent = widget::rounded_rectangle::RoundedRectangle::styled(
            parent_dimensions,
            round,
            parent_fill,
        )
        .bottom_left_with_margins(y_position, x_position);

        parent.set(id, &mut self.ui);
    }

    fn telemetry_widget(&mut self, config: TelemetryWidgetConfig) -> f64 {
        self.create_bottom_left_rounded_rectangle(
            DISPLAY_WIDGET_SIZE_WIDTH,
            DISPLAY_WIDGET_SIZE_HEIGHT,
            2.5,
            Some(config.background_color),
            config.ids.0,
            config.x_position + DISPLAY_WIDGET_SIZE_SPACING,
            config.y_position,
        );

        widget::Text::new(config.title)
            .color(color::WHITE)
            .top_left_with_margins_on(config.ids.0, 10.0, 20.0)
            .font_size(11)
            .set(config.ids.1, &mut self.ui);

        let mut value_style = conrod_core::widget::primitive::text::Style::default();

        value_style.font_id = Some(Some(self.fonts.bold));
        value_style.color = Some(color::WHITE);
        value_style.font_size = Some(14);

        widget::Text::new(&config.value)
            .with_style(value_style)
            .mid_left_with_margin_on(config.ids.0, 20.0)
            .font_size(17)
            .set(config.ids.2, &mut self.ui);

        widget::Text::new(config.unit)
            .color(color::WHITE.with_alpha(0.2))
            .bottom_left_with_margins_on(config.ids.0, 12.0, 20.0)
            .font_size(11)
            .set(config.ids.3, &mut self.ui);

        DISPLAY_WIDGET_SIZE_WIDTH
    }

    fn error(&mut self, config: ErrorWidgetConfig) -> f64 {
        let mut text_style = conrod_core::widget::primitive::text::Style::default();

        text_style.font_id = Some(Some(self.fonts.bold));
        text_style.color = Some(color::WHITE);
        text_style.font_size = Some(30);

        widget::Text::new(&format!("An error happened:\n{}", config.error)) // using \n instead of the wrap methods because I couldn't make them work
            .color(color::WHITE)
            .align_top() // Aligned to top otherwise I can't make the line breaks work
            .with_style(text_style)
            .set(config.id, &mut self.ui);

        0 as _
    }

    fn stop(&mut self, config: StopWidgetConfig) -> f64 {
        let mut text_style = conrod_core::widget::primitive::text::Style::default();

        text_style.font_id = Some(Some(self.fonts.bold));
        text_style.color = Some(color::WHITE);
        text_style.font_size = Some(30);

        widget::Text::new("Press start to begin")
            .color(color::WHITE)
            .middle()
            .with_style(text_style)
            .set(config.id, &mut self.ui);

        0 as _
    }

    fn no_data(&mut self, config: NoDataWidgetConfig) -> f64 {
        let mut text_style = conrod_core::widget::primitive::text::Style::default();

        text_style.font_id = Some(Some(self.fonts.bold));
        text_style.color = Some(color::WHITE);
        text_style.font_size = Some(30);

        widget::Text::new("Device disconnected or no data received")
            .color(color::WHITE)
            .middle()
            .with_style(text_style)
            .set(config.id, &mut self.ui);
        0 as _
    }

    fn initializing(&mut self, config: InitializingWidgetConfig) -> f64 {
        let mut text_style = conrod_core::widget::primitive::text::Style::default();

        text_style.font_id = Some(Some(self.fonts.bold));
        text_style.color = Some(color::WHITE);
        text_style.font_size = Some(30);

        widget::Text::new("Initialization..")
            .color(color::WHITE)
            .middle()
            .with_style(text_style)
            .set(config.id, &mut self.ui);
        0 as _
    }
}
