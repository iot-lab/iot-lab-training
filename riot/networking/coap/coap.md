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

## Discover CoAP protocol

In this excercice we will discover gcoap which provides a high-level interface for writing CoAP messages via RIOT's sock networking API. For a server, gcoap accepts a list of resource paths with callbacks for writing the response. For a client, gcoap provides a function to send a request, with a callback for reading the server response.

We propose to expose different kind of CoAP resources:
* <code>/riot/cpu</code> returns the name of the cpu running RIOT
* <code>/temperature</code> returns LPS331ap sensor value (provided by iotlab-m3 board and supported by RIOT) 
* <code>/value</code> returns an arbitrary value, equal to 0 by default. This value can be updated using PUT request

With the client you will learn how to discover CoAP server resources and request them.

### Add CoAP resources

1. Add cpu resource

* Edit [main.c](main.c) file and add an entry in the resources list with the form {\<path\>, \<method\>, \<handler function\>, \<context\>}. You can help yourself and follow the way it was done for riot board CoAP resource
    
```c
    { "/riot/cpu", COAP_GET, _riot_cpu_handler, NULL },
```

* Declare the cpu handler
    
```c
static ssize_t _riot_cpu_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);
```

* Implement the cpu handler

    For the implementation copy the content of <code>_riot_board_handler</code>  in <code>_riot_cpu_handler</code>  and replace <code>RIOT_BOARD</code> by <code>RIOT_CPU</code> variable.

2. Add temperature resource  
    
* Add LPS331ap module driver into the [Makefile](Makefile)

```mk
USEMODULE += lps331ap
```

* Edit main.c file and include the sensor driver

```c
#include "lpsxxx.h"
#include "lpsxxx_params.h"
```

* Add a sensor variable 
   
```c
static lpsxxx_t sensor;
```
    
* Initialize of the driver in the main method

```c
    if (lpsxxx_init(&sensor, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("LPS331AP initialization failed");
        return 1;
    }
    lpsxxx_enable(&sensor);
```

* Add an entry in resources list

```c
    { "/temperature", COAP_GET, _temperature_handler, NULL },
```

* Declare the temperature handler

```c
static ssize_t _temperature_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);
```

* Implement the temperature handler

```c
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    char response[32];
    int16_t temp;
    lpsxxx_read_temp(&sensor, &temp);
    int temp_abs = temp / 100;
    temp -= temp_abs * 100;
    sprintf(response, "%2i.%02i°C",temp_abs, temp);

    /* write the temperature value in the response buffer */
    if (pdu->payload_len >= strlen(response)) {
        memcpy(pdu->payload, response, strlen(response));
        return resp_len + strlen(response);
    }
    else {
        puts("gcoap: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
```

2. Add value resource 

* Add a _value variable 

```c
static uint16_t _value = 0;
```

* Add an entry in resources list. You can note that instead of previous cpu and temperature resources with GET request we add PUT and POST method to update the value resource.

```c
    { "/value", COAP_GET | COAP_PUT | COAP_POST, _value_handler, NULL },
```

* Declare the value handler

```c
static ssize_t _value_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);
```

* Implement the value handler

```c
/* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));
    switch(method_flag) {
        case COAP_GET:
            gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
            coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
            size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

            /* write the response buffer with the request count value */
            resp_len += fmt_u16_dec((char *)pdu->payload, _value);
            return resp_len;
    
         case COAP_PUT:
         case COAP_POST:
             /* convert the payload to an integer and update the internal
                value */
             if (pdu->payload_len <= 5) {
                 char payload[6] = { 0 };
                 memcpy(payload, (char *)pdu->payload, pdu->payload_len);
                 _value = (uint16_t)strtoul(payload, NULL, 10);
                 return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
             }
             else {
                 return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
             }
     }
     return 0;
```

Compile with the iotlab-m3 target (default board in the application [Makefile](Makefile)) and verify your code


```python
!make
```

#### Radio settings

If you are running this training at the same time as other people on the testbed, it is a good idea to change the default radio configuration to avoid too much collision with others.

