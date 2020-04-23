// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_winit::WinitWindow;
use glium;

pub struct GliumDisplayWinitWrapper(pub glium::Display);

pub struct EventLoop {
    ui_needs_update: bool,
    last_update: std::time::Instant,
}

impl WinitWindow for GliumDisplayWinitWrapper {
    fn get_inner_size(&self) -> Option<(u32, u32)> {
        self.0.gl_window().get_inner_size().map(Into::into)
    }

    fn hidpi_factor(&self) -> f32 {
        self.0.gl_window().get_hidpi_factor() as _
    }
}

impl EventLoop {
    pub fn new() -> Self {
        EventLoop {
            last_update: std::time::Instant::now(),
            ui_needs_update: true,
        }
    }

    /// Produce an iterator yielding all available events.
    pub fn next(
        &mut self,
        events_loop: &mut glium::glutin::EventsLoop,
    ) -> Vec<glium::glutin::Event> {
        // We don't want to loop any faster than 60 FPS, so wait until it has been at least 16ms \
        //   since the last yield.
        let (last_update, sixteen_ms) = (self.last_update, std::time::Duration::from_millis(32));

        let duration_since_last_update = std::time::Instant::now().duration_since(last_update);

        if duration_since_last_update < sixteen_ms {
            std::thread::sleep(sixteen_ms - duration_since_last_update);
        }

        // Collect all pending events.
        let mut events = Vec::new();

        events_loop.poll_events(|event| events.push(event));

        // If there are no events and the `Ui` does not need updating, wait for the next event.
        if events.is_empty() && !self.ui_needs_update {
            events_loop.run_forever(|event| {
                events.push(event);

                glium::glutin::ControlFlow::Break
            });
        }

        self.ui_needs_update = false;
        self.last_update = std::time::Instant::now();

        events
    }

    /// Notifies the event loop that the `Ui` requires another update whether or not there are any \
    ///   pending events.
    pub fn needs_update(&mut self) {
        self.ui_needs_update = true;
    }
}

// Conversion functions for converting between types from glium's version of `winit` and \
//   `conrod_core`.
conrod_winit::conversion_fns!();
