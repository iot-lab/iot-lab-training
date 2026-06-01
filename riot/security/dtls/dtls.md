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

## DTLS communication

In this exercise, you will simply book 2 IoT-LAB M3 devices and use one of the RIOT example to perform an encrypted DTLS communication between them.

The example is based on the [TinyDTLS library](https://github.com/eclipse/tinydtls) which is adapted to RIOT as an external package.

The source code of the examples is available on the [GitHub repository of RIOT](https://github.com/RIOT-OS/RIOT/tree/master/examples/dtls-echo).

### Submit an experiment on the IoT-LAB

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-dtls" -d 20 -l 2,archi=m3:at86rf231+site=$SITE
```

3. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.


4. Get the experiment nodes list:

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

5. Build and flash the application on the devices:

```python
!make BOARD=iotlab-m3 -C ../../RIOT/examples/dtls-echo
```

```python
!iotlab-node --flash ../../RIOT/examples/dtls-echo/bin/iotlab-m3/dtls_echo.bin
```

### Interact with the 2 IoT-LAB M3 devices

1. Open **two** JupyterLab Terminal (File > New > Terminal) and connect to the SSH frontend server in each of them. Replace `<site>` with the right value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

2. From there, use the `nc` (or 'netcat') command to connect the device socket (host is node's network address, port is 20000), from both terminals:

<!-- #raw -->
<login>@<site>:~$ nc m3-<id1> 20000
<!-- #endraw -->

<!-- #raw -->
<login>@<site>:~$ nc m3-<id2> 20000
<!-- #endraw -->

3. On one of the device, use `help` to list the available commands provided by the shell:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
dtlsc                Start a DTLS client
dtlss                Start and stop a DTLS server
reboot               Reboot the node
random_init          initializes the PRNG
random_get           returns 32 bit of pseudo randomness
nib                  Configure neighbor information base
ifconfig             Configure network interfaces
6ctx                 6LoWPAN context configuration tool
<!-- #endraw -->

4. Call `ifconfig` on the device that will be used a server to print its network configuration:

<!-- #raw -->
> ifconfig
ifconfig
Iface  6  HWaddr: 11:15  Channel: 26  Page: 0  NID: 0x23
          Long HWaddr: 3A:5D:F9:65:10:6B:11:15 
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
          AUTOACK  ACK_REQ  CSMA  L2-PDU:102 MTU:1280  HL:64  6LO  
          IPHC  
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::385d:f965:106b:1115  scope: link  VAL
          inet6 group: ff02::1
<!-- #endraw -->

The address of the server is `fe80::385d:f965:106b:1115`.


5. Then start the `dtls` server:

<!-- #raw -->
> dtlss start
<!-- #endraw -->

6. Switch to the other terminal, it will be used as a client and send an encrypted message to the server:

<!-- #raw -->
> dtlsc fe80::385d:f965:106b:1115 "Hello encrypted world"
dtlsc fe80::385d:f965:106b:1115 "Hello encrypted world"
Client: got DTLS Data App -- Hello encrypted world --
<!-- #endraw -->

On the server side, the message is successfuly reveived and echoed to the client:

<!-- #raw -->
Server: got DTLS Data App: --- Hello encrypted world ---        (echo!)
Received alert from client
<!-- #endraw -->

### Free up the resources

Stop your experiment to free up the devices:

```python
!iotlab-experiment stop
```
