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

## Discover CoAP

In this excercice you will run a CoAP application on a IoT-LAB M3 node and send requests to a CoAP server, from the frontend server, through a private IPv6 network. Moreover you will learn how to add a CoAP resource to get the accelerometer sensor (i.e. LSM303DLHC hardware) values.


### Compile the Contiki-NG firmwares

#### IoT-LAB platform support

Since IoT-LAB boards support is not included in the Contiki-NG official repository, set a `ARCH_PATH` environment variable that points to the additionnal platform support:

```python
%env ARCH_PATH = /home/user/iot-lab-training/contiki-ng/iot-lab-contiki-ng/arch
```

#### Radio settings

If you are running this training as the same time as other people, it is a good idea to change the default radio configuration to avoid too much collision with others.

Use the following cell to give you random values for channel and PAN ID.

```python
import os,binascii,random
pan_id = binascii.b2a_hex(os.urandom(2)).decode()
channel = random.randint(11, 26)
print('Use CHANNEL={}, PAN_ID={}'.format(channel, pan_id))
```

Change default values below before running the cell.

```python
%env RADIO = IEEE802154_CONF_PANID=0xBEEF,IEEE802154_CONF_DEFAULT_CHANNEL=11
```

#### CoAP Server

We modified a bit the `coap-example-server`. Compile the local example.

```python
!make -C coap-example-server TARGET=iotlab BOARD=m3 DEFINES=$RADIO
```

#### Border Router

Use the `rpl-border-router` example, to be used by one node as border router, and enabe communication with all nodes from the frontend server.

```python
%env APP_DIR = ../../iot-lab-contiki-ng/contiki-ng/examples/rpl-border-router
!make -C $APP_DIR TARGET=iotlab BOARD=m3 DEFINES=$RADIO
```

### Launch an experiment

1. Choose your site (grenoble|lille|saclay|strasbourg|...):

```python
%env SITE = lille
```

2. Submit an experiment with two M3 nodes:

```python
!iotlab-experiment submit -d 120 -l 2,archi=m3:at86rf231+site=$SITE
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

And assign IDs to environment variables:

```python
%env ID1 = <id1>
%env ID2 = <id2>
```

5. Flash the first node as border router:

```python
!iotlab-node --update $APP_DIR/border-router.iotlab -l $SITE,m3,$ID1
```

6. Flash the second node as CoAP server:

```python
!iotlab-node --update coap-example-server/coap-example-server.iotlab -l $SITE,m3,$ID2
```

### Set your private IPv6 network

Open a Jupyter terminal (use `File > New > Terminal`) and connect to the SSH frontend server. Replace `<site>` by the good value.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

From the frontend SSH launch the following command to create the tunslip interface like in the IPv6 training:

<!-- #raw -->
<login>@<site>:~$ sudo tunslip6.py -v2 -L -a m3-<id1> -p 20000 fd00::1/64
<!-- #endraw -->

You will see in the output the IP address of the border router (_fd00::xxxx_).

Open a Jupyter terminal (use `File > New > Terminal`) and from the frontend server again, get the routes page of the border router's web server to know the IP address of the CoAP server (_Routing links_):

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

<!-- #raw -->
<login>@<site>:~$ lynx --dump http://[fd00::xxxx]
<!-- #endraw -->

### CoAP requests

You now have a connection between the frontend server and your node running the CoAP server firmware. Let's use the [aiocoap](https://aiocoap.readthedocs.io/en/latest/index.html) client from there to call server URIs.


#### Discover resources

First you need to know which ressources are avalaible. For that purpose the standard `/.well-known/core` URI is present.

<!-- #raw -->
aiocoap-client coap://[fd00::yyyy]/.well-known/core
<!-- #endraw -->

You should see a list with the following resources:
- /test/hello
- /debug/mirror
- /test/chunks
- /test/separate
- /test/push
- /test/sub
- /test/b1sepb2
- /actuators/toggle
- /sensors/light


#### GET Request

The more standard use is to call a sensor ressource with a GET method (used by default by the client).

<!-- #raw -->
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/sensors/light
<!-- #endraw -->

Another ressource is available to test the GET method. For this one you can specify a paramater thanks to a query string. Test different values, and have a look to the `coap-example-server/resources/res-hello.c` to understand what's going on.

<!-- #raw -->
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/test/hello
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/test/hello?len=5
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/test/hello?len=24
<!-- #endraw -->

#### Observe

An interesting feature of CoAP is resource observation. You do not need to send a GET request periodically to have an up-to-date value, but send only once a GET request with the 'observe' option. Thus you are registered as an 'observer' for this resource, and the CoAP server will notify you periodically or at each time the value changes significantly. In this example, the server send a new value at a period of 5 seconds.

<!-- #raw -->
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/test/push --observe
<!-- #endraw -->

### Add CoAP resource

In this excercice you will add a CoAP resource to get the accelerometer sensor values.

1. Edit the file `coap-example-server/coap-example-server.c` and  declare the accelerometer resource

```c
#if PLATFORM_HAS_ACCELEROMETER
#include "dev/acc-mag-sensor.h"
extern coap_resource_t res_accel;
#endif
```

2. Activate the resource

```c
#if PLATFORM_HAS_ACCELEROMETER
  coap_activate_resource(&res_accel, "sensors/accel");
  SENSORS_ACTIVATE(acc_sensor);
