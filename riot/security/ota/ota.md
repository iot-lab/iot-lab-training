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

## SUIT Firmware update

In this exercise, we propose to perform firmware updates on IoT-LAB M3 devices.

The principle of this exercise is as follows:
- An updatable firmware is built and flashed first on the device.
- This firmware provides a CoAP server to receive update notifications and is able to download new firmware images from another CoAP server running on an IoT-LAB SSH frontend
- On the frontend, the CoAP server exposes the images located in a directory on the server: as a result, publishing new updates is just a matter of putting files in a directory
- The update is trigger manually from the device itself with a shell command

The application used in this exercise implements a version of the SUIT (Softwate Update for the Internet of Things) protocol that is in the standardization process at IETF.

As you saw previously in other networking exercises, to be able to reach the device from the SSH frontend, you'll have to run ethos: this will create a bridge between the serial port of the device and the SSH frontend.

#### What is an updatable firmware?

For a firmware to become updatable, several conditions must be met:
- The flash memory on the device must be devided in 3 parts:
  1. The beginning of the flash is reserved to a small and very simple bootloader application whose goal is to choose and verify the application to boot
  2. The rest of the flash is divided into 2 equally sized memory *slots*. Each slot may contain a binary firmware for a given application. At the beginning of each slot are located some metadata which describe the version of the application and the name of the application. Since only one application can run at a time on a microcontroller, only one slot is active at a time.
 
- The firmware in the active slot must contain some logic for retrieving an update. This update is copied on-the-fly to the other slot. Once the update has completed, the application reboots the device. During the boot phase, the bootloader chooses the most recent version between the 2 slots and launches the application on it.

To summarize, initially, **an updatable device must be provisionned with a bootloader and a slot containing an application with the update logic**.

### Discover the firmware update mechanism

As a first step, we propose you to test the basic firmware update mechanism only between the SSH frontend and the device: the device will be provisioned with an updatable firmware, a bridge between the SSH frontend and the device will be setup using `ethos` and the new firmware will be stored on the SSH frontend and exposed to the device on a CoAP server running on the frontend.

#### **Step 1:** prepare the device


1. Choose your site (grenoble, lille, strasbourg or saclay):

```python
%env SITE=grenoble
```

2. Start an experiment on IoT-LAB with 1 IoT-LAB M3 device:

