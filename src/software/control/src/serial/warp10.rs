use chrono::{DateTime, Duration, Utc};
use std::convert::TryInto;
use std::sync::mpsc::{Receiver, Sender};
use time::Timespec;
use warp10::{Data, Label, Value};

use telemetry::structures::*;

pub fn init(host: &'static str, token: &'static str) -> Sender<TelemetryMessage> {
    let (tx, rx): (Sender<TelemetryMessage>, Receiver<TelemetryMessage>) =
        std::sync::mpsc::channel();

    std::thread::spawn(move || {
        let client = warp10::Client::new(host).unwrap();
        let writer = client.get_writer(token.to_string());
        send_messages_to_warp10(rx, writer);
    });
    tx
}

fn send_messages_to_warp10(rx: Receiver<TelemetryMessage>, writer: warp10::Writer) {
    static PREFIX: &str = "makair";
    let mut buffer: Vec<Data> = vec![];
    let mut origin: Option<DateTime<Utc>> = None;

    loop {
        match rx.try_recv() {
            Ok(TelemetryMessage::DataSnapshot(DataSnapshot {
                device_id,
                systick,
                pressure,
                ..
            })) => {
                origin = mk_origin(origin, systick);
                let ts = mk_timestamp(origin.unwrap(), systick);

                buffer.push(Data::new(
                    ts,
                    None,
                    format!("{}.{}", &PREFIX, "pressure"),
                    mk_label(&device_id),
                    Value::Int(pressure.into()),
                ));
            }
            Ok(TelemetryMessage::AlarmTrap(AlarmTrap {
                device_id,
                systick,
                alarm_code,
                triggered,
                ..
            })) => {
                origin = mk_origin(origin, systick);
                let ts = mk_timestamp(origin.unwrap(), systick);

                buffer.push(Data::new(
                    ts,
                    None,
                    format!("{}.{}.{}", &PREFIX, "alarm", &alarm_code),
                    mk_label(&device_id),
                    Value::Boolean(triggered),
                ));
            }
            Ok(TelemetryMessage::MachineStateSnapshot(MachineStateSnapshot {
                device_id,
                systick,
                peak_command,
                plateau_command,
                peep_command,
                cpm_command,
                ..
            })) => {
                origin = mk_origin(origin, systick);
                let ts = mk_timestamp(origin.unwrap(), systick);

                buffer.append(&mut vec![
                    Data::new(
                        ts,
                        None,
                        format!("{}.{}", &PREFIX, "peak_command"),
                        mk_label(&device_id),
                        Value::Int(peak_command.into()),
                    ),
                    Data::new(
                        ts,
                        None,
                        format!("{}.{}", &PREFIX, "plateau_command"),
                        mk_label(&device_id),
                        Value::Int(plateau_command.into()),
                    ),
                    Data::new(
                        ts,
                        None,
                        format!("{}.{}", &PREFIX, "peep_command"),
                        mk_label(&device_id),
                        Value::Int(peep_command.into()),
                    ),
                    Data::new(
                        ts,
                        None,
                        format!("{}.{}", &PREFIX, "cpm_command"),
                        mk_label(&device_id),
                        Value::Int(cpm_command.into()),
                    ),
                ]);

                match writer.post(buffer) {
                    Ok(_) => (),
                    Err(e) => {
                        warn!("There was an error while sending values to Warp10: {}", &e);
                    }
                }
                buffer = vec![];
            }
            _ => (),
        }
    }
}

fn mk_origin(origin: Option<DateTime<Utc>>, systick: u64) -> Option<DateTime<Utc>> {
    match origin {
        Some(t) => Some(t),
        None => {
            let now = Utc::now();
            let duration = Duration::microseconds(systick.try_into().unwrap());
            Some(now - duration)
        }
    }
}

fn mk_timestamp(origin: DateTime<Utc>, systick: u64) -> Timespec {
    let duration = Duration::microseconds(systick.try_into().unwrap());
    to_timespec(origin + duration)
}

fn mk_label(device_id: &str) -> Vec<Label> {
    vec![Label::new("device_id", device_id)]
}

fn to_timespec(datetime: DateTime<Utc>) -> Timespec {
    Timespec::new(
        datetime.timestamp(),
        datetime.timestamp_subsec_nanos().try_into().unwrap(),
    )
}
