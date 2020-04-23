// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::path::{Path, PathBuf};

use piston_window::image::Image;

pub struct DisplayLoaderBuilder;

pub struct DisplayLoader {
    // pub top_logo: (Image, Texture<Texture as Resources::Texture>),
}

impl DisplayLoaderBuilder {
    pub fn new() -> DisplayLoader {
        DisplayLoader {
            // TODO: improve loader code structure

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
