// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, TryRecvError};

use telemetry::serial::core::{Error, ErrorKind};
use telemetry::structures::{DataSnapshot, MachineStateSnapshot, StoppedMessage, TelemetryMessage};
use telemetry::{self, TelemetryChannelType};

use crate::chip::Chip;

pub struct SerialPollerBuilder;
pub struct SerialPoller;

#[derive(Debug, Default)]
pub struct PolledTelemetry {
    pub data_snapshots: Option<Vec<DataSnapshot>>,
    pub machine_snapshot: Option<MachineStateSnapshot>,
    pub stopped_message: Option<StoppedMessage>,
}

impl SerialPollerBuilder {
    pub fn new() -> SerialPoller {
        SerialPoller {}
    }
}

impl SerialPoller {
    pub fn poll(
        &mut self,
        rx: &Receiver<TelemetryChannelType>,
        chip: &mut Chip,
    ) -> Result<PolledTelemetry, Error> {
        let mut data_snapshots = Vec::new();
        let mut machine_snapshot = None;
        let mut stopped_message = None;

        loop {
            match rx.try_recv() {
                Ok(message) => match message {
                    Ok(message) => match message {
                        TelemetryMessage::BootMessage(snapshot) => {
                            chip.reset(snapshot.systick);
                        }
                        TelemetryMessage::DataSnapshot(snapshot) => {
                            chip.update_tick(snapshot.systick);
                            data_snapshots.push(snapshot);
                        }
                        TelemetryMessage::MachineStateSnapshot(snapshot) => {
                            machine_snapshot = Some(snapshot);
                        }
                        TelemetryMessage::StoppedMessage(message) => {
                            chip.update_tick(message.systick);
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
}