#endif
```

3. Edit the file `coap-example-server/resources/res-accel.c` and implement the resource

```c
#include "contiki.h"

#if PLATFORM_HAS_ACCELEROMETER

#include <stdio.h>
#include <string.h>
#include "coap-engine.h"
#include "dev/acc-mag-sensor.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* A simple getter example. Returns the reading from accelerometer sensor */
RESOURCE(res_accel,
         "title=\"Three axis accelerometer (supports JSON)\";rt=\"AccelerometerSensor\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int x = acc_sensor.value(ACC_MAG_SENSOR_X);
  int y = acc_sensor.value(ACC_MAG_SENSOR_Y);
  int z = acc_sensor.value(ACC_MAG_SENSOR_Z);
  unsigned int accept = -1;
  coap_get_header_accept(request, &accept);
  if(accept == -1 || accept == TEXT_PLAIN) {
    coap_set_header_content_format(response, TEXT_PLAIN);
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "%d;%d;%d", x, y, z);

    coap_set_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  } else if(accept == APPLICATION_XML) {
    coap_set_header_content_format(response, APPLICATION_XML);
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "<accelerometer x=\"%d\" y=\"%d\" z=\"%d\"/>", x, y, z);

    coap_set_payload(response, buffer, strlen((char *)buffer));
  } else if(accept == APPLICATION_JSON) {
    coap_set_header_content_format(response, APPLICATION_JSON);
    snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{'accelerometer':{'x':%d,'y':%d,'z':%d}}", x, y, z);

    coap_set_payload(response, buffer, strlen((char *)buffer));
  } else {
    coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
    const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
    coap_set_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_ACCELEROMETER */
```

4. Compile and verify your code

```python
!make -C coap-example-server TARGET=iotlab BOARD=m3 DEFINES=$RADIO
```

5. Flash the second node with the new CoAP firmware

```python
!iotlab-node --update coap-example-server/coap-example-server.iotlab -l $SITE,m3,$ID2
```

6. From the frontend SSH check that the CoAP resource is exposed by the server and request it.

<!-- #raw -->
<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/.well-known/core
...
</sensors/light>;title="Ambient light (supports JSON)";rt="LightSensor",</sensors/accel>;title="Three axis accelerometer (supports JSON)";rt="AccelerometerSensor"

<login>@<site>:~$ aiocoap-client coap://[fd00::yyyy]/sensors/accel
-482;-26;-909
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH and the tunslip process will be closed automatically.
