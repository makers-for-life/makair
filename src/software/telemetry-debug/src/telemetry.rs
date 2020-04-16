#[derive(Debug, Clone)]
pub enum MessageType {
    DataSnapshot,
    MachineStateSnapshot,
    AlarmTrap,
}

#[derive(Debug, Clone)]
pub struct Header {
    pub version: u8,
    pub message_type: MessageType,
}

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

#[derive(Debug, Clone)]
pub struct MachineStateSnapshot {
    pub version: String,
    pub device_id: String,
    pub cycle: u32,
    pub peak_command: u8,
    pub plateau_command: u8,
    pub peep_command: u8,
    pub cpm_command: u8,
    pub previous_peak_pressure: u8,
    pub previous_plateau_pressure: u8,
    pub previous_peep_pressure: u8,
}
