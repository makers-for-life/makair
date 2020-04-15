// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use piston_window::{Event, PistonWindow};

pub struct DisplayDrawerBuilder;
pub struct DisplayDrawer;

impl DisplayDrawerBuilder {
    pub fn new() -> DisplayDrawer {
        DisplayDrawer {}
    }
}

impl DisplayDrawer {
    pub fn cycle(&mut self, window: &mut PistonWindow, event: &Event) {
        window.draw_2d(event, |context, graphics, _device| {
            piston_window::clear([0.0; 4], graphics);

            // TODO
            // piston_window::rectangle(
            //     [1.0, 0.0, 0.0, 1.0], // red
            //     [0.0, 0.0, 100.0, 100.0],
            //     context.transform,
            //     graphics,
            // );
        });
    }
}
