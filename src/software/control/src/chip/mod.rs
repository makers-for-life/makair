// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use chrono::{offset::Utc, DateTime, Duration};
use std::collections::{HashMap, VecDeque};
use std::convert::TryFrom;

use crate::config::environment::*;
use crate::physics::types::DataPressure;
use telemetry::alarm::AlarmCode;
use telemetry::serial::core;
use telemetry::structures::{AlarmPriority, DataSnapshot, MachineStateSnapshot, TelemetryMessage};

#[derive(Debug, PartialEq, Eq)]
pub enum ChipState {
    Initializing,
    Running,
    Stopped,
    WaitingData,
    Error(String),
}

pub struct Chip {
    pub boot_time: Option<DateTime<Utc>>,
    pub last_tick: u64,
    pub data_pressure: DataPressure,
    pub last_machine_snapshot: MachineStateSnapshot,
    pub ongoing_alarms: HashMap<AlarmCode, AlarmPriority>,
    state: ChipState,
}

impl Chip {
    pub fn new() -> Chip {
        Chip {
            boot_time: None,
            last_tick: 0,
            data_pressure: VecDeque::with_capacity(GRAPH_NUMBER_OF_POINTS + 100),
            last_machine_snapshot: MachineStateSnapshot::default(),
            ongoing_alarms: HashMap::new(),
            state: ChipState::WaitingData,
        }
    }

    pub fn new_event(&mut self, event: TelemetryMessage) {
        match event {
            TelemetryMessage::AlarmTrap(alarm) => {
                self.update_tick(alarm.systick);
                self.new_alarm(
                    alarm.alarm_code.into(),
                    alarm.alarm_priority,
                    alarm.triggered,
                );
            }

            TelemetryMessage::BootMessage(snapshot) => {
                self.reset(snapshot.systick);

                self.state = ChipState::Initializing;
            }

            TelemetryMessage::DataSnapshot(snapshot) => {
                self.clean_if_stopped();
                self.update_tick(snapshot.systick);

                self.add_pressure(&snapshot);

                self.state = ChipState::Running;
            }

            TelemetryMessage::MachineStateSnapshot(snapshot) => {
                self.clean_if_stopped();
                self.update_tick(snapshot.systick);

                for alarm in &snapshot.current_alarm_codes {
                    match AlarmPriority::try_from(*alarm) {
                        Ok(priority) => self.new_alarm((*alarm).into(), priority, true),
                        Err(e) => warn!(
                            "Skip alarm {} because we couldn't get the priority: {:?}",
                            alarm, e
                        ),
                    };
                }

                self.last_machine_snapshot = snapshot;

                self.state = ChipState::Running;
            }

            TelemetryMessage::StoppedMessage(message) => {
                self.update_tick(message.systick);

                self.state = ChipState::Stopped;
            }
        };
    }

    pub fn new_error(&mut self, error: core::Error) {
        match error.kind() {
            core::ErrorKind::NoDevice => self.state = ChipState::WaitingData,
            err => self.state = ChipState::Error(format!("{:?}", err)),
        };
    }

    fn new_alarm(&mut self, code: AlarmCode, priority: AlarmPriority, triggered: bool) {
        if triggered {
            self.ongoing_alarms.insert(code, priority); // If we ever receive the same alarm, just replace the one we have
        } else {
            self.ongoing_alarms.remove(&code);
        }
    }

    fn add_pressure(&mut self, snapshot: &DataSnapshot) {
        assert!(self.boot_time.is_some());

        let snapshot_time =
            self.boot_time.unwrap() + Duration::microseconds(snapshot.systick as i64);

        // Points are stored as mmH20 (for more precision; though we do work in cmH20)
        self.data_pressure.push_front((snapshot_time, snapshot.pressure));
    }

    pub fn get_state(&self) -> &ChipState {
        &self.state
    }

    fn update_boot_time(&mut self) {
        let now = Utc::now();
        let duration = chrono::Duration::microseconds(self.last_tick as i64);

        self.boot_time = Some(now - duration);
    }

    fn update_tick(&mut self, tick: u64) {
        if tick < self.last_tick {
            self.reset(tick);
        } else {
            self.last_tick = tick;

            if self.boot_time.is_none() {
                self.update_boot_time();
            }
        }
    }

    fn clean_if_stopped(&mut self) {
        if self.state == ChipState::Stopped {
            self.data_pressure.clear();
            self.last_machine_snapshot = MachineStateSnapshot::default();
        }
    }

    pub fn reset(&mut self, new_tick: u64) {
        self.last_tick = new_tick;
        self.data_pressure.clear();
        self.last_machine_snapshot = MachineStateSnapshot::default();

        self.update_boot_time();
    }

    pub fn clean_events(&mut self) {
        if !self.data_pressure.is_empty() {
            let older = self.data_pressure.front().unwrap().0
                - chrono::Duration::seconds(GRAPH_DRAW_SECONDS as _);

            while self
                .data_pressure
                .back()
                .map(|p| p.0 < older)
                .unwrap_or(false)
            {
                self.data_pressure.pop_back();
            }
        }
    }

    pub fn ongoing_alarms_sorted(&self) -> Vec<(&AlarmCode, &AlarmPriority)> {
        let mut vec_alarms = self
            .ongoing_alarms
            .iter()
            .collect::<Vec<(&AlarmCode, &AlarmPriority)>>();

        vec_alarms.sort_by(|(_, priority1), (_, priority2)| priority1.cmp(&priority2).reverse());

        vec_alarms
    }
}
