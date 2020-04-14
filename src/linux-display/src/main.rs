#[macro_use]
extern crate log;
extern crate rand;
extern crate simple_logger;

use piston_window::*;
use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};
use log::{info, warn};
use std::thread;

use std::collections::vec_deque::VecDeque;

use rand::Rng;

const FPS: u32 = 4;
const LENGTH: u32 = 20;
const N_DATA_POINTS: usize = (FPS * LENGTH) as usize;

fn main() {
    simple_logger::init().unwrap();
    info!("plop");

    let datas = Arc::new(Mutex::new(vec![(Local::now(), 0)]));

    let d = datas.clone();

    let mut window: PistonWindow = WindowSettings::new("MakAir UI", [800, 480])
        .exit_on_esc(true)
        .build()
        .unwrap_or_else(|e| panic!("Failed to build PistonWindow: {}", e));
    window.set_max_fps(FPS as u64);
    //window.set_lazy(true);

    //   while let Some(_) = draw_piston_window(&mut window, |b| {
    while let Some(e) = window.next() {
        window.draw_2d(&e, |c, g, _| {
            rectangle(
                [1.0, 0.0, 0.0, 1.0],
                [350.0, 350.0, 100.0, 100.0],
                c.transform,
                g,
            );
        });
        draw_piston_window(&mut window, |b| {
            let root = b.into_drawing_area().shrink((5, 5), (780, 200));
            root.fill(&WHITE);
            let mut rng = rand::thread_rng();
            let mut data = d.lock().unwrap();
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

            let mut chart = ChartBuilder::on(&root)
                .x_label_area_size(40)
                .y_label_area_size(40)
                .build_ranged(oldest..newest, 0..10)?;
            chart.configure_mesh().draw()?;
            chart.draw_series(LineSeries::new(data.iter().map(|x| (x.0, x.1)), &GREEN))?;

            Ok(())
        });
    }
}
