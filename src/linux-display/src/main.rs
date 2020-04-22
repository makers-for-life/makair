#[macro_use]
extern crate log;
extern crate rand;
extern crate simple_logger;

extern crate glutin_window;
extern crate graphics;
extern crate opengl_graphics;
extern crate piston;

extern crate image;

use plotters::drawing::bitmap_pixel::BGRXPixel;
use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};
use log::{info, warn};
use std::{thread, time};

use std::collections::vec_deque::VecDeque;

use glutin_window::GlutinWindow as Window;
use graphics::draw_state::DrawState;
use graphics::rectangle;
use graphics::rectangle::rectangle_by_corners;
use graphics::{clear, Image};
use image::ImageBuffer;
use image::{Rgb, RgbaImage};
use opengl_graphics::{GlGraphics, OpenGL, Texture, TextureSettings};
use piston::event_loop::{EventSettings, Events};
use piston::input::{RenderArgs, RenderEvent, UpdateArgs, UpdateEvent};
use piston::window::WindowSettings;
use piston_window::PistonWindow;
use rand::Rng;
use std::path::Path;

pub struct App {
    gl: GlGraphics, // OpenGL drawing backend.
    rotation: f64,  // Rotation for the square.
    data: Vec<(DateTime<Local>, i32)>,
}

impl App {
    fn render(&mut self, args: &RenderArgs) {
        use crate::graphics::Transformed;
        info!("{}", self.data.len());

        const GREEN: [f32; 4] = [0.0, 1.0, 0.0, 1.0];
        const RED: [f32; 4] = [1.0, 0.0, 0.0, 1.0];

        let square = rectangle::square(0.0, 0.0, 50.0);
        let rotation = self.rotation;
        let (x, y) = (
            args.window_size[0] / 2.0,
            args.window_size[1] / 2.0 + args.window_size[1] / 4.0,
        );

        let mut buffer = vec![0; (780 * 200 * 4) as usize];
        let root = BitMapBackend::with_buffer(&mut buffer, (780, 200)).into_drawing_area();
        //let root = BitMapBackend::new(Path::new("/tmp/foo.png"), (780, 200)).into_drawing_area();
        root.fill(&WHITE);

        let oldest = self.data.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = self.data.first().unwrap().0;

        let mut chart = ChartBuilder::on(&root)
            .x_label_area_size(40)
            .y_label_area_size(40)
            .build_ranged(oldest..newest, 0..10)
            .unwrap();
        chart.configure_mesh().draw().unwrap();
        chart
            .draw_series(LineSeries::new(
                self.data.iter().map(|x| (x.0, x.1)),
                ShapeStyle::from(&BLACK).filled(),
            ))
            .unwrap();

        drop(chart);
        drop(root);
        let image_graph = Image::new().rect(rectangle_by_corners(0.0, 0.0, 780.0, 200.0));
        let texture = Texture::from_image(
            &RgbaImage::from_raw(780, 200, ((&mut buffer).to_vec())).unwrap(),
            &TextureSettings::new(),
        ); //(&mut buffer[..], 780, 200, &TextureSettings::new())
           //.unwrap();
           //          Texture::from_path(Path::new("/tmp/foo.png"), &TextureSettings::new()).unwrap();

        self.gl.draw(args.viewport(), |c, gl| {
            // Clear the screen.
            clear([0.0, 0.0, 0.0, 0.0], gl);

            let transform = c
                .transform
                .trans(x, y)
                .rot_rad(rotation)
                .trans(-25.0, -25.0);

            // Draw a box rotating around the middle of the screen.
            rectangle(RED, square, transform, gl);
            //image(&texture, c.transform, gl);
            image_graph.draw(&texture, &DrawState::default(), c.transform, gl);
        });
    }

    fn update(&mut self, args: &UpdateArgs) {
        // Rotate 2 radians per second.
        self.rotation += 2.0 * args.dt;
    }

    fn addFakeData(&mut self) {
        let mut rng = rand::thread_rng();

        self.data.insert(0, (Local::now(), rng.gen_range(0, 10)));
        let oldest = self.data.first().unwrap().0 - chrono::Duration::seconds(40);
        let newest = self.data.first().unwrap().0;
        let mut i = 0;
        while i != self.data.len() {
            if oldest > (&mut self.data[i]).0 || newest < (&mut self.data[i]).0 {
                let val = self.data.remove(i);
            } else {
                i += 1;
            }
        }
    }
}

fn main() {
    simple_logger::init().unwrap();
    /*
        let datas = Arc::new(Mutex::new(vec![(Local::now(), 0)]));
        let data1 = datas.clone();
    */
    let opengl = OpenGL::V3_2;

    let mut window: PistonWindow = WindowSettings::new("MakAir UI", [800, 480])
        .graphics_api(opengl)
        .exit_on_esc(true)
        .build()
        .unwrap_or_else(|e| panic!("Failed to build PistonWindow: {}", e));

    // Create a new game and run it.
    let app_core = Arc::new(Mutex::new(App {
        gl: GlGraphics::new(opengl),
        rotation: 0.0,
        data: vec![(Local::now(), 0)],
    }));

    let app1 = app_core.clone();
    thread::spawn(move || {
        let mut rng = rand::thread_rng();

        while true {
            app1.lock().unwrap().addFakeData();
            thread::sleep(time::Duration::from_millis(200));
        }
    });

    let app2 = app_core.clone();
    let mut events = Events::new(EventSettings::new());
    while let Some(e) = events.next(&mut window) {
        let mut app = app2.lock().unwrap();
        if let Some(args) = e.render_args() {
            app.render(&args);
        }

        if let Some(args) = e.update_args() {
            app.update(&args);
        }
    }
    //window.set_lazy(true);
    //let data2 = datas.clone();

    /*
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
    */
}
