// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::sync::mpsc::{Receiver, TryRecvError};

use telemetry::serial::core::{Error, ErrorKind};
use telemetry::structures::TelemetryMessage;
use telemetry::{self, TelemetryChannelType};

pub struct SerialPollerBuilder;
pub struct SerialPoller;

#[derive(Debug)]
pub enum PollEvent {
    Ready(TelemetryMessage),
    Pending,
}

impl SerialPollerBuilder {
    pub fn new() -> SerialPoller {
        SerialPoller {}
    }
}

impl SerialPoller {
    pub fn poll(&mut self, rx: &Receiver<TelemetryChannelType>) -> Result<PollEvent, Error> {
        match rx.try_recv() {
            Ok(message) => match message {
                Ok(message) => Ok(PollEvent::Ready(message)),
                Err(serial_error) => Err(serial_error),
            },
            Err(TryRecvError::Empty) => Ok(PollEvent::Pending),
            Err(TryRecvError::Disconnected) => {
                Err(Error::new(ErrorKind::NoDevice, "device is disconnected"))
            }
        }
    }
}