```python
!iotlab-experiment submit -n "riot-ota" -d 120 -l 1,archi=m3:at86rf231+site=$SITE
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

5. Provision the device with the updatable application

To provision the device, e.g to flash a binary that contains both the bootloader and the application for the first slot (*slot0*), RIOT provides the `riotboot/flash-combined-slot0` target. Let's use it with IoT-LAB to provision the IoT-LAB.

Generate the elliptic curves key use to sign the SUIT manifest, build and flash the RIOT firmware:

```python
!mkdir -p ~/.local/share/RIOT/keys
!openssl genpkey -algorithm ed25519 -out ~/.local/share/RIOT/keys/default.pem
!make ETHOS_BAUDRATE=500000 IOTLAB_NODE=auto flash
```

<!-- #region -->
**Important:** during this step, several things are automatically performed by the RIOT build system:
- The bootloader and the updatable application are built
- A keypair is generated: according to the SUIT update protocol, the private key will be used to sign the update manifest and the public is stored in the updatable application. This mechanism ensures the application is able to reject non authentic updates and increases security during the update.


At this stage, you have a running firmware that is updatable by following the SUIT update protocol.


By default, the updatable application running now on the device is using the `ethos` mechanism. This is why the build command is using `ETHOS_BAUDRATE=500000`. In fact, this is the same as the `gnrc_border_router` that is used in the networking notebooks
but here `ethos` is just used to provide a link between the SSH frontend and the device. The device itself doesn't use its radio interface, for the moment.

And like for the border router, a bridge must be configured.

So let's setup the network between the SSH frontend and the device!
<!-- #endregion -->

#### **Step 2:** setup the network

The setup of the network can only be performed from the IoT-LAB SSH frontend.

Use a Terminal (File > New > Terminal) to connect to the IoT-LAB SSH frontend. Replace `<site>` with the right value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

From the frontend SSH we need to create a tap network interface and choose a public IPv6 prefix available. As it is a shared environment you must check before which tap network interfaces are already in use:

<!-- #raw -->
<login>@<site>:~$ ip addr show | grep tap
1406: tap0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 500
...
<!-- #endraw -->

You can find below the list of available IPV6 prefix by sites

| Site       | First Prefix        | Last Prefix         | Number of Prefix   |
|------------|---------------------|---------------------|--------------------|
| Grenoble   | 2001:660:5307:3100  | 2001:660:5307:317f  | 128                |
| Lille      | 2001:660:4403:0480  | 2001:660:4403:04ff  | 128                |
| Saclay     | 2001:660:3207:04c0  | 2001:660:3207:04ff  | 64                 |
| Strasbourg | 2a07:2e40:fffe:00e0 | 2a07:2e40:fffe:00ff | 32                 |

As it's a shared environment you must check before. Visualize which prefix are already used

<!-- #raw -->
<login>@<site>:~$ ip -6 route
2001:660:5307:3100::/64 dev tun0  proto kernel  metric 256  mtu 1500 advmss 1440 hoplimit 4294967295
<!-- #endraw -->

To expose the firmware update via CoAP, you have to open an UDP port on the frontend. The default CoAP UDP port is 5683 but it might be already used by another user. Check which UDP ports are already used:

<!-- #raw -->
<login>@<site>:~$ netstat -a | grep udp
udp        0      0 0.0.0.0:4000            0.0.0.0:*                          
udp        0      0 0.0.0.0:38696           0.0.0.0:*                          
udp        0      0 0.0.0.0:sunrpc          0.0.0.0:*                          
udp        0      0 localhost:848           0.0.0.0:*                          
udp        0      0 0.0.0.0:1017            0.0.0.0:*                          
udp6       0      0 [::]:4000               [::]:*                             
udp6       0      0 [::]:5683               [::]:*                             
udp6       0      0 [::]:sunrpc             [::]:*                             
udp6       0      0 [::]:1017               [::]:*                             
udp6       0      0 [::]:58895              [::]:* 
<!-- #endraw -->

One can see that the UDP port 5683 is already in use, so you have to choose a different one between 5000 and 10000, for example 5684.

You can now start `ethos` using the free tap network interface, the free IPv6 prefix you choose, the free UDP port and the id of node booked in your experiment:

<!-- #raw -->
<login>@<site>:~$ sudo ethos_uhcpd.py m3-<id> tap<num> <ipv6_prefix>::1/64 --udp-ports <udp_port>
net.ipv6.conf.tap0.forwarding = 1
net.ipv6.conf.tap0.accept_ra = 0
Switch from 'root' to '<login>'
Joining IPv6 multicast group...
entering loop...
----> ethos: sending hello.
----> ethos: activating serial pass through.
----> ethos: hello reply received
<!-- #endraw -->

**Warning:** It is very important to verify that the message `Warning: port udp:<udp_port> already in use, cannot open.` is not printed at the beginning of the `ethos_uhcpd` command (before the `Switch from 'root' to '<login>'` message). If it's the case, choose another UDP port!


Keep the `ethos` process running and verify that the RIOT shell is working through `ethos`, check available commands using `help` and list the configured interface using `ifconfig`:

<!-- #raw -->
----> ethos: sending hello.
----> ethos: activating serial pass through.
----> ethos: hello reply received
help
help
Command              Description
---------------------------------------
reboot               Reboot the node
ping6                Ping via ICMPv6
random_init          initializes the PRNG
random_get           returns 32 bit of pseudo randomness
nib                  Configure neighbor information base
ifconfig             Configure network interfaces
suit                 Trigger a SUIT firmware update
> ifconfig
ifconfig
Iface  5  HWaddr: 02:AE:24:E3:07:90 
          L2-PDU:1500 MTU:1500  HL:64  RTR  
          RTR_ADV  
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::ae:24ff:fee3:790  scope: link  VAL
          inet6 addr: fe80::2  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ffe3:790
          inet6 group: ff02::1:ff00:2
          
> 
<!-- #endraw -->

Now note the link local address of the device, here it's **fe80::ae:24ff:fee3:790**. It will be used later to check the connectivity between the device and the SSH frontend.

Still from the RIOT shell, you can also verify that the device can ping the server ipv6 address. On the grenoble SSH frontend, the global IPv6 address is **2001:660:5307:30ff::5**:

<!-- #raw -->
> ping 2001:660:5307:30ff::5
ping 2001:660:5307:30ff::5
12 bytes from 2001:660:5307:30ff::5: icmp_seq=0 ttl=64 time=127.871 ms
12 bytes from 2001:660:5307:30ff::5: icmp_seq=1 ttl=64 time=47.885 ms
12 bytes from 2001:660:5307:30ff::5: icmp_seq=2 ttl=64 time=47.863 ms
<!-- #endraw -->

**warning:** the IPv6 address of the SSH server is different on the other sites (lille, strasbourg, saclay). Use the following command to get it:

<!-- #raw -->
<login>@<site>:~$ ip addr show ens3 | grep inet6 | grep global
inet6 2001:660:3207:4bf::17/64 scope global
<!-- #endraw -->

Here the IPv6 address of the SSH server is **2001:660:3207:4bf::17**


Open a second terminal with menu `File > New > Terminal` and from there connect via SSH to the frontend. Then verify that the device can be reached (replace `tap<num>` with the value set in the ethos command above):

<!-- #raw -->
<login>@<site>:~$ ping6 -c 3 fe80::ae:24ff:fee3:790%tap<num>
PING fe80::ae:24ff:fee3:790%tap0(fe80::ae:24ff:fee3:790%tap0) 56 data bytes
64 bytes from fe80::ae:24ff:fee3:790%tap0: icmp_seq=1 ttl=64 time=38.8 ms
64 bytes from fe80::ae:24ff:fee3:790%tap0: icmp_seq=2 ttl=64 time=49.7 ms
64 bytes from fe80::ae:24ff:fee3:790%tap0: icmp_seq=3 ttl=64 time=47.4 ms

--- fe80::ae:24ff:fee3:790%tap0 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 38.805/45.348/49.744/4.723 ms
<!-- #endraw -->

The network is in place!

Keep the `ethos` terminal open until the end of this notebook.


#### **Step 3:** publish the update

To publish the update, we must setup the CoAP server that will host the files on the SSH frontend. To download the update, the device will just perform CoAP GET request from this server.

There are 2 things to take into account in the setup:
- the SSH frontend provides the `aiocoap-fileserver` which is a tool based on the [aiocoap](https://aiocoap.readthedocs.io/en/latest/) Python library to expose files as CoAP resources. This is what will be used to expose updates to the devices
- the firmware is generated in this notebook, so it has to be copied manually on the SSH frontend. You will use a simple `scp` to do this
- the CoAP must listen on the same UDP port that was chosen when configuring the network

1. Let's start the CoAP server first. From the terminal that was used to ping the device from the SSH frontend (the last one that you used), run the following commands (replace `<udp_port>` with the port you selected):

<!-- #raw -->
<login>@<site>:~$ mkdir -p ~/ota/fw/iotlab-m3
<login>@<site>:~$ aiocoap-fileserver -p <udp_port> ~/ota
<!-- #endraw -->

Keep the server running.


2. Publish the new firmware version:

RIOT provides the `suit/publish` to build and publish the generated update automatically. The CoAP server variable `SUIT_COAP_SERVER` must be set when publishing because it will be written in the SUIT manifest to tell the device at which url to download the update.
`SUIT_COAP_SERVER` is the url of the server; e.g IPv6 address + UDP port, on the grenoble site it will be `[2001:660:5307:30ff::5]:<udp_port>` (make sure to adapt the IPv6 address depending on the IoT-LAB site you are currently using).

```python
!make SUIT_COAP_SERVER=[2001:660:5307:30ff::5]:<udp_port> ETHOS_BAUDRATE=500000 suit/publish
```

All generated files are located in `/home/user/iot-lab-training/riot/security/ota/bin/iotlab-m3/` as raw binary files:
- `suit_update-riot.suit_signed.<version>.bin` is the SUIT manifest signed
- `suit_update-slot1.<version>.riot.bin` is the new application, built for slot1, in case the running application on the device is on slot0
- `suit_update-slot0.<version>.riot.bin` is the new application, built for slot0, in case the running application on the device is on slot1

Let's copy them to firmware updates location on the SSH frontend. Replace `<site>` with the correct value.

```python
!scp bin/iotlab-m3/*.bin $IOTLAB_LOGIN@$SITE.iot-lab.info:~/ota/fw/iotlab-m3/.
```

Now it is time to download this update. The RIOT shell provides the `suit` command that initiates the SUIT procedure from the url of the update manifest.

On our CoAP server, the SUIT manifest of the latest version can be downloaded at `coap://[2001:660:5307:30ff::5]:<udp_port>/fw/iotlab-m3/suit_update-riot.suit_signed.latest.bin`.

In the RIOT shell available via `ethos`, trigger the firmware update (replace `<udp_port>` with the UDP port configured in your network and make sure to adapt the IPv6 address depending on the IoT-LAB site you are currently using):

<!-- #raw -->
> suit coap://[2001:660:5307:30ff::5]:<udp_port>/fw/iotlab-m3/suit_update-riot.suit_signed.latest.bin
<!-- #endraw -->

If everything succeeds, the RIOT application performs several steps:
1. it downloads the manifest
2. if checks the manifest (signature, version, etc)
3. if all checks succeed, it fetches the new firmware image
4. once the new firmware image download is complete, it verifies the hash of the image compared to the one given by the manifest
5. it reboots the device to the new application (check the update of the version number and the active slot)


Here is the expected output of the update trigger:

<!-- #raw -->
> suit coap://[2001:660:5307:30ff::5]:<udp_port>/fw/iotlab-m3/suit_update-riot.suit_signed.latest.bin
suit coap://[2001:660:5307:30ff::5]:<udp_port>/fw/iotlab-m3/suit_update-riot.suit_signed.latest.bin
suit_coap: trigger received
suit_coap: downloading "coap://[2001:660:5307:30ff::5]:<udp_port>/fw/iotlab-m3/suit_update-riot.suit_signed.latest.bin"
> suit_coap: got manifest with size 521
suit: verifying manifest signature
suit: validated manifest version
)riotboot_hdr_validate: riotboot_hdr magic number invalid
Manifest seq_no: 1609352410, highest available: 1609352332
suit: validated sequence number
)Formatted component name: 
Comparing manifest offset 1000 with other slot offset
Comparing manifest offset 40800 with other slot offset
validating vendor ID
Comparing 547d0d74-6d3a-5a92-9662-4881afd9407b to 547d0d74-6d3a-5a92-9662-4881afd9407b from manifest
validating vendor ID: OK
validating class id
Comparing 4448e64b-4d23-551e-b143-622726145213 to 4448e64b-4d23-551e-b143-622726145213 from manifest
validating class id: OK
Comparing manifest offset 1000 with other slot offset
Comparing manifest offset 40800 with other slot offset
SUIT policy check OK.
Formatted component name: 
riotboot_flashwrite: initializing update to target slot 1
Fetching firmware |█████████████████████████| 100%
Finalizing payload store
Verifying image digest
Starting digest verification against image
Install correct payload
Verifying image digest
Starting digest verification against image
Install correct payload
Image magic_number: 0x544f4952
Image Version: 0x5fecc4da
Image start address: 0x08040c00
Header chksum: 0xb0aed66c

suit_coap: rebooting...
----> ethos: hello received
Failed to send flush request: Operation not permitted
gnrc_uhcpc: Using 4 as border interface and 0 as wireless interface.
gnrc_uhcpc: only one interface found, skipping setup.
main(): This is RIOT! (Version: labs)
RIOT SUIT update example application
Running from slot 1
Image magic_number: 0x544f4952
Image Version: 0x5fecc4da
Image start address: 0x08040c00
Header chksum: 0xb0aed66c

suit_coap: started.
Starting the shell
> 
<!-- #endraw -->

### Going further

Here are a couple of ideas if you want to go further with firmware updates:

#### Extend the application

The default updatable application is running a CoAP server with only one resource available at `/riot/board` endpoint. You can add new endpoints to the application for reading the temperature or pressure from one of the sensors available on the IoT-LAB M3 device and publish an update of your new application (starting from step 3 in the previous section).

#### Perform over-the-air updates

Until now, in this notebook, firmware updates were only performed over a wired connection between the SSH frontend and the IoT-LAB M3 device. To perform the update over a wireless 802.15.4 radio, you can:
- start an experiment with 2 IoT-LAB M3 devices, one will act as a border router between the wired network and a wireless network
- setup a border router, see [ipv6](../../networking/ipv6/ipv6.md)
- disable `ethos` in each updatable firmwares by adding `ETHOS=0` to the build command line
- enable the radio by prepending `USEMODULE=gnrc_netdev_default` to the build command line

Example of provisioning command line:
```
USEMODULE=gnrc_netdev_default ETHOS=0 make IOTLAB_NODE=auto riotboot/flash-combined-slot0
```

Example of publish command line:
```
USEMODULE=gnrc_netdev_default ETHOS=0 SUIT_COAP_SERVER=[2001:660:5307:30ff::5]:<udp_port> make suit/publish
```

The firmware update takes significantly more time when performed over-the-air, but this is expected since the bandwidth is much smaller.

<!-- #region -->
### Free up the resources

1. Stop the `ethos` process by pressing the `Ctrl+C` keyboard shortcut in the terminal that is running it.


2. Stop your experiment to free up the device:
<!-- #endregion -->

```python
!iotlab-experiment stop
```
