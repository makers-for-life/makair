// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, TryRecvError};

use chrono::Duration;
use telemetry::{
    self,
    structures::MachineStateSnapshot,
    structures::{DataSnapshot, TelemetryMessage},
};

use crate::chip::Chip;
use crate::physics::types::DataPressure;

pub struct SerialHandlerBuilder;

pub struct SerialHandler {
    chip: Chip,
}

impl SerialHandlerBuilder {
    pub fn new() -> SerialHandler {
        SerialHandler { chip: Chip::new() }
    }
}

impl SerialHandler {
    pub fn handle(
        &mut self,
        rx: &Receiver<TelemetryMessage>,
        data: &mut DataPressure,
    ) -> Option<MachineStateSnapshot> {
        let mut machine_snapshot = None;

        loop {
            match rx.try_recv() {
                Ok(message) => match message {
                    TelemetryMessage::BootMessage(snapshot) => {
                        self.chip.reset(snapshot.systick);
                    }

                    TelemetryMessage::DataSnapshot(snapshot) => {
                        self.chip.update_tick(snapshot.systick);

                        self.add_pressure(data, snapshot);
                    }

                    TelemetryMessage::MachineStateSnapshot(snapshot) => {
                        machine_snapshot = Some(snapshot);
                    }

                    _ => {}
                },

                Err(TryRecvError::Empty) => {
                    break;
                }

                Err(TryRecvError::Disconnected) => {
                    panic!("channel to serial port thread was closed");
                }
            }
        }

        machine_snapshot
    }

    fn add_pressure(&self, data: &mut DataPressure, snapshot: DataSnapshot) {
        assert!(self.chip.boot_time.is_some());

        let snapshot_time =
            self.chip.boot_time.unwrap() + Duration::microseconds(snapshot.systick as i64);

        let point = snapshot.pressure / 10;

        data.push_front((snapshot_time, point));
    }
}
