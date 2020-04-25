// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use chrono::{offset::Utc, DateTime};

pub struct Chip {
    pub boot_time: Option<DateTime<Utc>>,
    pub last_tick: u64,
}

impl Chip {
    pub fn new() -> Chip {
        Chip {
            boot_time: None,
            last_tick: 0,
        }
    }

    fn update_boot_time(&mut self) {
        let now = Utc::now();
        let duration = chrono::Duration::microseconds(self.last_tick as i64);

        self.boot_time = Some(now - duration);
    }

    #[inline(always)]
    pub fn update_tick(&mut self, tick: u64) {
        if tick < self.last_tick {
            self.reset(tick);
        } else {
            self.last_tick = tick;

            if self.boot_time.is_none() {
                self.update_boot_time();
            }
        }
    }

    pub fn reset(&mut self, new_tick: u64) {
        self.last_tick = new_tick;

        self.update_boot_time();
    }
}
