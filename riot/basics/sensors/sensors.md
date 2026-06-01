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

## Use sensors on the IoT-LAB M3 board

The IoT-LAB M3 board provides several sensors:
- lps331ap: a temperature and atmospheric pressure sensor
- l3g4200d: a gyroscope
- lsm303dlhc: an accelerometer and magnetometer
- isl29020: a light sensor

In this exercise, we propose to read and display the values of these sensors using shell commands and synchronization between threads.

- The lps331ap temperature and pressure values will read on demand via a single shell command, `lps`
- The lsm303dlhc accelerometer and magnetometer values will be read continuously every 500ms after calling `lsm start`. The continuous read will be stopped by calling `lsm stop`
- The isl29020 light value will also be read on demand, using `isl`
- Similar to the lsm303dlhc, the l3g4200d gyroscope will be read continuously every 500ms after calling `l3g start`. The continuous read will be stopped by calling `l3g stop`

### Submit an experiment on IoT-LAB

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=saclay
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-sensors" -d 120 -l 1,archi=m3:at86rf231+site=$SITE
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

### Read the lps331ap sensor using a shell command

To read the lps331ap sensor, we will use the lps331ap module driver.

1. In the application [Makefile](Makefile), add the lps331ap module to the build:

```Makefile
USEMODULE += lps331ap
```

2. In the [main.c](main.c), add the necessary includes for this driver:

```c
#include "lpsxxx.h"
#include "lpsxxx_params.h"
```

**Note:** The files, device descriptor type and function names start with `lpsxxx` because this driver is generic and also works with the lps25hb and lps22hb variants.

3. Declare the device descriptor variable:

```c
static lpsxxx_t lpsxxx;
```

4. In the main function, initialize the driver:

```c
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);
```

5. Implement the `lpsxxx_handler` function. This function will be called when the shell command uses `lps` as first argument:

```c
    if (!strcmp(argv[1], "temperature")) {
        int16_t temp = 0;
        lpsxxx_read_temp(&lpsxxx, &temp);
        printf("Temperature: %i.%u°C\n", (temp / 100), (temp % 100));
    }
    else if (!strcmp(argv[1], "pressure")) {
        uint16_t pres = 0;
        lpsxxx_read_pres(&lpsxxx, &pres);
        printf("Pressure: %uhPa\n", pres);
    }
    else {
        _lpsxxx_usage(argv[0]);
        return -1;
    }
```

As you can see, each `temperature` and `pressure` are handled separately using subcommands.

6. Add the `lps` shell command in the list of available commands (in the `shell_command_t` list before the main function):

```c
    { "lps", "read the lps331ap values", lpsxxx_handler },
```

7. Build and flash the application:

```python
!make IOTLAB_NODE=auto flash
```

8. Open a Jupyter terminal (use `File > New > Terminal`) and connect to the serial port of the IoT-LAB M3 and observe the value displayed every 2 seconds:

<!-- #raw -->
make IOTLAB_NODE=auto -C riot/basics/sensors term
<!-- #endraw -->

9. In the RIOT shell, verify that the `lps` command is working as expected:

<!-- #raw -->
> lps
lps
usage: lps <temparature|pressure>
> lps temperature
lps temperature
Temperature: 37.02°C
> lps pressure
lps pressure
Pressure: 998hPa
<!-- #endraw -->

<!-- #region -->
You can keep this terminal open while performing the next steps of this notebook.

### Read the lsm303dlhc sensor


To read the lsm303dlhc sensor in a loop and to start/stop this continous read from a shell command, we will use 2 things:
- the continuous read will be done in a separate thread `lsm303dlhc_thread`,
- the start/stop feature will use one of the synchronization mechanisms available in RIOT: a mutex. The idea is make the lsm303dlhc thread try to aquire the mutex before reading the values. If the mutex is already aquired by the main thread, the lsm303dlhc thread will be blocked, trying to aquire it. So the start subcommand of the `lsm` shell will just release the mutex to let the lsm303dlhc thread read the values. After each read, the lsm303dlhc thread will release the mutex. The stop subcommand of the `lsm` shell will just aquire the mutex again to clock the lsm303dlhc thread.
To read the lsm303dlhc sensor, use the lsm303dlhc module driver. In this part of the exercise, the driver is read from a separate background thread.

1. In the application [Makefile](Makefile), add the lsm303dlhc module to the build:

```Makefile
USEMODULE += lsm303dlhc
```

2. In the [main.c](main.c), add the necessary includes for this driver:

```c
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"
```

3. Declare the device descriptor variable:

```c
static lsm303dlhc_t lsm303dlhc;
```

4. Declate the mutex used to lock/unlock the lsm303dlhc thread. The mutex is initialized locked in order to block the lsm303dlhc thread at startup.

```c
static mutex_t lsm_lock = MUTEX_INIT_LOCKED;
```


5. In the main function, initialize the driver:

```c
    lsm303dlhc_init(&lsm303dlhc, &lsm303dlhc_params[0]);
```

6. In the lsm303dlhc thread function, acquire the mutex. This call blocks the thread until the mutew is released by another thread:

```c
        mutex_lock(&lsm_lock);
```

7. In the lsm303dlhc thread function, after the mutex is aquired, the accelerometer/magnetometer values can be read:

