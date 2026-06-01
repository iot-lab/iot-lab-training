---
jupyter:
  jupytext:
    text_representation:
      extension: .md
      format_name: markdown
      format_version: '1.3'
      jupytext_version: 1.19.3
  kernelspec:
    display_name: Python 3 (ipykernel)
    language: python
    name: python3
---

## Scan BLE advertisement paquets

In this exercise, you will use one [nRF52DK](https://www.iot-lab.info/docs/boards/nordic-nrf52dk/) to scan BLE advertisement paquets sent by 2 other nRF52DK boards.

BLE support in RIOT is provided by the [external package Nimble](http://doc.riot-os.org/group__pkg__nimble.html) which is an adaption of [Apache Nimble](https://github.com/apache/mynewt-nimble) to RIOT and provides major BLE features.

For exercise, we prepared RIOT applications and Python scripts that will allow you to:
- build an application with shell command to start a BLE scan and to send BLE advertise paquets
- retrieve the scan output and store it in a file on this Juputerlab server
- plot several radio informations related to the caprtured BLE packets

_Note:_ if no nRF52DK are available you can also try with [DWM1001](https://www.iot-lab.info/docs/boards/decawave-dwm1001/) or [nRF52840DK](https://www.iot-lab.info/docs/boards/nordic-nrf52840dk/) .


### Launch an experiment

1. Submit an experiment with 3 nrf52dk boards and during 60 minutes:

```python
!iotlab-experiment submit -n "riot-ble-scan" -d 60 -l 3,archi=nrf52dk:ble+site=saclay
```

2. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.

3. Get the experiment nodes list:

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

### Build the RIOT firmware

```python
!make
```

Now you can flash the fimrware on the 3 nodes of the experiment:

```python
!iotlab-node --flash bin/nrf52dk/ble_scan_rss.bin
```

### Play with the RIOT firmware

Let's start by connecting to the first board that will be used later to scan the BLE advertisement paquets.

1. Go to `File > New > Terminal` and open a serial terminal on the first board. In the command below, replace `<node>` with one of the nodes of your experiment, for example `<node name>-<id>.saclay.iot-lab.info`. Also note that it will output nothing by default, you have to enter commands (see the next steps).

<!-- #raw -->
TERMLOG=$(pwd)/riot/ble/ble_scan/scan_raw.log make IOTLAB_NODE=<node> -C riot/ble/ble_scan term
<!-- #endraw -->

_Note:_  in the command above, we use the `TERMLOG` variable with a file path. This is a RIOT build system trick that will dump all RIOT shell output to a log file. We will use this file later to parse the scanned BLE packets.

2. In the RIOT shell opened by the previous commands, you can check the available commands:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
scan                 trigger a BLE scan
autoadv              manages BLE advertisment
<!-- #endraw -->

Keep this terminal open, we will come back to it later to start the scan.

3. Go to `File > New > Terminal` twice and open a serial terminal on the 2 remaining boards (repeat twice the following command, adapted with the board hostname):

<!-- #raw -->
make IOTLAB_NODE=<node> -C riot/ble/ble_scan term
<!-- #endraw -->

4. In each of these 2 RIOT shells, set the name of the board (`<node>-<id>`) and start the auto-advertisement:

<!-- #raw -->
> autoadv name <node>-<id>
autoadv name <node>-<id>
> autoadv name
autoadv name
Current name = <node>-<id>
> autoadv start
<!-- #endraw -->

5. Now go back to the first RIOT shell and start 100 BLE scan of 100 milliseconds each, we also configure an output in json format that we can parse later:

<!-- #raw -->
> scan
> scan
scan
usage: scan <output_type> <scan_timeout> <scan_cycles>
 - output_type: json or txt for log output
 - scan_timeout: scan timeout in milliseconds
 - scan_cycles: the number of scan cycles
    
> scan json 100 100
{"nodes": [{"addr":["CC","3E","19","71","63","70"],"addr_type":1,"name":"nrf52dk-2","adv_type":0,"ad_data":["02","01","06","0A","08","64","77","6D","31","30","30","31","2D","32","02","0A","00"],"last_rssi":-53,"txpwr_dbm":0,"adv_msg_cnt":826,"first_update":1115111832,"last_update":1145030423}, {"addr":["DB","DD","06","5A","89","E8"],"addr_type":1,"name":"dwm1001-3","adv_type":0,"ad_data":["02","01","06","0A","08","64","77","6D","31","30","30","31","2D","33","02","0A","00"],"last_rssi":-64,"txpwr_dbm":0,"adv_msg_cnt":852,"first_update":1115096137,"last_update":1144961615}]}
...
<!-- #endraw -->

### Free up the resources

You are done with the node, so stop your experiment to free up the devices:

```python
!iotlab-experiment stop
```

### Process the log file

If everything went well, you should have a [scan_raw.log](scan_raw.log) file in the same directory as this notebook.

```python
!head -n 10 scan_raw.log
```

As you can see, this file contains all the output catched by the scanning node, including the shell input commands. Let's cleanup this with the following Python code:

```python
import json

# Clean non unicode characters
content = ""
with open("scan_raw.log", "rb") as f_raw:
    while 1:
        byte = f_raw.read(1)
        try:
            content += byte.decode()
        except UnicodeDecodeError:
            continue
        if not byte:
            break

# Remove non json content
output = []
for line in content.split("\n"):
    try:
        json.loads(line)
    except json.JSONDecodeError:
        continue
    else:
        output.append(line)

# Write to new log file
with open("scan_clean.log", "w") as f_clean:
    f_clean.write("".join(output))
```

The new [scan_clean.log](scan_clean.log) file only contains json lines with the BLE advertisement packets:

```python
!head -n 10 scan_clean.log
```

Let's now parse the content of [scan_clean.log](scan_clean.log). The cell below will create a dictionnary with device names as keys and the list of rssi measured as values:

```python
import json

with open("scan_clean.log") as f_log:
    data = f_log.readlines()

node_data = {}
for line in data:
    line_dict = json.loads(line)
    for node in line_dict["nodes"]:
        if node["name"] == "undefined":
            continue
        if node_data and node["name"] in node_data:
            node_data[node["name"]].append(node["last_rssi"])
        else:
            node_data.update({node["name"]:[node["last_rssi"]]})
```

We can now plot the RSSI timeseries for each node:

```python
import matplotlib.pyplot as plt

fig, axs = plt.subplots(len(node_data), 1, sharey=True, sharex=True, squeeze=False)
for idx, node in enumerate(node_data):
    axs[idx, 0].plot(node_data[node], label=node)
    axs[idx, 0].legend()
    axs[idx, 0].set_ylabel('RSSI (dBm)')
    axs[idx, 0].grid(True)
axs[idx, 0].set_xlabel('sample')
fig.suptitle('RSSI timeseries')
```

Let's now plot the RSSI distributions:

```python
fig, axs = plt.subplots(len(node_data), 1, sharey=True, sharex=True, squeeze=False)
for idx, node in enumerate(node_data):
    axs[idx, 0].hist(node_data[node], label=node)
    axs[idx, 0].legend()
    axs[idx, 0].set_ylabel('frequency')
axs[idx, 0].set_xlabel('RSSI (dBm')
fig.suptitle('RSSI distributions')
```

Congratulations! You completed this BLE scan tutorial with success.