Use the following cell to give you random values for channel and PAN ID.

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
!iotlab-experiment submit -n "riot-coap" -d 60 -l 2,archi=m3:at86rf231+site=$SITE
```

3. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.


4. Check the nodes allocated to the experiment (filtered by network address)

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

From here you will choose one node with the CoAP server role and the other one with client role. You need to open a terminal for both nodes

5. For server and client node, open a Jupyter terminal (use `File > New > Terminal`) and run the command replacing `<channel> <pan_id>` by the values you obtained in the `Radio settings` section `<id> <site>`with the right values.

<!-- #raw -->
make DEFAULT_CHANNEL=<channel> DEFAULT_PAN_ID=<pan_id> IOTLAB_NODE=m3-<id>.<site>.iot-lab.info -C riot/networking/coap flash term
<!-- #endraw -->

### Request CoAP server with the client

1. On the client terminal press "Enter" to show the shell prompt. Next type help to show all commands available

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
coap                 CoAP example
reboot               Reboot the node
ps                   Prints information about running threads.
ping6                Ping via ICMPv6
random_init          initializes the PRNG
random_get           returns 32 bit of pseudo randomness
nib                  Configure neighbor information base
ifconfig             Configure network interfaces
<!-- #endraw -->

2. Verify the radio connectivity between your server and client

On the server print the network configuration with the <code>ifconfig</code> command. View the IPv6 link local address with <code>inet6 addr</code> field

<!-- #raw -->
> ifconfig
ifconfig
Iface  7  HWaddr: 2E:16  Channel: 26  Page: 0  NID: 0x23
          Long HWaddr: 15:11:6B:10:65:FA:AE:16 
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
          AUTOACK  ACK_REQ  CSMA  L2-PDU:102 MTU:1280  HL:64  6LO  
          IPHC  
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::1711:6b10:65fa:ae16  scope: local  VAL
          inet6 group: ff02::1
<!-- #endraw -->

On the client use <code>ping6</code> command to test radio connectivity with the client

<!-- #raw -->
> ping fe80::1711:6b10:65fa:ae16 
ping fe80::1711:6b10:65fa:ae16 
12 bytes from fe80::1711:6b10:65fa:ae16: icmp_seq=0 ttl=64 rssi=-47 dBm time=6.036 ms
12 bytes from fe80::1711:6b10:65fa:ae16: icmp_seq=1 ttl=64 rssi=-47 dBm time=6.037 ms
12 bytes from fe80::1711:6b10:65fa:ae16: icmp_seq=2 ttl=64 rssi=-47 dBm time=6.351 ms

--- fe80::1711:6b10:65fa:ae16 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 6.036/6.141/6.351 ms
<!-- #endraw -->

3. Discover the CoAP resources available on the server

On the server terminal verify that you have a Coap server which listenning on the default port 5683

<!-- #raw -->
> coap info
coap info
CoAP server is listening on port 5683
<!-- #endraw -->

On the client terminal use the coap cli command to request the server and use the well-known core URI. You can visualize the message ID in the request and the response success code 2.05 

<!-- #raw -->
> coap help           
coap help
usage: coap <get|post|put|info>

> coap get help
coap get help
usage: coap <get|post|put> [-c] <addr>[%iface] <port> <path> [data]
Options
    -c  Send confirmably (defaults to non-confirmable)
    
> coap get fe80::1711:6b10:65fa:ae16 5683 /.well-known/core    
coap get fe80::1711:6b10:65fa:ae16 5683 /.well-known/core
gcoap_cli: sending msg ID 61401, 23 bytes
> gcoap: response Success, code 2.05, 77 bytes
</riot/board>,</riot/cpu>,</temperature>,</value>
<!-- #endraw -->

4. Request the CoAP resources

Get the board name, cpu name, temperature sensor value and default value endpoint 

<!-- #raw -->
> coap get fe80::1711:6b10:65fa:ae16 5683 /riot/board
coap get fe80::1711:6b10:65fa:ae16 5683 /riot/board
gcoap_cli: sending msg ID 61402, 17 bytes
> gcoap: response Success, code 2.05, 9 bytes
iotlab-m3


> coap get fe80::1711:6b10:65fa:ae16 5683 /riot/cpu  
coap get fe80::1711:6b10:65fa:ae16 5683 /riot/cpu
gcoap_cli: sending msg ID 61403, 15 bytes
> gcoap: response Success, code 2.05, 7 bytes
stm32f1

> coap get fe80::1711:6b10:65fa:ae16 5683 /temperature
coap get fe80::1711:6b10:65fa:ae16 5683 /temperature
gcoap_cli: sending msg ID 61404, 18 bytes
> gcoap: response Success, code 2.05, 8 bytes
 36.59°C
 
> coap get fe80::1711:6b10:65fa:ae16 5683 /value      
coap get fe80::1711:6b10:65fa:ae16 5683 /value
gcoap_cli: sending msg ID 61405, 12 bytes
> gcoap: response Success, code 2.05, 1 bytes
0

<!-- #endraw -->

Update the value endpoint with a PUT request and verify that the value is updated

<!-- #raw -->
> coap put fe80::1711:6b10:65fa:ae16 5683 /value 8888
coap put fe80::1711:6b10:65fa:ae16 5683 /value 8888
gcoap_cli: sending msg ID 61406, 18 bytes
> gcoap: response Success, code 2.04, empty payload


> coap get fe80::1711:6b10:65fa:ae16 5683 /value
coap get fe80::1711:6b10:65fa:ae16 5683 /value
gcoap_cli: sending msg ID 61407, 12 bytes
> gcoap: response Success, code 2.05, 4 bytes
8888
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH will be closed automatically.
