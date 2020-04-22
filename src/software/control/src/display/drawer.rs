// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use crate::config::environment::DISPLAY_WINDOW_SIZE_WIDTH;
use gfx_core::Resources;
use itertools::Itertools;
use piston_window::image::Image;
use piston_window::types::Color;
use piston_window::{
    clear, ellipse, line, text, Event, Glyphs, Line, PistonWindow, Rectangle, Text, Texture,
    TextureContext, TextureSettings, Transformed,
};
use std::path::{Path, PathBuf};

static WHITE: [f32; 4] = [1.0; 4];
static LIGHT_WHITE: [f32; 4] = [1.0, 1.0, 1.0, 0.5];
static LIGHT_BLUE: [f32; 4] = [0.5, 0.5, 1.0, 1.0];
static SKY_BLUE: [f32; 4] = [0.0, 0.8, 1.0, 1.0];
static LIGHT_YELLOW: [f32; 4] = [1.0, 1.0, 0.5, 1.0];
static LIGHT_PURPLE: [f32; 4] = [0.5, 1.0, 1.0, 1.0];
static PURPLE: [f32; 4] = [0.5, 0.0, 0.5, 1.0];

static BOX_H: f64 = 70.0;
static BOX_Y: f64 = 400.0;
static BOX_INTER_X: f64 = 20.0;
static BOX_W: f64 = ((DISPLAY_WINDOW_SIZE_WIDTH as f64) - 7.0 * BOX_INTER_X) / 6.0;

struct DisplayDrawerLoaderBuilder;

pub struct DisplayDrawerLoader {
    // pub top_logo: (Image, Texture<Texture as Resources::Texture>),
}

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    pub loader: DisplayDrawerLoader,
}

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
    pub fn new() -> DisplayDrawer {
        DisplayDrawer {
            loader: DisplayDrawerLoaderBuilder::new(),
        }
    }
}

impl DisplayDrawer {
    pub fn cycle(&mut self, window: &mut PistonWindow, event: &Event) {
        let mut glyphs = Glyphs::from_bytes(
            include_bytes!("../../res/fonts/FiraSans-Regular.ttf"),
            TextureContext {
                factory: window.factory.clone(),
                encoder: window.factory.create_command_buffer().into(),
            },
            TextureSettings::new(),
        )
        .unwrap();

        window.draw_2d(event, |c, g, device| {
            // Clear window and set a black background
            clear([0.0; 4], g);

            {
                let mut horizontal_dashed =
                    |x1: f64,
                     x2: f64,
                     y: f64,
                     color: Color,
                     left: &str,
                     right1: &str,
                     right2: &str| {
                        let a = chart_point([x1, y]);
                        let b = chart_point([x2, y]);

                        text(
                            WHITE,
                            18,
                            left,
                            &mut glyphs,
                            c.transform.trans(a[0] - 30.0, a[1] + 7.0),
                            g,
                        )
                        .unwrap();
                        text(
                            WHITE,
                            24,
                            right1,
                            &mut glyphs,
                            c.transform.trans(b[0] + 10.0, a[1] - 5.0),
                            g,
                        )
                        .unwrap();
                        text(
                            WHITE,
                            18,
                            right2,
                            &mut glyphs,
                            c.transform.trans(b[0] + 10.0, a[1] + 17.0),
                            g,
                        )
                        .unwrap();

                        for x in ((a[0] as u32)..(b[0] as u32)).step_by(15) {
                            let x = x as f64;
                            line(color, 1.0, [x, a[1], x + 10.0, a[1]], c.transform, g);
                        }
                    };

                horizontal_dashed(0.0, 1.0, 0.4, LIGHT_BLUE, "40", "P(peak)", "target");
                horizontal_dashed(0.0, 1.0, 0.25, LIGHT_PURPLE, "25", "P(plateau)", "target");
                horizontal_dashed(0.0, 1.0, 0.08, LIGHT_YELLOW, "8", "P(expiratory)", "target");
            }

            {
                let mut box_x = 0.0;
                let mut bottom_box = |color: Color, title: &str, center: &str, bottom: &str| {
                    box_x += BOX_INTER_X;
                    Rectangle::new_round(color, 5.0).draw(
                        [box_x, BOX_Y, BOX_W, BOX_H],
                        &c.draw_state,
                        c.transform,
                        g,
                    );

                    text(
                        WHITE,
                        12,
                        title,
                        &mut glyphs,
                        c.transform.trans(box_x + 7.0, BOX_Y + 17.0),
                        g,
                    )
                    .unwrap();
                    text(
                        WHITE,
                        22,
                        center,
                        &mut glyphs,
                        c.transform.trans(box_x + 7.0, BOX_Y + 45.0),
                        g,
                    )
                    .unwrap();
                    text(
                        LIGHT_WHITE,
                        12,
                        bottom,
                        &mut glyphs,
                        c.transform.trans(box_x + 7.0, BOX_Y + BOX_H - 5.0),
                        g,
                    )
                    .unwrap();

                    box_x += BOX_W;
                };

                bottom_box(PURPLE, "P(peak)", "32 <- (35)", "cmH2O");
                bottom_box(PURPLE, "P(plateau)", "23 <- (35)", "cmH2O");
                bottom_box(PURPLE, "P(expiratory)", "7 <- (35)", "cmH2O");
                bottom_box(PURPLE, "Cycles/minutes", "12", "/minute");
                bottom_box(PURPLE, "Insp-exp ratio", "1:2", "inspir.:expir.");
                bottom_box(PURPLE, "Tidal volume", "643", "mL (milliliters)");
            }

            {
                let curve_datas: Vec<[f64; 2]> = vec![
                    [0.0, 0.1],
                    [0.1, 0.1],
                    [0.1, 0.4],
                    [0.2, 0.4],
                    [0.2, 0.3],
                    [0.8, 0.3],
                    [0.8, 0.1],
                    [0.9, 0.1],
                ];

                for (a, b) in curve_datas.iter().tuple_windows() {
                    let a = chart_point(*a);
                    let b = chart_point(*b);

                    Line::new_round(SKY_BLUE, 3.0).draw(
                        [a[0], a[1], b[0], b[1]],
                        &c.draw_state,
                        c.transform,
                        g,
                    );
                }

                let last =
                    chart_point(*curve_datas.iter().last().unwrap_or_else(|| &[0.0f64, 0.0]));
                ellipse(
                    SKY_BLUE,
                    [last[0] - 10.0, last[1] - 10.0, 20.0, 20.0],
                    c.transform,
                    g,
                );
            }

            glyphs.factory.encoder.flush(device); // Update glyphs before rendering.
        });

        fn chart_point(p: [f64; 2]) -> [f64; 2] {
            [50.0 + p[0] * 550.0, BOX_Y - 5.0 - p[1] * 600.0]
        }
    }
}
