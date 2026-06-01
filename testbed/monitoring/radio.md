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

## Radio monitoring

Radio monitoring is an optional feature which measures the radio signal power (RSSI) or can be used as radio packet sniffer for IEEE 802.15.4. It refers to the Control Node dedicated hardware installed on the IoT-LAB node to enable the monitoring. You will learn how to create a Profile monitoring configuration and enable it for your experiment. Moreover you will figure out how to get and analyse the monitoring data.
    


### Launch an experiment

1. Choose your site (grenoble|lille|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment with two nodes and a prebuilt tutorial firmware 

```python
!iotlab-experiment submit -n "radio-rssi" -d 120 -l 2,archi=m3:at86rf231+site=$SITE,tutorial_m3.elf
```

3. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.

4. Check the nodes allocated to the experiment

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

### Monitoring profile

You must create a monitoring profile with the following configuration

* Monitor radio: RSSI mode.
* Channel 11 (used by the prebuilt firmware) and 14 (unused channel)
* Period = 1 ms (duration of one measure for a channel)
* Number of measure by channel = 1 (the number of times of measurements per channel)

```python
!iotlab-profile addm3 -n radio_rssi -rssi -channels 11 14 -rperiod 1 -num 1
```

Apply this monitoring configuration to your nodes

```python
!iotlab-node --update-profile radio_rssi
```

At this stage use `File > New > Terminal` and connect to the IoT-LAB SSH frontend to have access of the node's serial link. You must replace ``<site>`` with the good value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

From the SSH frontend launch the serial_aggregator command and choose one node (``m3-<id1>`` below) to send five big radio packets (with the firmware shell) and visualize the good reception on the other one. You must replace ``<id1>`` with the good value.

<!-- #raw -->
<login>@<site>:~$ serial_aggregator
...
# Hit "Space+Enter" to stop the flood.
m3-<id1>;b # send one big radio packet from the node m3-<id1>. Hit "Enter" to execute the command
1578307588.006162;m3-13;cmd >
1578307588.006417;m3-13;radio > Big packet sent
1578307588.006543;m3-13;
1578307588.009438;m3-14;cmd >
1578307588.011165;m3-14;radio > Got packet from a569 (m3-13). Len: 60 Rssi: -56: 'Big Hello World!: 0 012345678901234567890123456789012345678'
1578307588.011331;m3-14;
m3-<id1>;b
1578307594.166009;m3-13;cmd >
1578307594.166634;m3-13;radio > Big packet sent
1578307594.166832;m3-13;
1578307594.169027;m3-14;cmd >
1578307594.171689;m3-14;radio > Got packet from a569 (m3-13). Len: 60 Rssi: -56: 'Big Hello World!: 1 012345678901234567890123456789012345678'
1578307594.171928;m3-14;
# Continue and send five packets in total
....
# Ctrl^C to exit
<!-- #endraw -->

### Analyse monitoring data

The monitoring data is stored on the SSH frontend server in your home directory. You can find it in the `~/.iot-lab/<exp_id>/consumption/` directory. We use the OML measurement library and you can find a file ``m3_<id>.oml`` for each monitored nodes. Don’t worry if you have empty files, OML library performs caching.

**You have to wait a little and manually stop the experiment to flush the cache.**

```python
!iotlab-experiment stop
```

Retrieve an OML file (scp copy from frontend SSH server to local). Replace `<id>` with the identifier of one the nodes in the previous experiment (`.iot-lab/last` is a symlink to your last experiment directory `.iot-lab/<exp_id>`):

```python
%env NODE_ID=<id>
!scp -o StrictHostKeyChecking=no $IOTLAB_LOGIN@$SITE.iot-lab.info:~/.iot-lab/last/radio/m3_$NODE_ID.oml radio.oml
```

View the monitoring OML file content:

```python
!head -n 20 radio.oml
```

RSSI stands for Received Signal Strength Indication. It is the relative received signal strength in a wireless environment. In IoT-LAB, measured RSSI values provide an indication of the radio power level received by the antenna of the Control Node hardware. The RSSI value is expressed in dBm (Decibel-milliwatts), and represents the power ratio in decibels (dB) of the measured power referenced to one milliwatt (mW). A power level of 0 dBm corresponds to 1mW.

We provide an OML plotting tool which helps you to analyse monitoring data.


```python
%matplotlib widget
from oml_plot_tools import radio
data = radio.oml_load('radio.oml')
data = data[0:-1]
radio.radio_plot(data, 'radio rssi', ('joined'))
```

Check that you observe the peaks in the measured RSSI that happen when sending the big packets on channel 11. Check that you observe only noise on channel 14. Depending on the radio environnement perturbations, you should measure a RSSI near -91 dBm with relative noise.
