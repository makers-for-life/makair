use nom::number::complete::{be_u16, be_u32, be_u64, be_u8};

use crate::telemetry::*;

named!(
    parse_message_type<MessageType>,
    alt!(
        tag!("D") => { |_| MessageType::DataSnapshot } |
        tag!("S") => { |_| MessageType::MachineStateSnapshot } |
        tag!("T") => { |_| MessageType::AlarmTrap }
    )
);

named!(sep, tag!("\t"));
named!(end, tag!("\n"));

named!(pub parse_header<Header>, do_parse!(
    message_type: parse_message_type
        >> tag!(":")
        >> version: be_u8
        >> (Header {
            version: version,
            message_type: message_type
        })
));

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

named!(pub parse_data_snapshot<DataSnapshot>, do_parse!(
    version_len: be_u8
        >> version: map_res!(take!(version_len), |bytes| std::str::from_utf8(bytes))
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
        >> (DataSnapshot {
            version: version.to_string(),
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
));
