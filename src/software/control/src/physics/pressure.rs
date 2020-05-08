// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use crate::config::environment::*;

pub fn process_max_allowed_pressure(peak_command: u8) -> u8 {
    let mut max_pressure = if peak_command > 0 {
        peak_command as f64
    } else {
        PEAK_PRESSURE_INITIAL_MIN
    };

    max_pressure = max_pressure + max_pressure * PEAK_PRESSURE_ALERT_ERROR_RATIO;

    max_pressure as u8
}
