// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, TryRecvError};

use chrono::Duration;
use telemetry::serial::core::{Error, ErrorKind};
use telemetry::structures::{DataSnapshot, MachineStateSnapshot, TelemetryMessage};
use telemetry::{self, TelemetryChannelType};

use crate::chip::Chip;
use crate::physics::types::DataPressure;

pub struct SerialPollerBuilder;

pub struct SerialPoller {
    chip: Chip,
}

#[derive(Debug, Default)]
struct PolledTelemetry {
    pub data_snapshots: Option<Vec<DataSnapshot>>,
    pub machine_snapshot: Option<MachineStateSnapshot>,
    pub stopped_message: Option<StoppedMessage>,
}

impl SerialPollerBuilder {
    pub fn new() -> SerialPoller {
        SerialPoller { chip: Chip::new() }
    }
}

impl SerialPoller {
    pub fn poll(
        &mut self,
        rx: &Receiver<TelemetryChannelType>,
    ) -> Result<MachineStateSnapshot, Error> {
        let mut data_snapshots = Vec::new();
        let mut machine_snapshot = None;
        let mut stopped_message = None;

        loop {
            match rx.try_recv() {
                Ok(message) => match message {
                    Ok(message) => match message {
                        TelemetryMessage::BootMessage(snapshot) => {
                            self.chip.reset(snapshot.systick);
                        }
                        TelemetryMessage::DataSnapshot(snapshot) => {
                            self.chip.update_tick(snapshot.systick);
                            data_snapshots.push(snapshot);
                        }
                        TelemetryMessage::MachineStateSnapshot(snapshot) => {
                            machine_snapshot = Some(snapshot);
                        }
                        TelemetryMessage::StoppedMessage(message) => {
                            self.chip.update_tick(message.systick);
                            stopped_message = Some(message);
                        }
                        _ => {}
                    },
                    Err(serial_error) => return Err(serial_error),
                },

                Err(TryRecvError::Empty) => {
                    break;
                }

                Err(TryRecvError::Disconnected) => {
                    return Err(Error::new(ErrorKind::NoDevice, "Device is disconnected"))
                }
            }
        }

        Ok(PolledTelemetry {
            data_snapshots: if data_snapshots.is_empty() {
                None
            } else {
                Some(data_snapshots)
            },
            machine_snapshot,
            stopped_message,
        })
    }

    fn add_pressure(&self, data: &mut DataPressure, snapshot: &DataSnapshot) {
        assert!(self.chip.boot_time.is_some());

        let snapshot_time =
            self.chip.boot_time.unwrap() + Duration::microseconds(snapshot.systick as i64);

        let point = snapshot.pressure / 10;

        data.push_front((snapshot_time, point));
    }
}
