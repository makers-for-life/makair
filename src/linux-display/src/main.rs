#[macro_use]
extern crate log;
extern crate rand;
extern crate simple_logger;

extern crate glutin_window;
extern crate graphics;
extern crate opengl_graphics;
extern crate piston;

use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};
use log::{info, warn};
use std::{thread, time};

use std::collections::vec_deque::VecDeque;

use glutin_window::GlutinWindow as Window;
use graphics::rectangle;
use opengl_graphics::{GlGraphics, OpenGL};
use piston::event_loop::{EventSettings, Events};
use piston::input::{RenderArgs, RenderEvent, UpdateArgs, UpdateEvent};
use piston::window::WindowSettings;
use piston_window::PistonWindow;

use rand::Rng;

pub struct App {
    gl: GlGraphics, // OpenGL drawing backend.
    rotation: f64,  // Rotation for the square.
}

impl App {
    fn render(&mut self, args: &RenderArgs) {
        use graphics::*;

        const GREEN: [f32; 4] = [0.0, 1.0, 0.0, 1.0];
        const RED: [f32; 4] = [1.0, 0.0, 0.0, 1.0];

        let square = rectangle::square(0.0, 0.0, 50.0);
        let rotation = self.rotation;
        let (x, y) = (
            args.window_size[0] / 2.0,
            args.window_size[1] / 2.0 + args.window_size[1] / 4.0,
        );

        self.gl.draw(args.viewport(), |c, gl| {
            // Clear the screen.

            let transform = c
                .transform
                .trans(x, y)
                .rot_rad(rotation)
                .trans(-25.0, -25.0);

            // Draw a box rotating around the middle of the screen.
            rectangle(RED, square, transform, gl);
        });
    }

    fn update(&mut self, args: &UpdateArgs) {
        // Rotate 2 radians per second.
        self.rotation += 2.0 * args.dt;
    }
}

fn main() {
    simple_logger::init().unwrap();

    let datas = Arc::new(Mutex::new(vec![(Local::now(), 0)]));
    let data1 = datas.clone();

    thread::spawn(move || {
        let mut rng = rand::thread_rng();

        while true {
            let mut data = datas.lock().unwrap();

            data.insert(0, (Local::now(), rng.gen_range(0, 10)));
            let oldest = data.first().unwrap().0 - chrono::Duration::seconds(40);
            let newest = data.first().unwrap().0;
            let mut i = 0;
            while i != data.len() {
                if oldest > (&mut data[i]).0 || newest < (&mut data[i]).0 {
                    let val = data.remove(i);
                } else {
                    i += 1;
                }
            }
            thread::sleep(time::Duration::from_millis(500));
        }
    });
    let opengl = OpenGL::V3_2;

    let mut window: PistonWindow = WindowSettings::new("MakAir UI", [800, 480])
        .graphics_api(opengl)
        .exit_on_esc(true)
        .build()
        .unwrap_or_else(|e| panic!("Failed to build PistonWindow: {}", e));

    // Create a new game and run it.
    let mut app = App {
        gl: GlGraphics::new(opengl),
        rotation: 0.0,
    };
    /*
    let mut events = Events::new(EventSettings::new());
    while let Some(e) = events.next(&mut window) {
        if let Some(args) = e.render_args() {
            app.render(&args);
        }

        if let Some(args) = e.update_args() {
            app.update(&args);
        }
    }
    */
    //window.set_lazy(true);
    //let data2 = datas.clone();
    while let Some(_) = draw_piston_window(&mut window, |b| {
        //while let Some(e) = window.next() {

        let root = b.into_drawing_area().shrink((5, 5), (780, 200));
        root.fill(&WHITE);
        let mut data = data1.lock().unwrap();

        let oldest = data.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = data.first().unwrap().0;

        let mut chart = ChartBuilder::on(&root)
            .x_label_area_size(40)
            .y_label_area_size(40)
            .build_ranged(oldest..newest, 0..10)?;
        chart.configure_mesh().draw()?;
        chart.draw_series(LineSeries::new(data.iter().map(|x| (x.0, x.1)), &GREEN))?;

        Ok(())
    }) {}
}
