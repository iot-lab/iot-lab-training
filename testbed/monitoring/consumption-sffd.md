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

## Consumption monitoring for Small Form Factor Devices

Consumption monitoring is an optional feature which measures the energy usage of your experiment nodes. It refers to the Control Node dedicated hardware installed on the IoT-LAB node to enable the monitoring. In this documentation you will learn how to create a Profile monitoring configuration and enable it for your experiment. Moreover you will figure out how to get and analyse the monitoring data.


### Monitoring profile for Small Form Factor Devices

You must create a monitoring profile with the following configuration (adda8)

* Monitor consumption: current, voltage and power.
* Period: 8244 µs
* Average: 4

These settings will give you a sampling period of P = 8.244 ms * 4 * 2 = 65.95 ms. You can see additional informations about sampling at the end of this tutorial

```python
!iotlab-profile adda8 -n consumption_sffd -voltage -current -power -period 8244 -avg 4
```

### Experiment with IOT-LAB A8-M3 board

1. Choose your site (grenoble|strasbourg|saclay):

```python
%env SITE=grenoble
```

2. Submit an experiment with one node and use the monitoring profile for Small Form Factor Devices (consumption_sffd). We do not deploy a specific firmware for the MCU behind the SFFD board. The SFFD board, in this example the IoT-LAB A8-M3, boots with a default Linux Yocto OS.

```python
!iotlab-experiment submit -n "consumption_a8" -d 5 -l 1,archi=a8:at86rf231+site=$SITE,,consumption_sffd
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

#### Analyse monitoring data

The monitoring data is stored on the SSH frontend server in your home directory. You can find it in the `~/.iot-lab/<exp_id>/consumption/` directory. We use the OML measurement library and you can find a file ``a8_<id>.oml`` for each monitored nodes. Don’t worry if you have empty files, OML library performs caching.

**You have to wait a little and manually stop the experiment to flush the cache.**

```python
!iotlab-experiment stop
```

Let's retrieve an OML file from the IoT-LAB SSH frontend. Replace `<id>` with the identifier of one the nodes in the previous experiment (`.iot-lab/last` is a symlink to your last experiment directory `.iot-lab/<exp_id>`):

```python
%env NODE_ID=<id>
!scp -o StrictHostKeyChecking=no $IOTLAB_LOGIN@$SITE.iot-lab.info:~/.iot-lab/last/consumption/a8_$NODE_ID.oml consumption-a8.oml
```

Print the beginning of the monitoring OML file content:

```python
!head -n 20 consumption-a8.oml
```

You can see the last three columns of the file which correspond respectively to the power, voltage and current measurements.

We provide an OML plotting tool which helps you to analyse monitoring data.


```python
%matplotlib widget
from oml_plot_tools import consum
data = consum.oml_load('consumption-a8.oml')
data = data[0:4000]
consum.consumption_plot(data, 'consumption', ('power'))
```

You can observe energy used by the Linux system startup and, in particular, the gradual switch-on of the node's peripherals.


### Experiment with Raspberry Pi 3 boards 

1. Choose your site (grenoble):

```python
%env SITE=grenoble
```

2. Submit an experiment with one node and use the monitoring profile for Small Form Factor Devices (consumption_sffd). We do not deploy a specific firmware for the MCU behind the SFFD board. The SFFD board, in this example the Raspberry Pi 3 board, boots with a default Linux Yocto OS.

```python
!iotlab-experiment submit -n "consumption_rpi3" -d 5 -l 1,archi=rpi3:at86rf233+site=$SITE,,consumption_sffd
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

#### Analyse monitoring data

The monitoring data is stored on the SSH frontend server in your home directory. You can find it in the `~/.iot-lab/<exp_id>/consumption/` directory. We use the OML measurement library and you can find a file ``rpi3_<id>.oml`` for each monitored nodes. Don’t worry if you have empty files, OML library performs caching.

**You have to wait a little and manually stop the experiment to flush the cache.**

```python
!iotlab-experiment stop
```

Let's retrieve an OML file from the IoT-LAB SSH frontend. Replace `<id>` with the identifier of one the nodes in the previous experiment (`.iot-lab/last` is a symlink to your last experiment directory `.iot-lab/<exp_id>`):

```python
%env NODE_ID=<id>
!scp -o StrictHostKeyChecking=no $IOTLAB_LOGIN@$SITE.iot-lab.info:~/.iot-lab/last/consumption/rpi3_$NODE_ID.oml consumption-rpi3.oml
```

Print the beginning of the monitoring OML file content:

```python
!head -n 20 consumption-rpi3.oml
```

You can see the last three columns of the file which correspond respectively to the power, voltage and current measurements.

We provide an OML plotting tool which helps you to analyse monitoring data.

```python
%matplotlib widget
from oml_plot_tools import consum
data = consum.oml_load('consumption-rpi3.oml')
data = data[0:4000]
consum.consumption_plot(data, 'consumption', ('power'))
```
