// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

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

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TelemetryMessage {
    BootMessage {
        version: String,
        device_id: String,
        systick: u64,
        mode: Mode,
        value128: u8,
    },
    StoppedMessage {
        version: String,
        device_id: String,
        systick: u64,
    },
    DataSnapshot {
        version: String,
        device_id: String,
        systick: u64,
        centile: u16,
        pressure: u16,
        phase: Phase,
        subphase: SubPhase,
        blower_valve_position: u8,
        patient_valve_position: u8,
        blower_rpm: u8,
        battery_level: u8,
    },
    MachineStateSnapshot {
        version: String,
        device_id: String,
        cycle: u32,
        peak_command: u8,
        plateau_command: u8,
        peep_command: u8,
        cpm_command: u8,
        previous_peak_pressure: u8,
        previous_plateau_pressure: u8,
        previous_peep_pressure: u8,
        current_alarm_codes: Vec<u8>,
        previous_alarm_codes: Vec<u8>,
    },
    AlarmTrap {
        version: String,
        device_id: String,
        systick: u64,
        centile: u16,
        pressure: u16,
        phase: Phase,
        subphase: SubPhase,
        cycle: u32,
        alarm_code: u8,
        alarm_priority: AlarmPriority,
        triggered: bool,
        expected: u32,
        measured: u32,
        cycles_since_trigger: u32,
    },
}
