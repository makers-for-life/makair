// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

const RMC_SW_1: u8 = 12;
const RMC_SW_2: u8 = 11;
const RMC_SW_3: u8 = 14;
const RMC_SW_11: u8 = 21;
const RMC_SW_12: u8 = 13;
const RMC_SW_14: u8 = 22;
const RMC_SW_15: u8 = 23;
const RMC_SW_16: u8 = 31;
const RMC_SW_18: u8 = 17;
const RMC_SW_19: u8 = 24;

#[allow(non_camel_case_types)]
#[derive(Debug, Eq, PartialEq, Hash, Clone, Copy)]
pub enum AlarmCode {
    RMC_SW_1,
    RMC_SW_2,
    RMC_SW_3,
    RMC_SW_11,
    RMC_SW_12,
    RMC_SW_14,
    RMC_SW_15,
    RMC_SW_16,
    RMC_SW_18,
    RMC_SW_19,
    Unknown(u8),
}

impl AlarmCode {
    pub fn description(self) -> String {
        match self {
            Self::RMC_SW_1 => "Plateau pressure is not reached".to_string(),
            Self::RMC_SW_2 => "Patient is unplugged".to_string(),
            Self::RMC_SW_3 => "PEEP pressure is not reached".to_string(),
            Self::RMC_SW_11 => "Battery low".to_string(),
            Self::RMC_SW_12 => "Battery very low".to_string(),
            Self::RMC_SW_14 => "Plateau pressure is not reached".to_string(),
            Self::RMC_SW_15 => "PEEP pressure is not reached".to_string(),
            Self::RMC_SW_16 => "Power cable unplugged".to_string(),
            Self::RMC_SW_18 => "Pressure too high".to_string(),
            Self::RMC_SW_19 => "Patient is unplugged".to_string(),
            Self::Unknown(code) => format!("Unknown alert {}", code),
        }
    }
}

impl From<u8> for AlarmCode {
    fn from(code: u8) -> AlarmCode {
        match code {
            RMC_SW_1 => AlarmCode::RMC_SW_1,
            RMC_SW_2 => AlarmCode::RMC_SW_2,
            RMC_SW_3 => AlarmCode::RMC_SW_3,
            RMC_SW_11 => AlarmCode::RMC_SW_11,
            RMC_SW_12 => AlarmCode::RMC_SW_12,
            RMC_SW_14 => AlarmCode::RMC_SW_14,
            RMC_SW_15 => AlarmCode::RMC_SW_15,
            RMC_SW_16 => AlarmCode::RMC_SW_16,
            RMC_SW_18 => AlarmCode::RMC_SW_18,
            RMC_SW_19 => AlarmCode::RMC_SW_19,
            _ => AlarmCode::Unknown(code),
        }
    }
}
