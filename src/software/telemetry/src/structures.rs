// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::cmp::{Ord, Ordering, PartialOrd};

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Mode {
    Production,
    Qualification,
    IntegrationTest,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Phase {
    Inhalation,
    Exhalation,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum SubPhase {
    Inspiration,
    HoldInspiration,
    Exhale,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum AlarmPriority {
    High,
    Medium,
    Low,
}

impl PartialOrd for AlarmPriority {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for AlarmPriority {
    fn cmp(&self, other: &Self) -> Ordering {
        let priority_to_int = |priority: &AlarmPriority| match priority {
            AlarmPriority::High => 3,
            AlarmPriority::Medium => 2,
            AlarmPriority::Low => 1,
        };

        priority_to_int(self).cmp(&priority_to_int(other))
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct BootMessage {
    pub version: String,
    pub device_id: String,
    pub systick: u64,
    pub mode: Mode,
    pub value128: u8,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct StoppedMessage {
    pub version: String,
    pub device_id: String,
    pub systick: u64,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DataSnapshot {
    pub version: String,
    pub device_id: String,
    pub systick: u64,
    pub centile: u16,
    pub pressure: u16,
    pub phase: Phase,
    pub subphase: SubPhase,
    pub blower_valve_position: u8,
    pub patient_valve_position: u8,
    pub blower_rpm: u8,
    pub battery_level: u8,
}

#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct MachineStateSnapshot {
    pub version: String,
    pub device_id: String,
    pub systick: u64,
    pub cycle: u32,
    pub peak_command: u8,
    pub plateau_command: u8,
    pub peep_command: u8,
    pub cpm_command: u8,
    pub previous_peak_pressure: u16,
    pub previous_plateau_pressure: u16,
    pub previous_peep_pressure: u16,
    pub current_alarm_codes: Vec<u8>,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AlarmTrap {
    pub version: String,
    pub device_id: String,
    pub systick: u64,
    pub centile: u16,
    pub pressure: u16,
    pub phase: Phase,
    pub subphase: SubPhase,
    pub cycle: u32,
    pub alarm_code: u8,
    pub alarm_priority: AlarmPriority,
    pub triggered: bool,
    pub expected: u32,
    pub measured: u32,
    pub cycles_since_trigger: u32,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TelemetryMessage {
    BootMessage(BootMessage),
    StoppedMessage(StoppedMessage),
    DataSnapshot(DataSnapshot),
    MachineStateSnapshot(MachineStateSnapshot),
    AlarmTrap(AlarmTrap),
}

#[cfg(test)]
mod tests {
    use crate::structures::AlarmPriority;
    use std::cmp::Ordering;

    #[test]
    fn order_alarm_priority() {
        let high = AlarmPriority::High;
        let medium = AlarmPriority::Medium;
        let low = AlarmPriority::Low;

        // equal
        assert_eq!(high.cmp(&high), Ordering::Equal);
        assert_eq!(medium.cmp(&medium), Ordering::Equal);
        assert_eq!(low.cmp(&low), Ordering::Equal);

        // lower
        assert_eq!(medium.cmp(&high), Ordering::Less);
        assert_eq!(low.cmp(&high), Ordering::Less);
        assert_eq!(low.cmp(&medium), Ordering::Less);

        // greater
        assert_eq!(high.cmp(&medium), Ordering::Greater);
        assert_eq!(high.cmp(&low), Ordering::Greater);
        assert_eq!(medium.cmp(&low), Ordering::Greater);
    }
}
