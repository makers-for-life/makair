use nom::number::streaming::{be_u16, be_u32, be_u64, be_u8};

use crate::telemetry::*;

named!(sep, tag!("\t"));
named!(end, tag!("\n"));

named!(
    parse_phase_and_subphase<(Phase, SubPhase)>,
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
    parse_u8_array<Vec<u8>>,
    map!(length_data!(be_u8), |slice| Vec::from(slice))
);

named!(
    parse_data_snapshot<TelemetryMessage>,
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
            >> phase_and_subphase: parse_phase_and_subphase
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
    parse_machine_state_snapshot<TelemetryMessage>,
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
            >> current_alarm_codes: parse_u8_array
            >> sep
            >> previous_alarm_codes: parse_u8_array
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

named!(pub parse_telemetry_message<TelemetryMessage>, alt!(
    parse_data_snapshot | parse_machine_state_snapshot
));
