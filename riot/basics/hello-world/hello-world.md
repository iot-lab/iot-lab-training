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

## First RIOT application

In this exercise, you will build your first RIOT application and run it.

- In a first step, the application will be built for the `native` target and run as a regular application.

- In a second step, you will submit a new experiment on IoT-LAB with one iotlab-m3 node, then build and flash this application for this target.


### Build for native

The [Makefile](Makefile) of the RIOT application is the simplest possible:

```mk
# Name of the application
APPLICATION = hello-world

# Default target board
BOARD ?= native

USEMODULE += ztimer_sec

# Path to the RIOT base directory
RIOTBASE ?= $(CURDIR)/../../RIOT

# Include all build system logic for building the application
include $(RIOTBASE)/Makefile.include
```

The [main.c](main.c) file is also very simple and contains standard C code. We wait five seconds and write the message "Hello from RIOT!" on the serial port
```c
#include <stdio.h>
#include "ztimer.h"

int main(void)
{
    ztimer_sleep(ZTIMER_SEC, 5);
    puts("Hello from RIOT!");
    return 0;
}
```

Build the application and run it on native is then as simple as running the following command:


```python
!make all term
```

**Important:** once done interrupt the Jupyter kernel by clicking the ◼ button in the menu bar above.


### Build for an iotlab-m3

Build the firmware for the `iotlab-m3` target, using the `BOARD` variable:

```python
!make BOARD=iotlab-m3
```

### Submit an experiment on IoT-LAB


1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "hello-world" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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

### Open a terminal and flash the firmware on the iotlab-m3

1. Open a Jupyter terminal (use `File > New > Terminal`) and run the following command from it.

<!-- #raw -->
make BOARD=iotlab-m3 IOTLAB_NODE=auto -C riot/basics/hello-world term
<!-- #endraw -->

2. Using the `IOTLAB_NODE`, you can also easily flash the device.

```python
!make BOARD=iotlab-m3 IOTLAB_NODE=auto flash
```

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH will be closed automatically.
