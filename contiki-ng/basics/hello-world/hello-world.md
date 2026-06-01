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

## First Contiki-NG application

In this exercise, you will build your first Contiki-NG application and run it on an IoT-LAB M3 node.


### Build the Contiki-NG firmwares

#### IoT-LAB platform support

IoT-LAB boards support is not included in the Contiki-NG official repository. It is provided by the [`iot-lab-contiki-ng`]() project. Contiki-NG has a special feature to enable the of use external platform definitions by setting a `ARCH_PATH` environment variable. Thus it will use the `arch` directory pointed by this environment variable, instead of the one present in the official repository.

```python
%env ARCH_PATH = /home/user/iot-lab-training/contiki-ng/iot-lab-contiki-ng/arch
```

#### Build 

Let's define a environment variable pointing to the `hello-world` example to shorten the command lines:

```python
%env APP_DIR = ../../iot-lab-contiki-ng/contiki-ng/examples/hello-world
```

Build the firmware for the `iotlab` target and `m3` board:

```python
!make -C $APP_DIR TARGET=iotlab BOARD=m3
```

### Launch an experiment

1. Choose your site (grenoble|lille|strasbourg):

```python
%env SITE = grenoble
```

2. Submit an experiment named 'hello' of 20 minutes duration, with one M3 node on the choosen site, running the previously compiled firmware:

```python
!iotlab-experiment submit \
    -n "hello" \
    -d 20 \
    -l 1,archi=m3:at86rf231+site=$SITE,$APP_DIR/hello-world.iotlab
```

3. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.


4. Check the resources allocated to the experiment:

```python
!iotlab-experiment get -ni
```

And assign node ID to an environment variable:

```python
%env ID = <id>
```

### Read the serial output

The `hello-world` application prints periodically a message on its serial link, let's read it.

In the Terminal, connect to the SSH frontend server. Replace `<site>` with the good value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

Open a TCP connection to the node on port 20000

<!-- #raw -->
nc m3-<id> 20000
<!-- #endraw -->

You should read a new message each 10 seconds.

If you want to see the Contiki-NG starting message, just reset your node.

```python
!iotlab-node --reset
```

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection will be closed automatically.
