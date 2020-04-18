use telemetry::*;

fn main() {
    env_logger::init();

    let port_id = "COM30";

    gather_telemetry(&port_id);
}
