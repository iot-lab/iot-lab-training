---
jupyter:
  jupytext:
    text_representation:
      extension: .md
      format_name: markdown
      format_version: '1.3'
      jupytext_version: 1.19.4
  kernelspec:
    display_name: Python 3 (ipykernel)
    language: python
    name: python3
---

## First Ariel OS application

In this exercise, you will build your first [Ariel OS](https://ariel-os.org) application and run it. Ariel OS is an operating system for secure, memory-safe, low-power Internet of Things, written in Rust.

- In a first step, the application will be built for the `native` target and run as a regular application.

- In a second step, you will submit a new experiment on IoT-LAB with one nrf52840dk node, then build and flash this application for this target.


### The application

An Ariel OS application is a Rust crate built with the [laze](https://kaspar030.github.io/laze/) build system, which itself invokes cargo with the right parameters for the selected board.

The [laze-project.yml](laze-project.yml) file configures the build: it imports the Ariel OS source code (available locally in this training) and declares the application:

```yaml
imports:
  - path: ../../ariel-os
    dldir: ariel-os
    symlink: true

apps:
  - name: hello-world-iotlab
```

The [Cargo.toml](Cargo.toml) file declares the Rust package, the path to the application code and the dependencies to the Ariel OS crates. The `time` feature is enabled because the application uses a timer.

<!-- #region -->
The [main.rs](main.rs) file contains the application code and is very simple. The entry point is an async task spawned automatically at startup by Ariel OS. The application waits five seconds and prints the message "Hello from Ariel OS!":

```rust
#![no_main]
#![no_std]

use ariel_os::{log::info, time::Timer};

#[ariel_os::task(autostart)]
async fn main() {
    Timer::after_secs(5).await;
    info!("Hello from Ariel OS!");
}
```
<!-- #endregion -->

### Build for native

Build the application and run it on native is as simple as running the following command:

```python
!laze build -b native run
```

**Important:** after the "Hello from Ariel OS!" message is displayed, the application keeps running. Interrupt the Jupyter kernel by clicking the stop button in the menu bar above.


### Build for the nrf52840dk

By default, Ariel OS sends the log messages to the debug channel (RTT), which requires a debug probe attached to the host computer. On IoT-LAB, the nodes are remote and only the serial port (UART) of the board is exposed. So here the `logging-over-uart` module is selected to redirect the log messages to the UART (configured at 115200 bauds, as expected by IoT-LAB):

```python
!laze build -b nrf52840dk --select logging-over-uart
```

The generated firmware is the ELF file produced by cargo, located at `build/bin/nrf52840dk/cargo/thumbv7em-none-eabihf/release/hello-world-iotlab`.

The IoT-LAB flashing tools detect the firmware format from the file extension, so copy the firmware to a file with the `.elf` extension:

```python
!cp build/bin/nrf52840dk/cargo/thumbv7em-none-eabihf/release/hello-world-iotlab hello-world-iotlab.elf
```

### Submit an experiment on IoT-LAB

The nrf52840dk boards are hosted on the Saclay site.

1. Submit an experiment with one nrf52840dk node for 20 minutes:

```python
!iotlab-experiment submit -n "hello-world-ariel" -d 20 -l 1,archi=nrf52840dk:multi+site=saclay
```

2. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later.

3. Get the experiment nodes list:

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

Note the id of the booked node: its network address is of the form `nrf52840dk-<id>.saclay.iot-lab.info`. You will need the `nrf52840dk-<id>` string to connect to the serial port of the node.


### Open a serial console and flash the firmware

The serial port of each node used in an experiment is reachable via a TCP socket (port 20000) exposed on the site SSH frontend.

1. Open a Jupyter terminal (use `File > New > Terminal`) and connect to the SSH frontend:

<!-- #raw -->
ssh $IOTLAB_LOGIN@saclay.iot-lab.info
<!-- #endraw -->

2. From there, use the `nc` (or 'netcat') utility to connect to the serial port of the node (replace `<id>` with the right value):

<!-- #raw -->
<login>@saclay:~$ nc nrf52840dk-<id> 20000
<!-- #endraw -->

3. Keep the terminal open and flash the firmware on the node:

```python
!iotlab-node --flash hello-world-iotlab.elf
```

The node is reset after flashing and the application starts: after five seconds, the "Hello from Ariel OS!" message is displayed in the serial console.


### Free up the resources

Since you finished the exercise, stop your experiment to free up the node:

```python
!iotlab-experiment stop
```
