#![no_main]
#![no_std]

use ariel_os::{log::info, time::Timer};

#[ariel_os::task(autostart)]
async fn main() {
    Timer::after_secs(5).await;
    info!("Hello from Ariel OS!");
}
