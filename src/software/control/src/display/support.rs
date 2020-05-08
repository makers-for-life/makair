// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use conrod_winit::WinitWindow;

pub struct GliumDisplayWinitWrapper(pub glium::Display);

pub struct EventLoop;

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
        EventLoop {}
    }

    /// Produce an iterator yielding all available events.
    pub fn next(
        &mut self,
        events_loop: &mut glium::glutin::EventsLoop,
    ) -> Vec<glium::glutin::Event> {
        // Collect all pending events.
        let mut events = Vec::new();

        events_loop.poll_events(|event| events.push(event));

        events
    }
}

// Conversion functions for converting between types from glium's version of `winit` and \
//   `conrod_core`.
conrod_winit::conversion_fns!();
