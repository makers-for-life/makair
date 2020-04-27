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
pub struct AlarmCode {
    code: u8,
}

impl AlarmCode {
    pub fn description(self) -> String {
        match self.code {
            RMC_SW_1 => "Plateau pressure is not reached".to_string(),
            RMC_SW_2 => "Patient is unplugged".to_string(),
            RMC_SW_3 => "PEEP pressure is not reached".to_string(),
            RMC_SW_11 => "Battery low".to_string(),
            RMC_SW_12 => "Battery very low".to_string(),
            RMC_SW_14 => "Plateau pressure is not reached".to_string(),
            RMC_SW_15 => "PEEP pressure is not reached".to_string(),
            RMC_SW_16 => "Power cable unplugged".to_string(),
            RMC_SW_18 => "Pressure too high".to_string(),
            RMC_SW_19 => "Patient is unplugged".to_string(),
            _ => format!("Unknown alert {}", self.code),
        }
    }

    pub fn code(self) -> u8 {
        self.code
    }
}

impl From<u8> for AlarmCode {
    fn from(code: u8) -> AlarmCode {
        AlarmCode { code }
    }
}
