// MakAir
//
// Copyright: 2020, Makers For Life
// License: Public Domain License

use std::collections::VecDeque;

use chrono::offset::Utc;
use chrono::DateTime;

pub type DataPressure = VecDeque<(DateTime<Utc>, u16)>;