```c
        lsm303dlhc_3d_data_t mag_value;
        lsm303dlhc_3d_data_t acc_value;
        lsm303dlhc_read_acc(&lsm303dlhc, &acc_value);
        printf("Accelerometer x: %i y: %i z: %i\n",
               acc_value.x_axis, acc_value.y_axis, acc_value.z_axis);
        lsm303dlhc_read_mag(&lsm303dlhc, &mag_value);
        printf("Magnetometer x: %i y: %i z: %i\n",
               mag_value.x_axis, mag_value.y_axis, mag_value.z_axis);
```

8. Once the values are read, the mutex must be released:

```c
        mutex_unlock(&lsm_lock);
```

9. Finally we can implement the `start`/`stop` subcommands of the `lsm` shell command `lsm303dlhc_handler` handler:

```c
    if (!strcmp(argv[1], "start")) {
        mutex_unlock(&lsm_lock);
    }
    else if (!strcmp(argv[1], "stop")) {
        mutex_trylock(&lsm_lock);
    }
    else {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }
```
   The `stop` subcommand calls the `mutex_trylock` instead of `mutex_lock` to avoid having the shell blocked when `lsm stop` is called several times in row.

10. Let's build and flash again the application:
<!-- #endregion -->

```python
!make IOTLAB_NODE=auto flash
```

11. If you kept the previous serial output opened in the terminal, you can test the new `lsm` with the `start` and `stop` subcommands

<!-- #raw -->
> lsm
lsm
usage: lsm <start|stop>
> lsm start
> lsm start
lsm start
Accelerometer x: 676 y: 160 z: -228
Magnetometer x: -292 y: -73 z: -80
> Accelerometer x: 680 y: 156 z: -236
Magnetometer x: -292 y: -75 z: -80
Accelerometer x: 676 y: 156 z: -240
Magnetometer x: -291 y: -73 z: -81
Accelerometer x: 672 y: 160 z: -232
Magnetometer x: -290 y: -73 z: -78
Accelerometer x: 664 y: 152 z: -236
Magnetometer x: -292 y: -74 z: -80
Accelerometer x: 676 y: 160 z: -236
Magnetometer x: -291 y: -74 z: -77
Accelerometer x: 676 y: 156 z: -236
Magnetometer x: -291 y: -73 z: -81
Accelerometer x: 680 y: 152 z: -236
Magnetometer x: -290 y: -74 z: -81
lsm stopAccelerometer x: 680 y: 160 z: -232
Magnetometer x: -290 y: -73 z: -78

lsm stop
> 
<!-- #endraw -->

### Read other sensors

As said at the beginning of this Notebook, the IoT-LAB M3 boards provides 2 other sensors:
- l3g4200d: a gyroscope
- isl29020: a light sensor

In this section, we propose that you extend the application with 2 new shell commands, `isl` and `l3g`. The synopsis of these commands is the following:
- `isl` should directly read the isl29020 light sensor and print the value:

<!-- #raw -->
> isl
isl
Light: 10lx
<!-- #endraw -->

- Similar to the lsm303dlhc, `l3g` should have the `start` and `stop` subcommands to start/stop reading the gyroscope values in a loop. The values will be read in a new thread (`l3g4200d_thread`) and the synchronization between the main thread and the l3g4200d thread will use a new mutex `l3g_lock`.

**Note:** In Grenoble, all sensors are located under an access floor, so completely in the dark. On this IoT-LAB site, the light sensors won't return usable values (always 0).

To help you with the APIs usage of the corresponding drivers, you can have a look at the following resources:
- [isl29020 API online documentation](http://doc.riot-os.org/group__drivers__isl29020.html)
- The [isl2920 test application code](https://github.com/RIOT-OS/RIOT/tree/master/tests/driver_isl29020) can also be useful
- [l3g4200d API online documentation](http://doc.riot-os.org/group__drivers__l3g4200d.html)
- The [l3g4200d test application code](https://github.com/RIOT-OS/RIOT/tree/master/tests/driver_l3g4200d) can also be useful

Also, don't forget to add the corresponding modules in the [Makefile](Makefile).

Once done, you can rebuild and flash your application.

```python
!make IOTLAB_NODE=auto flash
```

<!-- #region -->
**Note:** If your experiment is already stopped, you can submit another one as explained in the "Submit an experiment on IoT-LAB" section.


**Optional improvement:** the continuous read is always done every 500ms. It is possible to extend the `lsm start` and `l3g start` commands with an extra parameter to specify the delay between each read:
<!-- #endregion -->

<!-- #raw -->
> lsm
lsm
usage: lsm <start|stop> [delay ms]
> l3g
l3g
usage: l3g <start|stop> [delay ms]
<!-- #endraw -->

If unspecified (`argc < 3`) the delay value is set by default to 500ms. If specified (`argc == 3`), the argument (`argv[2]`) can be converted to an int using the `atoi` function. The delay value can be stored in a global static variable (`uint32_t`).

The following command should read the lsm303dlhc values every 2s:

<!-- #raw -->
> lsm start 2000
Accelerometer x: 672 y: 156 z: -232
Magnetometer x: -292 y: -73 z: -81
> Accelerometer x: 672 y: 156 z: -236
Magnetometer x: -292 y: -74 z: -80
Accelerometer x: 676 y: 160 z: -240
Magnetometer x: -292 y: -73 z: -78
lsm stop
<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH will be closed automatically.
