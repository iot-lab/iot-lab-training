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

## Discover UDP socket

In this exercice we will use RIOT [Posix compliant UDP API](https://riot-os.org/api/group__posix__sockets.html) in order to implement two UDP applications: one client will periodically send the "Hello Server" message, the server prints any messages received.
This Posix API provides a set of functions to establish connections or send and receive datagrams using UDP. It is based on the [sock UDP API](https://riot-os.org/api/group__net__sock.html) which can be used on top of any networking stacks (GNRC, LwIP) without modification. The Sock UDP API allows to write portable networking applications. In this exercise, the server listens on port 8888 and the client sends its messages to the IPv6 link-local address of the server.

The entire process can be broken down into following steps :

#### UDP Server

1. Create UDP socket.
2. Bind the socket to server address.
3. Wait until datagram packet arrives from client.
4. Process the datagram packet.

#### UDP Client

1. Create UDP socket.
2. Send message to server.
3. Wait one second
4. Go back to step 2




### Implement server application

1. Go into the server application directory
2. Add the posix sockets module dependency to the Makefile

```mk
USEMODULE += posix_sockets
```

3. In the main.c file, create the server socket

```c
    int server_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket < 0) {
        puts("error initializing socket");
        server_socket = 0;
        return 1;
    }
```

4. Declare and configure the server address

```c
    struct sockaddr_in6 server_addr = {
            .sin6_family = AF_INET6,
            .sin6_port = htons(SERVER_PORT)
    };
```

5. Bind the socket to the server address

```c
    if (bind(server_socket, (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0) {
        server_socket = -1;
        puts("error binding socket");
        return 1;
    }
```

6. Start listening to incoming packets

```c
        int res;
        struct sockaddr_in6 src;
        socklen_t src_len = sizeof(struct sockaddr_in6);
        if ((res = recvfrom(server_socket, server_buffer, sizeof(server_buffer), 0,
                           (struct sockaddr *)&src, &src_len)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("Peer did shut down");
        }
        else {
            printf("Message received: ");
            puts(server_buffer);
        }
```

Compile and verify your application code with native target is then as simple as running the following command

```python
!make -C server
```

<!-- #region -->
### Implement client application

1. Go into the client application directory
2. Add the posix sockets module dependency to the Makefile

```mk
USEMODULE += posix_sockets
```

3. In the main.c file, create the socket

```c
    int s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0) {
        puts("error initializing socket");
        return 1;
    }
```

4. Configure the destination address (based on SERVER_ADDR and SERVER_PORT macros)

```c
    struct sockaddr_in6 dst = {
            .sin6_family = AF_INET6,
            .sin6_port = htons(SERVER_PORT),
            .sin6_scope_id = 5  /* interface identifier */
    };

    /* parse destination address */
    ipv6_addr_from_str((ipv6_addr_t *)&dst.sin6_addr, SERVER_ADDR);

```

5. Send packets to the server


```c
        if (sendto(s, CLIENT_MESSAGE, strlen(CLIENT_MESSAGE), 0,
                   (struct sockaddr *)&dst, sizeof(dst)) < 0) {
            puts("could not send");
        }
```


Compile and verify your application code with native target and the following command
<!-- #endregion -->

```python
!make -C client
```

#### Radio settings

If you are running this training as the same time as other people on the testbed, it is a good idea to change the default radio configuration to avoid too much collision with others.

Use the following cell to give you random values for channel and PAN ID that you will use in the next celluls.

```python
import os,binascii,random
pan_id = binascii.b2a_hex(os.urandom(2)).decode()
channel = random.randint(11, 26)
print('Use CHANNEL={}, PAN_ID=0x{}'.format(channel, pan_id))
```

### Submit an experiment on IoT-LAB

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment with two nodes

```python
!iotlab-experiment submit -n "riot-udp" -d 60 -l 2,archi=m3:at86rf231+site=$SITE
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

From here you will choose one node with the server role and the other one with client role. You need to open a terminal for the server

5. For server node, open a Jupyter terminal (use `File > New > Terminal`) and run the command replacing `<channel> <pan_id>` by the values you obtained in the `Radio settings` section and `<id> <site>`with the right values

<!-- #raw -->
make -C riot/networking/udp/server DEFAULT_CHANNEL=<channel> DEFAULT_PAN_ID=<pan_id> IOTLAB_NODE=m3-<id>.<site>.iot-lab.info flash term
<!-- #endraw -->

```
Configured network interfaces:
Iface  5  HWaddr: F2:9D:F4:58:14:D4
        L2-PDU:1500 MTU:1500  HL:64  Source address length: 6
        Link type: wired
        inet6 addr: fe80::f09d:f4ff:fe58:14d4  scope: local  TNT[1]
        inet6 group: ff02::1
        inet6 group: ff02::1:ff58:14d4
```

On the serial port you should retrieve the server link-local address. In the above example it's **fe80::f09d:f4ff:fe58:14d4**. Keep the terminal open and the server application running.

6. For client node, open a Jupyter terminal (use `File > New > Terminal`) and run the command replacing `<channel> <pan_id>` by the values you obtained in the `Radio settings` section and `<id> <site>` with the right values. For `<link-local-addr>` use the server address obtained above.

<!-- #raw -->
make -C riot/networking/udp/client SERVER_ADDR=<link-local-addr> DEFAULT_CHANNEL=<channel> DEFAULT_PAN_ID=<pan_id> IOTLAB_NODE=m3-<id>.<site>.iot-lab.info flash
<!-- #endraw -->

On the server terminal, you should see the following messages:

<!-- #raw -->
Message received: Hello Server
Message received: Hello Server
Message received: Hello Server
Message received: Hello Server
Message received: Hello Server
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```
