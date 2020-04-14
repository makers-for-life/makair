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
pub struct DataSnapshot {
    pub device_id: String,
    pub systick: u64,
    pub centile: u16,
    pub pressure: u16,
}
