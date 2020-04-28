use telemetry::alarm::AlarmCode;
use telemetry::serial::core;
use telemetry::structures::{
    AlarmPriority, AlarmTrap, BootMessage, DataSnapshot, MachineStateSnapshot, Mode, Phase,
    StoppedMessage, SubPhase, TelemetryMessage,
};

// /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\
// /!\ THIS FILE IS JUST A PARTIAL COPY OF src/software/telemetry/src/parsers.rs
// /!\ IT MUST BE DELETED AND REPLACED BY A GOOD CARGO CONFIG.
// /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\

#[cfg(test)]
pub mod tests {
    use super::*;
    use proptest::collection;
    use proptest::prelude::*;

    fn mode_strategy() -> impl Strategy<Value = Mode> {
        prop_oneof![
            Just(Mode::Production),
            Just(Mode::Qualification),
            Just(Mode::IntegrationTest),
        ]
    }

    // TODO Generate all combinations (independent each other) ?
    fn phase_subphase_strategy() -> impl Strategy<Value = (Phase, SubPhase)> {
        prop_oneof![
            (Just(Phase::Inhalation), Just(SubPhase::Inspiration)),
            (Just(Phase::Inhalation), Just(SubPhase::HoldInspiration)),
            (Just(Phase::Exhalation), Just(SubPhase::Exhale)),
        ]
    }

    fn alarm_priority_strategy() -> impl Strategy<Value = AlarmPriority> {
        prop_oneof![
            Just(AlarmPriority::Low),
            Just(AlarmPriority::Medium),
            Just(AlarmPriority::High),
        ]
    }

    #[derive(Debug, Clone, Copy)]
    pub struct DeviceIdInts(u32, u32, u32);

    impl DeviceIdInts {
        fn str(&self) -> String {
            format!("{}-{}-{}", self.0, self.1, self.2)
        }
    }

    prop_compose! {
        pub fn device_id_ints_strategy()(id1 in (0u32..), id2 in (0u32..), id3 in (0u32..)) -> DeviceIdInts {
            DeviceIdInts(id1, id2, id3)
        }
    }

    prop_compose! {
        pub fn boot_message_strategy()(
            version in ".*",
            device_id in device_id_ints_strategy(),
            systick in (0u64..),
            mode in mode_strategy(),
            value128 in (0u8..),
        ) -> (BootMessage, DeviceIdInts) {
            (BootMessage { version, device_id: device_id.str(), systick, mode, value128 }, device_id)
        }
    }

    prop_compose! {
        pub fn stopped_message_strategy()(
            version in ".*",
            device_id in device_id_ints_strategy(),
            systick in (0u64..),
        ) -> (StoppedMessage, DeviceIdInts){
            (StoppedMessage { version, device_id: device_id.str(), systick, }, device_id)
        }
    }

    prop_compose! {
        pub fn data_snapshot_message_strategy()(
            version in ".*",
            device_id in device_id_ints_strategy(),
            systick in (0u64..),
            centile in (0u16..),
            pressure in (0u16..),
            phase_subphase in phase_subphase_strategy(),
            blower_valve_position in (0u8..),
            patient_valve_position in (0u8..),
            blower_rpm in (0u8..),
            battery_level in (0u8..),
        ) -> (DataSnapshot, DeviceIdInts, (Phase, SubPhase)){
            (DataSnapshot {
                version,
                device_id: device_id.str(),
                systick,
                centile,
                pressure,
                phase: phase_subphase.0.clone(),
                subphase: phase_subphase.1.clone(),
                blower_valve_position,
                patient_valve_position,
                blower_rpm,
                battery_level,
            }, device_id, phase_subphase)
        }
    }

    prop_compose! {
        pub fn machine_state_message_strategy()(
            version in ".*",
            device_id in device_id_ints_strategy(),
            systick in (0u64..),
            cycle in (0u32..),
            peak_command in (0u8..),
            plateau_command in (0u8..),
            peep_command in (0u8..),
            cpm_command in (0u8..),
            previous_peak_pressure in (0u16..),
            previous_plateau_pressure in (0u16..),
            previous_peep_pressure in (0u16..),
            current_alarm_codes in collection::vec(0u8.., 0..100),
        ) -> (MachineStateSnapshot, DeviceIdInts){
            (MachineStateSnapshot {
                version,
                device_id: device_id.str(),
                systick,
                cycle,
                peak_command,
                plateau_command,
                peep_command,
                cpm_command,
                previous_peak_pressure,
                previous_plateau_pressure,
                previous_peep_pressure,
                current_alarm_codes,
            }, device_id)
        }
    }

    prop_compose! {
        pub fn alarm_trap_message_strategy()(
            version in ".*",
            device_id in device_id_ints_strategy(),
            systick in (0u64..),
            centile in (0u16..),
            pressure in (0u16..),
            phase_subphase in phase_subphase_strategy(),
            cycle in (0u32..),
            alarm_code in (0u8..),
            alarm_priority in alarm_priority_strategy(),
            triggered in proptest::bool::ANY,
            expected in (0u32..),
            measured in (0u32..),
            cycles_since_trigger in (0u32..),
        ) -> (AlarmTrap, DeviceIdInts, (Phase, SubPhase)){
            (AlarmTrap {
                version,
                device_id: device_id.str(),
                systick,
                centile,
                pressure,
                phase: phase_subphase.0.clone(),
                subphase: phase_subphase.1.clone(),
                cycle,
                alarm_code,
                alarm_priority,
                triggered,
                expected,
                measured,
                cycles_since_trigger,
            }, device_id, phase_subphase)
        }
    }

    pub fn telemetry_message_strategy() -> impl Strategy<Value = TelemetryMessage> {
        prop_oneof![
            boot_message_strategy().prop_map(|(msg, _)| TelemetryMessage::BootMessage(msg)),
            stopped_message_strategy().prop_map(|(msg, _)| TelemetryMessage::StoppedMessage(msg)),
            data_snapshot_message_strategy()
                .prop_map(|(msg, _, _)| TelemetryMessage::DataSnapshot(msg)),
            machine_state_message_strategy()
                .prop_map(|(msg, _)| TelemetryMessage::MachineStateSnapshot(msg)),
            alarm_trap_message_strategy().prop_map(|(msg, _, _)| TelemetryMessage::AlarmTrap(msg)),
        ]
    }
}
