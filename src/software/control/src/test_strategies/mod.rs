#[cfg(test)]
pub mod tests {
    use proptest::collection;
    use proptest::prelude::*;
    use proptest::strategy::Strategy;
    use std::cell::Cell;
    use std::time::SystemTime;
    use telemetry::structures::{
        AlarmPriority, AlarmTrap, BootMessage, DataSnapshot, MachineStateSnapshot, Mode, Phase,
        StoppedMessage, SubPhase, TelemetryMessage,
    };

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

    prop_compose! {
        fn current_alarm_codes_strategy()(
            current_alarm_codes in collection::vec(0u8.., 0..100),
        ) -> Vec<u8> { current_alarm_codes }
    }

    fn pressure_strategy() -> impl Strategy<Value = u16> {
        0u16..32768
    }

    pub struct TelemetryStrategies {
        pub current_systick: Cell<u64>,
    }

    impl TelemetryStrategies {
        pub fn new() -> TelemetryStrategies {
            TelemetryStrategies {
                current_systick: Cell::new(
                    SystemTime::now()
                        .duration_since(SystemTime::UNIX_EPOCH)
                        .expect("Can't get millis time since UNIX_EPOCH")
                        .as_millis() as u64,
                ),
            }
        }

        pub fn systick_strategy(&self) -> impl Strategy<Value = u64> + '_ {
            (-100i64..1000).prop_map(move |delta| {
                let new_state = (self.current_systick.get() as i64 + delta) as u64;
                self.current_systick.set(new_state);
                new_state
            })
        }

        pub fn boot_message_strategy(
            &self,
        ) -> impl Strategy<Value = (BootMessage, DeviceIdInts)> + '_ {
            (
                ".*",
                device_id_ints_strategy(),
                self.systick_strategy(),
                mode_strategy(),
                0u8..,
            )
                .prop_map(|(version, device_id, systick, mode, value128)| {
                    (
                        BootMessage {
                            version,
                            device_id: device_id.str(),
                            systick,
                            mode,
                            value128,
                        },
                        device_id,
                    )
                })
        }

        pub fn stopped_message_strategy(
            &self,
        ) -> impl Strategy<Value = (StoppedMessage, DeviceIdInts)> + '_ {
            (".*", device_id_ints_strategy(), self.systick_strategy()).prop_map(
                |(version, device_id, systick)| {
                    (
                        StoppedMessage {
                            version,
                            device_id: device_id.str(),
                            systick,
                        },
                        device_id,
                    )
                },
            )
        }

        pub fn data_snapshot_message_strategy(
            &self,
        ) -> impl Strategy<Value = (DataSnapshot, DeviceIdInts, (Phase, SubPhase))> + '_ {
            (
                ".*",
                device_id_ints_strategy(),
                self.systick_strategy(),
                0u16..,
                pressure_strategy(),
                phase_subphase_strategy(),
                0u8..,
                0u8..,
                0u8..,
                0u8..,
            )
                .prop_map(
                    |(
                        version,
                        device_id,
                        systick,
                        centile,
                        pressure,
                        phase_subphase,
                        blower_valve_position,
                        patient_valve_position,
                        blower_rpm,
                        battery_level,
                    )| {
                        (
                            DataSnapshot {
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
                            },
                            device_id,
                            phase_subphase,
                        )
                    },
                )
        }

        pub fn machine_state_message_strategy(
            &self,
        ) -> impl Strategy<Value = (MachineStateSnapshot, DeviceIdInts)> + '_ {
            (
                (
                    ".*",
                    device_id_ints_strategy(),
                    self.systick_strategy(),
                    0u32..,
                    0u8..,
                    0u8..,
                    0u8..,
                    0u8..,
                ) // Sub-tuple, because the tuple exceeded size limit.
                    .prop_map(|t| t),
                0u16..,
                0u16..,
                0u16..,
                current_alarm_codes_strategy(),
            )
                .prop_map(
                    |(
                        (
                            version,
                            device_id,
                            systick,
                            cycle,
                            peak_command,
                            plateau_command,
                            peep_command,
                            cpm_command,
                        ),
                        previous_peak_pressure,
                        previous_plateau_pressure,
                        previous_peep_pressure,
                        current_alarm_codes,
                    )| {
                        (
                            MachineStateSnapshot {
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
                            },
                            device_id,
                        )
                    },
                )
        }

        pub fn alarm_trap_message_strategy(
            &self,
        ) -> impl Strategy<Value = (AlarmTrap, DeviceIdInts, (Phase, SubPhase))> + '_ {
            (
                (
                    ".*",
                    device_id_ints_strategy(),
                    self.systick_strategy(),
                    0u16..,
                    pressure_strategy(),
                    phase_subphase_strategy(),
                    0u32..,
                    0u8..,
                ) // Sub-tuple, because the tuple exceeded size limit.
                    .prop_map(|t| t),
                alarm_priority_strategy(),
                any::<bool>(),
                0u32..,
                0u32..,
                0u32..,
            )
                .prop_map(
                    |(
                        (
                            version,
                            device_id,
                            systick,
                            centile,
                            pressure,
                            phase_subphase,
                            cycle,
                            alarm_code,
                        ),
                        alarm_priority,
                        triggered,
                        expected,
                        measured,
                        cycles_since_trigger,
                    )| {
                        (
                            AlarmTrap {
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
                            },
                            device_id,
                            phase_subphase,
                        )
                    },
                )
        }

        pub fn telemetry_message_strategy(&self) -> impl Strategy<Value = TelemetryMessage> + '_ {
            prop_oneof![
                self.boot_message_strategy()
                    .prop_map(|(msg, _)| TelemetryMessage::BootMessage(msg)),
                self.stopped_message_strategy()
                    .prop_map(|(msg, _)| TelemetryMessage::StoppedMessage(msg)),
                self.data_snapshot_message_strategy()
                    .prop_map(|(msg, _, _)| TelemetryMessage::DataSnapshot(msg)),
                self.machine_state_message_strategy()
                    .prop_map(|(msg, _)| TelemetryMessage::MachineStateSnapshot(msg)),
                self.alarm_trap_message_strategy()
                    .prop_map(|(msg, _, _)| TelemetryMessage::AlarmTrap(msg)),
            ]
        }
    }
}
