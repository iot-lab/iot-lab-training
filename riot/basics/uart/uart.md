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

<!-- #region -->
## UART

In this exercise, we propose to use the [UART API of RIOT](http://doc.riot-os.org/group__drivers__periph__uart.html) in conjunction with the main thread to print all characters received on stdio.

The application that you will write in this notebook will initialize the STDIO UART with a custom callback function. This callback function is triggered on each UART interrupt, e.g. for each character received, and sends the character via IPC to the main thread. On each IPC message, the main thread prints the content of the message.


### Implement the application

#### I. Adapt the Makefile

1. Open the [Makefile](Makefile)
2. Add the dependency to the `periph_uart` feature:

```make
FEATURES_REQUIRED += periph_uart
```

#### II. Adapt main.c

Open the [main.c](main.c) file in a new tab.

1. Add the missing includes for UART peripheral:

```c
#include "periph/uart.h"
```
2. Declare the message queue. A message queue is needed because IPC messages will be sent from interrupt context.

```c
#define MSG_QUEUE_SIZE    (8)
static msg_t msg_queue[MSG_QUEUE_SIZE];
```
3. Implement the uart callback function: on each character received, this function will send it to the main thread. Note the context arg that contains the pid of the thread.
```c
static void uart_cb(void *arg, uint8_t data)
{
       kernel_pid_t *main_pid = (kernel_pid_t *)arg;
       msg_t msg;
       msg.content.value = data;

       msg_send(&msg, *main_pid);
}
```
4. Initialize the STDIO UART: in addition to the callback function implemented above, the main thread pid is given as context parameter.
```c
       kernel_pid_t main_pid = thread_getpid();
       uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, uart_cb, &main_pid);
```
By default, the `STDIO_UART_DEV` and `STDIO_UART_BAUDRATE` constants correspond to respectively `UART_DEV(0)` and `115200` but can be adjusted a board level if special configuration is needed depending on the board setup.
5. Add the reception loop at the end of the main function: it simply is an endless loop that waits for incoming the IPC messages sent from the UART callback function. Each character received is writen to stdout. Also note the message queue initialization: it's important because messages are sent from interupt context and this is not blocking.
```c
       msg_init_queue(msg_queue, MSG_QUEUE_SIZE);
       while (1) {
           msg_t msg;
           msg_receive(&msg);
           printf("received: %c\n", (char)msg.content.value);
       }
```

### Build for an iotlab-m3

Build the firmware for the `iotlab-m3` target, using the `BOARD` variable (default board in the [Makefile](Makefile)):
<!-- #endregion -->

```python
!make
```

### Submit an experiment on IoT-LAB

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=saclay
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "uart" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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

5. Flash the firmware on the iotlab-m3:

```python
!make IOTLAB_NODE=auto flash-only
```

### Open a terminal and test the application

Open a Jupyter terminal (use `File > New > Terminal`) and type characters + Enter in the terminal.

<!-- #raw -->
make IOTLAB_NODE=auto -C riot/basics/uart term
<!-- #endraw -->

The application should echo your inputs!

<!-- #raw -->
lmdfmdlkf
received: l
received: m
received: d
received: f
received: m
received: d
received: l
received: k
received: f
received: 
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

### Going further

As you can see, the application has some limitations:
- characters are printed one by one
- the message queue size introduce a limitation when too many characters are sent before pressing enter

You could try to improve the application by adding each character received in an intermediate static buffer of let's say 64B size:
```
static uint8_t buffer[64];
```

and send an IPC message only when the received character is `\n`.
Some things to also think about:
- you will have to handle the current position in the buffer
- be careful with the closing character of a string (`\0`)
- when printing the buffer, it must be casted to `char *`.
