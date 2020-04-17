use nom::number::streaming::{be_u16, be_u32, be_u64, be_u8};

use crate::telemetry::*;

named!(sep, tag!("\t"));
named!(end, tag!("\n"));

named!(
    phase_and_subphase<(Phase, SubPhase)>,
    alt!(
        map!(tag!([17u8]), |_| (Phase::Inhalation, SubPhase::Inspiration))
            | map!(tag!([18u8]), |_| (
                Phase::Inhalation,
                SubPhase::HoldInspiration
            ))
            | map!(tag!([68u8]), |_| (Phase::Exhalation, SubPhase::Exhale))
    )
);

named!(
    alarm_priority<AlarmPriority>,
    alt!(
        map!(tag!([4u8]), |_| AlarmPriority::High)
            | map!(tag!([2u8]), |_| AlarmPriority::Medium)
            | map!(tag!([1u8]), |_| AlarmPriority::Low)
    )
);

named!(
    u8_array<Vec<u8>>,
    map!(length_data!(be_u8), |slice| Vec::from(slice))
);

named!(
    triggered<bool>,
    alt!(map!(tag!([240u8]), |_| true) | map!(tag!([15u8]), |_| false))
);

named!(
    data_snapshot<TelemetryMessage>,
    do_parse!(
        tag!("D:")
            >> tag!([1u8])
            >> software_version_len: be_u8
            >> software_version:
                map_res!(take!(software_version_len), |bytes| std::str::from_utf8(
                    bytes
                ))
            >> device_id1: be_u32
            >> device_id2: be_u32
            >> device_id3: be_u32
            >> sep
            >> systick: be_u64
            >> sep
            >> centile: be_u16
            >> sep
            >> pressure: be_u16
            >> sep
            >> phase_and_subphase: phase_and_subphase
            >> sep
            >> blower_valve_position: be_u8
            >> sep
            >> patient_valve_position: be_u8
            >> sep
            >> blower_rpm: be_u8
            >> sep
            >> battery_level: be_u8
            >> end
            >> (TelemetryMessage::DataSnapshot {
                version: software_version.to_string(),
                device_id: format!("{}-{}-{}", device_id1, device_id2, device_id3),
                systick,
                centile,
                pressure,
                phase: phase_and_subphase.0,
                subphase: phase_and_subphase.1,
                blower_valve_position,
                patient_valve_position,
                blower_rpm,
                battery_level,
            })
    )
);

named!(
    machine_state_snapshot<TelemetryMessage>,
    do_parse!(
        tag!("S:")
            >> tag!([1u8])
            >> software_version_len: be_u8
            >> software_version:
                map_res!(take!(software_version_len), |bytes| std::str::from_utf8(
                    bytes
                ))
            >> device_id1: be_u32
            >> device_id2: be_u32
            >> device_id3: be_u32
            >> sep
            >> cycle: be_u32
            >> sep
            >> peak_command: be_u8
            >> sep
            >> plateau_command: be_u8
            >> sep
            >> peep_command: be_u8
            >> sep
            >> cpm_command: be_u8
            >> sep
            >> previous_peak_pressure: be_u8
            >> sep
            >> previous_plateau_pressure: be_u8
            >> sep
            >> previous_peep_pressure: be_u8
            >> sep
            >> current_alarm_codes: u8_array
            >> sep
            >> previous_alarm_codes: u8_array
            >> end
            >> (TelemetryMessage::MachineStateSnapshot {
                version: software_version.to_string(),
                device_id: format!("{}-{}-{}", device_id1, device_id2, device_id3),
                cycle,
                peak_command,
                plateau_command,
                peep_command,
                cpm_command,
                previous_peak_pressure,
                previous_plateau_pressure,
                previous_peep_pressure,
                current_alarm_codes,
                previous_alarm_codes,
            })
    )
);

named!(
    alarm_trap<TelemetryMessage>,
    dbg_dmp!(do_parse!(
        tag!("T:")
            >> tag!([1u8])
            >> software_version_len: be_u8
            >> software_version:
                map_res!(take!(software_version_len), |bytes| std::str::from_utf8(
                    bytes
                ))
            >> device_id1: be_u32
            >> device_id2: be_u32
            >> device_id3: be_u32
            >> sep
            >> systick: be_u64
            >> sep
            >> centile: be_u16
            >> sep
            >> pressure: be_u16
            >> sep
            >> phase_and_subphase: phase_and_subphase
            >> sep
            >> cycle: be_u32
            >> sep
            >> alarm_code: be_u8
            >> sep
            >> alarm_priority: be_u8
            >> sep
            >> triggered: dbg_dmp!(triggered)
            >> sep
            >> expected: be_u32
            >> sep
            >> measured: be_u32
            >> sep
            >> cycles_since_trigger: be_u32
            >> end
            >> ({
                println!("{:?}", triggered);
                println!("{:#b}", alarm_priority);
                TelemetryMessage::AlarmTrap {
                    version: software_version.to_string(),
                    device_id: format!("{}-{}-{}", device_id1, device_id2, device_id3),
                    systick,
                    centile,
                    pressure,
                    phase: phase_and_subphase.0,
                    subphase: phase_and_subphase.1,
                    cycle,
                    alarm_code,
                    alarm_priority: AlarmPriority::High,
                    triggered: true,
                    expected,
                    measured,
                    cycles_since_trigger,
                }
            })
    ))
);

named!(pub parse_telemetry_message<TelemetryMessage>, alt!(
    data_snapshot | machine_state_snapshot | alarm_trap
));
