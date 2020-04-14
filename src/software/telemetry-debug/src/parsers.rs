use nom::number::streaming::{be_u16, be_u32, be_u64, be_u8};

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

named!(pub parse_header<Header>, do_parse!(
    message_type: parse_message_type
        >> tag!(":")
        >> version: be_u8
        >> (Header {
            version: version,
            message_type: message_type
        })
));

named!(pub parse_data_snapshot<DataSnapshot>, do_parse!(
    device_id1: be_u32
        >> device_id2: be_u32
        >> device_id3: be_u32
        >> sep
        >> systick: be_u64
        >> sep
        >> centile: be_u16
        >> sep
        >> pressure: be_u16
        >> (DataSnapshot {
            device_id: format!("{}-{}-{}", device_id1, device_id2, device_id3),
            systick,
            centile,
            pressure,
        })
));
