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

## UDP networking using the Sock API

In this exercise, we propose to implement 2 UDP applications:
- one client that periodically sends the "Hello Server" message
- one server that echoes all messages received

The server listens on port 8888. The client sends its messages to the IPv6 link-local address of the server.

Both applications will be based on the [UDP sock API](http://doc.riot-os.org/group__net__sock__udp.html) of RIOT.

### Implement the server application

1. Open the server application [Makefile](server/Makefile) and add the required network modules:

```make
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp
```

3. Open the server [main.c](server/main.c) file and the sock udp header include:

```c
#include "net/sock/udp.h"
```

4. In the _main_ function, create the UDP sock endpoint:

```c
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = SERVER_PORT;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }
```

5. Finally, in the while loop, implement the UDP packet reception logic. Each received packet is sent back to the sender:

```c
        sock_udp_ep_t remote;
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {

            printf("Message received: %s\n", (char*)buf);

            if (sock_udp_send(&sock, buf, res, &remote) < 0) {
                puts("Error sending reply");
            }
        }
```

6. Verify the build of the server application (iotlab-m3 is the default board in the Makefile)

```python
!make -C server
```

<!-- #region -->
### Implement the client application


1. Open the client application [Makefile](client/Makefile) and add the required network modules:

```mk
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp
```

2. Open the [main.c](client/main.c) file and add the IPv6/UDP header includes:

```c
#include "net/netif.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
```

3. At the beginning of the _main_ function, add the code to create the sock UDP endpoint:

```c
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = 0xabcd;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }
```

4. Configure the remote destination address (using `SERVER_ADDR`) and port (using `SERVER_PORT`):

```c
    sock_udp_ep_t remote = { .family = AF_INET6, .netif = 5  };
    remote.port = SERVER_PORT;

    /* Convert server address from string to ipv6_addr_t */
    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, SERVER_ADDR) == NULL) {
        puts("Cannot convert server address");
        sock_udp_close(&sock);
        return 1;
    }
```

5. In the while loop, send the message via UDP to the server:

```c
        if (sock_udp_send(&sock, CLIENT_MESSAGE, sizeof(CLIENT_MESSAGE),
                          &remote) < 0) {
            puts("Error sending message");
            sock_udp_close(&sock);
            return 1;
        }
```

6. Now that the message is sent, the client can wait for the reply from the server:

```c
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
                                NULL)) < 0) {
            if (res == -ETIMEDOUT) {
                puts("Timed out");
            }
            else {
                puts("Error receiving message");
            }
        }
        else {
            printf("Reply received: \"");
            for (int i = 0; i < res; i++) {
                printf("%c", buf[i]);
            }
            puts("\"");
        }
```

6. Verify the build of the server application (iotlab-m3 is the default board in the Makefile)
<!-- #endregion -->

```python
!make -C client
```

### Submit an experiment on IoT-LAB

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
%env SITE=saclay
```

2. Submit an experiment with two nodes

```python
!iotlab-experiment submit -n "riot-udp-sock" -d 60 -l 2,archi=m3:at86rf231+site=$SITE
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

```python
make -C riot/networking/udp-sock/server DEFAULT_CHANNEL=<channel> DEFAULT_PAN_ID=<pan_id> IOTLAB_NODE=m3-<id>.<site>.iot-lab.info flash term
```

After a few seconds, the configured interface is printed as follows:

<!-- #raw -->
Configured network interfaces:
Iface  5  HWaddr: F2:9D:F4:58:14:D4
        L2-PDU:1500 MTU:1500  HL:64  Source address length: 6
        Link type: wired
        inet6 addr: fe80::f09d:f4ff:fe58:14d4  scope: local  TNT[1]
        inet6 group: ff02::1
        inet6 group: ff02::1:ff58:14d4
<!-- #endraw -->

With the printed message, you can retrieve the server link-local address. In the above example it's **fe80::f09d:f4ff:fe58:14d4**. Keep the terminal open and the server application running.

6. For client node, open a nes Jupyter terminal (use `File > New > Terminal`) and run the command replacing `<channel> <pan_id>` by the values you obtained in the `Radio settings` section and `<id> <site>` with the right values. For `<link-local-addr>` use the server address obtained above.

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

On the client terminal, you should see the following messages:

<!-- #raw -->
Reply received: "Hello Server"
Reply received: "Hello Server"
Reply received: "Hello Server"
Reply received: "Hello Server"
Reply received: "Hello Server"
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```
