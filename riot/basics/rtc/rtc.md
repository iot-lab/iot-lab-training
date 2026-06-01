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
## Real Time Clock (RTC)

In this exercise, we propose to use the [RTC API of RIOT](http://doc.riot-os.org/group__drivers__periph__rtc.html) in conjunction with the main thread to print the current RTC time every 5s.

The application that you will write in this notebook will:
1. print the current time and set an RTC alarm 5 seconds later in the main thread that will then wait for an IPC message
2. the callback function triggered by the RTC alarm will send an IPC message to the main thread
3. on reception of the IPC message, the main thread will print the current time and configure the next alarm, and so on.


### Implement the application

#### I. Adapt the Makefile

1. Open the [Makefile](Makefile)
2. Add the dependency to the `periph_rtc` feature:

```make
FEATURES_REQUIRED += periph_rtc
```

#### II. Adapt main.c

Open the [main.c](main.c) file in a new tab.

1. Add the missing includes for RTC peripheral and the time manipulation functions:

```c
#include "periph/rtc.h"
```
2. Implement the RTC alarm callback function: this function will simply send an empty IPC message to the main thread. Note the context arg that contains the pid of the main thread.
```c
static void rtc_alarm_cb(void *arg)
{
       kernel_pid_t *main_pid = (kernel_pid_t *)arg;
       msg_t msg;

       msg_send(&msg, *main_pid);
}
```
3. At the beginning of the main function, read the current RTC time and print it:
```c
       struct tm time;
       rtc_get_time(&time);
       printf("Startup time: %d:%d:%d\n",
              time.tm_hour, time.tm_min, time.tm_sec);
```
4. In the do..while loop, configure the next alarm in 5 seconds. Note the context argument that contains the pid of the current thread. The `mktime` function is used to make sure the time format is correct (e.g 58s + 5s -> 1m3s).
```c
           time.tm_sec += 5;
           mktime(&time);
           kernel_pid_t main_pid = thread_getpid();
           rtc_set_alarm(&time, rtc_alarm_cb, &main_pid);
```
5. Next, wait for incoming messages:
```c
           msg_t msg;
           msg_receive(&msg);
           printf("Alarm received: ");
```
6. Finally, after a message is received, read the current time and print it:
```c
           rtc_get_time(&time);
           printf("%d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
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
!iotlab-experiment submit -n "rtc" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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
make IOTLAB_NODE=auto -C riot/basics/rtc term
<!-- #endraw -->

You should see the following output:

<!-- #raw -->
Alarm received: 0:0:10
Alarm received: 0:0:15
Alarm received: 0:0:20
Alarm received: 0:0:25
Alarm received: 0:0:30
<!-- #endraw -->

By default, the RTC is initialized at 00:00:00 but it can be configured using the [rtc_set_time](http://doc.riot-os.org/group__drivers__periph__rtc.html#gaa2d60ad372a3712b875a15e07c517843) function.

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```
