extern crate rand;
extern crate timer;

use piston_window::*;
use plotters::prelude::*;
use std::sync::{Arc, Mutex};

use chrono::offset::{Local, TimeZone};
use chrono::prelude::*;
use chrono::{Date, Duration};

use std::collections::vec_deque::VecDeque;

use rand::Rng;

const FPS: u32 = 10;
const LENGTH: u32 = 20;
const N_DATA_POINTS: usize = (FPS * LENGTH) as usize;

fn main() {
    let mut window: PistonWindow = WindowSettings::new("Real Time CPU Usage", [800, 480])
        .exit_on_esc(true)
        .build()
        .unwrap_or_else(|e| panic!("Failed to build PistonWindow: {}", e));
    window.set_max_fps(FPS as u64);
    window.set_lazy(true);

    let datas = Arc::new(Mutex::new(vec![(Local::now(), 0)]));
    let timer = timer::Timer::new();
    // Start repeating. Each callback increases `count`.
    let guard = {
        timer.schedule_repeating(chrono::Duration::milliseconds(500), move || {
            let mut rng = rand::thread_rng();
            datas
                .lock()
                .unwrap()
                .insert(0, (Local::now(), rng.gen_range(0, 10)));
        })
    };

    while let Some(e) = draw_piston_window(&mut window, |b| {
        let root = b.into_drawing_area().shrink((5, 5), (780, 200));
        root.fill(&WHITE);
        let data = Arc::clone(&datas);

        let mut chart = ChartBuilder::on(&root)
            .x_label_area_size(40)
            .y_label_area_size(40)
            .build_ranged(
                (data.lock().unwrap().last().unwrap().0)..(data.lock().unwrap().first().unwrap().0),
                0..10,
            )?;
        chart.configure_mesh().draw()?;

        chart.draw_series(LineSeries::new(
            data.lock().unwrap().iter().map(|x| (x.0, x.1)),
            &GREEN,
        ))?;
        /*
        &window.next().map(|e| {
            window.draw_2d(&e, |c, g, _| {
                clear([0.5, 1.0, 0.5, 1.0], g);
                rectangle(
                    [1.0, 0.0, 0.0, 1.0],
                    [50.0, 50.0, 100.0, 100.0],
                    c.transform,
                    g,
                );
            });
        });*/
        Ok(())
    }) {}
}
