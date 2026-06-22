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

## RPL


#### Radio settings

If you are running this training as the same time as other people on the testbed, it is a good idea to change the default radio configuration to avoid too much collision with others.

Use the following cell to give you random values for channel and PAN ID.

```python
import os,binascii,random
pan_id = binascii.b2a_hex(os.urandom(2)).decode()
channel = random.randint(11, 26)
print('Use CHANNEL={}, PAN_ID=0x{}'.format(channel, pan_id))
```

For these values to be taken into account you can set an environment variable for the notebook. Modify the values in the cell below with those obtained and execute it.

```python
%env CHANNEL=11
```

```python
%env PAN_ID=0xBEEF
```

### Submit an experiment on IoT-LAB

You will use the `gnrc_networking` example from RIOT.
Let's define an environment variable to shorten the command lines.

```python
%env APP_DIR = ../../RIOT/examples/networking/gnrc/networking
```

1. Compile the example:

```python
!make -C $APP_DIR BOARD=iotlab-m3 DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID
```

2. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

3. Submit an experiment with three nodes and the previous compiled firware:

```python
!iotlab-experiment submit -n "riot-rpl" -d 60 -l 3,archi=m3:at86rf231+site=$SITE,$APP_DIR/bin/iotlab-m3/gnrc_networking.elf
```

4. Wait for the experiment to be running:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.


5. Check the nodes allocated to the experiment; one of them will have to be be chosen as RPL root node:

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

### Launch the Serial Aggregator

From here, open a Jupyter terminal (use `File > New > Terminal`).

Connect to the SSH frontend and replace `<site>` with the right value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

From the SSH frontend launch the serial aggregator:

<!-- #raw -->
<login>@<site>:~$ serial_aggregator
<!-- #endraw -->

By default, the serial_aggregator will try to connect to all the nodes of your current experiment.

The serial link output of all nodes will be printed here. You are also able to write to nodes serial link. By default, the writen message is sent to all nodes, but you can prepend the message by the node ID to send it to a particular node.


### Start RPL

Before starting RPL, an IPv6 address must be configured for the RPL root node. Since we just need here to communicate locally, the following cell will generate a unique local address randomly, to not have the same address than other people doing this training (fd00::1 in the command examples).

```python
import os,binascii,random
address = binascii.b2a_hex(os.urandom(1)).decode()
print('Root address is fd{}::1'.format(address))
```

In this tutorial, we chose node 1 as root. Replace the ID by the one you have chosen from your nodes and the address by the one given just above.

<!-- #raw -->
m3-1;ifconfig 6 add fd00::1
<!-- #endraw -->

Afteerwards, RPL have to be initialized on interface 6 for all nodes:

<!-- #raw -->
rpl init 6
<!-- #endraw -->

Now, a DODAG can be started on the root node with the instance id _1_ and the root's configured IPv6 address as the DODAG-ID:

<!-- #raw -->
m3-1;rpl root 1 fd00::1
<!-- #endraw -->

The `rpl` command can be used to show information about the RPL state from the root:

<!-- #raw -->
m3-1;rpl
<!-- #endraw -->

```
1575919912.736839;m3-1;> rpl
1575919912.737875;m3-1;instance table:        [X]
1575919912.738071;m3-1;parent table:  [ ]     [ ]     [ ]
1575919912.738210;m3-1;
1575919912.740233;m3-1;instance [1 | Iface: 6 | mop: 2 | ocp: 0 | mhri: 256 | mri 0]
1575919912.742806;m3-1;       dodag [fd00::1 | R: 256 | OP: Router | PIO: on | TR(I=[8,20], k=10, c=0, TC=107s)]
```


The last line indicates that this node is part of the DODAG, with a rank of `R: 256`.

Call the samecommand from another node:

<!-- #raw -->
m3-2;rpl
<!-- #endraw -->

```
1575919904.549503;m3-2;> rpl
1575919904.549748;m3-2;instance table:        [X]
1575919904.550382;m3-2;parent table:  [X]     [ ]     [ ]
1575919904.550580;m3-2;
1575919904.551289;m3-2;instance [1 | Iface: 6 | mop: 2 | ocp: 0 | mhri: 256 | mri 0]
1575919904.553231;m3-2;       dodag [fd00::1 | R: 512 | OP: Router | PIO: on | TR(I=[8,20], k=10, c=1, TC=52s)]
1575919904.554237;m3-2;               parent [addr: fe80::1711:6b10:65f7:550a | rank: 256]
```


This node is part of the same DODAG, but with a rank of `512`. Furthermore, it has a preferred parent with a rank of `256` with a link-local IPv6 address that matches that of the root. You can check it with the `ifconfig` command: 

<!-- #raw -->
m3-1;ifconfig
<!-- #endraw -->

```
1575920479.654471;m3-1;> ifconfig
1575920479.656100;m3-1;Iface  6  HWaddr: 55:0A  Channel: 26  Page: 0  NID: 0x23
1575920479.656948;m3-1;          Long HWaddr: 15:11:6B:10:65:F7:55:0A 
1575920479.658778;m3-1;           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
1575920479.660791;m3-1;          AUTOACK  ACK_REQ  CSMA  L2-PDU:102 MTU:1280  HL:64  RTR  
1575920479.660987;m3-1;          6LO  IPHC  
1575920479.661790;m3-1;          Source address length: 8
1575920479.662788;m3-1;          Link type: wireless
1575920479.663785;m3-1;          inet6 addr: fe80::1711:6b10:65f7:550a  scope: local  VAL
1575920479.665779;m3-1;          inet6 addr: fd00::1  scope: global  VAL
[...]
```


Note that all participating nodes in the RPL-DODAG have configured automatically an IPv6 address matching the prefix from the DODAG-ID. Verify with `ifconfig` on your two others nodes.


### Show neighbors

You can show the Neighbor Information Base with the `nib` command, to explore the DODAG, from the root:

<!-- #raw -->
m3-1;nib route
<!-- #endraw -->

```
1575921101.414540;m3-1;> nib route
1575921101.415532;m3-1;fd00::/64 dev #6
1575921101.416389;m3-1;fd00::1711:6b10:65fb:850a/128 via fe80::1711:6b10:65fb:850a dev #6
1575921101.418222;m3-1;fd00::1711:6b10:65f9:ac2a/128 via fe80::1711:6b10:65f9:ac2a dev #6
```


and from another node:

<!-- #raw -->
m3-2;nib route
<!-- #endraw -->

```
1575921285.989180;m3-2;> nib route
1575921285.989336;m3-2;fd00::/64 dev #6
1575921285.990040;m3-2;default* via fe80::1711:6b10:65f7:550a dev #7
```


### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH and the ethos process will be closed automatically.
