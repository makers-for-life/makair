#[derive(Debug, Clone)]
pub enum Phase {
    Inhalation,
    Exhalation,
}

#[derive(Debug, Clone)]
pub enum SubPhase {
    Inspiration,
    HoldInspiration,
    Exhale,
}

#[derive(Debug, Clone)]
pub enum TelemetryMessage {
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
}
