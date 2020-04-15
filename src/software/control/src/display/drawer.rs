// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use piston_window::image::Image;
use piston_window::Texture;
use piston_window::{Event, PistonWindow};
use gfx_core::Resources;

use std::path::{Path, PathBuf};

struct DisplayDrawerLoaderBuilder;
struct DisplayDrawerLoader {
    pub top_logo: (Image, Texture<Texture as Resources::Texture>),
}

pub struct DisplayDrawerBuilder;

pub struct DisplayDrawer {
    pub loader: DisplayDrawerLoader,
}

impl DisplayDrawerLoaderBuilder {
    fn new() -> DisplayDrawerLoader {
        DisplayDrawerLoader {
            // TODO
            top_logo: Self::load_top_logo(),
        }
    }

    fn acquire_path(name: &str) -> PathBuf {
        Path::new(&format!("./res/{}.png", name)).to_path_buf()
    }

    fn load_top_logo() -> (Image, Texture<Texture as Resources::Texture>) {
        (
            // TODO: proper size & position
            Image::new().rect(piston_window::rectangle::square(0.0, 0.0, 200.0)),
            // TODO: acquire path from fn
            Texture::from_path(Self::acquire_path("top-logo")).unwrap(),
        )
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
        window.draw_2d(event, |context, graphics, _device| {
            // Clear window and set a black background
            piston_window::clear([0.0; 4], graphics);

            // TODO
            // piston_window::rectangle(
            //     [1.0, 0.0, 0.0, 1.0], // red
            //     [0.0, 0.0, 100.0, 100.0],
            //     context.transform,
            //     graphics,
            // );

            // Draw top logo
            // TODO
            self.loader.top_logo.0.draw(
                &self.loader.top_logo.1,
                graphics.default_draw_state(),
                context.transform,
                graphics,
            );
        });
    }
}
