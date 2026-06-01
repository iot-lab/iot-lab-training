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

## Extending a RIOT application

In this exercise, you will extend a RIOT application with new shell commands and add an interactive behavior:

- the 'board' command will return the name of the board
- the 'cpu' command will return the name of the cpu

You will have to edit the [Makefile](Makefile) and the [main.c](main.c) files of the application in this notebook.

For all shell commands, this notebook will precisely give you the implementation and give you all useful documentation links (online documentation and similar sample applications).

### Add the shell module to the build

Edit the file [Makefile](Makefile) and add there the `shell` module to the build.

```mk
USEMODULE += shell
```

### Add basic shell to the application

The first thing to do is to include the `shell.h` header at the beginning of the [main.c](main.c) file, just below the line `#include <stdio.h>`:

```c
#include "shell.h"
```

Then, in the `main` function, the shell can be started by adding the following code before `return 0`:

```c
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
```

The application can be tested on the `native` platform with the following commands (**open a terminal in File > New > Terminal menu**):

<!-- #raw -->
make -C riot/basics/shell all term
<!-- #endraw -->

By default, the shell module provides the `help` command to list available commands. Try it.

As you can see, there is no available command.

**Important:** Use Ctrl+C to quit the RIOT native instance running in the terminal.

Let's implement a couple of simple ones to print the board and cpu.

### Implement simple shell commands

RIOT provides 2 predefined macros that contain the current board and cpu the application is running on: `RIOT_BOARD` and `RIOT_CPU`. You will use them to print the board and cpu in the corresponding shell commands, e.g. `board` and `cpu` callbacks.

1. In [main.c](main.c), let's add the callback functions for the `board` and `cpu` commands, just below the line `#include "shell.h`:

```c
static int _board_handler(int argc, char **argv)
{
    /* These parameters are not used, avoid a warning during build */
    (void)argc;
    (void)argv;
    
    puts(RIOT_BOARD);
    
    return 0;
}

static int _cpu_handler(int argc, char **argv)
{
    /* These parameters are not used, avoid a warning during build */
    (void)argc;
    (void)argv;
    
    puts(RIOT_CPU);
    
    return 0;
}
```

2. Now let's define the command name, the help message and the associated callback function in the list of available shell commands. This is simply done by adding the following code between the function callbacks and the main function:

```c
static const shell_command_t shell_commands[] = {
    { "board", "Print the board name", _board_handler },
    { "cpu", "Print the cpu name", _cpu_handler },
    { NULL, NULL, NULL }
};
```

3. In the main function, modify the `shell_run` call to integrate the new extended `shell_commands` list:

```c
shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
```

instead of:
```c
shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
```

4. In the terminal, rebuild and run the application for the native board:

<!-- #raw -->
make -C riot/basics/shell all term
<!-- #endraw -->

### Submit an experiment on the IoT-LAB

The application is now ready and can be tested on real hardware, on an iotlab-m3 in the IoT-LAB testbed.

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-shell" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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

5. Build and flash the shell application for the iotlab-m3 target device:

```python
!make BOARD=iotlab-m3 IOTLAB_NODE=auto flash
```

6. Open a Jupyter terminal (use `File > New > Terminal`) and open the serial link.

<!-- #raw -->
make BOARD=iotlab-m3 IOTLAB_NODE=auto -C riot/basics/shell term
<!-- #endraw -->

Interact with the shell to get the command output:

<!-- #raw -->
> board

> cpu

<!-- #endraw -->

### Free up the resources

Since you finished the training, stop your experiment to free up the experiment nodes:

```python
!iotlab-experiment stop
```

The serial link connection through SSH will be closed automatically.
