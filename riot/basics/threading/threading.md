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

## Managing threads with RIOT

In this exercise, you will discover the basic threading features of RIOT:
- how to monitor the threads available in an application
- how to manage the priority between threads

### Monitor running threads

To monitor running threads, the application needs 1) to start a shell and 2) to extend the shell with commands for printing threads activity. In RIOT, this can be done by simply extending the provided template application as follows:

1. Load extra modules

In the [Makefile](Makefile) file, add the following:

```mk
USEMODULE += shell
USEMODULE += ps
```

The `shell_commands` module will automatically build shell commands provided by other modules in the application. The `ps` module provides a shell command to monitor the threads activity.

2. Run the shell

In [main.c](main.c) file, in the `main` function, just run the shell as follows (under the comment `/* Start the shell here */`:

```c
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
```

3. Test the application

Open a Jupyter terminal (use `File > New > Terminal`), build and run the application for native:

<!-- #raw -->
make -C riot/basics/threading all term
<!-- #endraw -->

<!-- #region -->
In the RIOT shell, list the available commands with the `help` command:
```sh
> help
```

Now display the list of running threads with the `ps` command:
```sh
> ps
```

**Important:** Use Ctrl+C to quit the RIOT native instance running in the terminal.

### Run a new thread

A new thread can be started from the `main` function using the `thread_create` function. The stack memory used by the thread must also be declared globally. Finally, the code running in the thread must be implemented in a thread handler function.

#### **Implement the new thread**

1. In [main.c](main.c), add the necessary `thread.h` include and declare the stack memory used by the new thread (below the "shell.h" include):

```c
#include "thread.h"
static char stack[THREAD_STACKSIZE_MAIN];
```

2. Add the implementation of the thread handler function (before the `main` function):

```c
static void *thread_handler(void *arg)
{
    (void)arg;
    puts("Hello from thread!");
    return NULL;
}
```

3. Finally, we can create and start the new thread, in the `main` function and before running the shell:
```c
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                  0, thread_handler, NULL, "new thread");
```
The thread must be created before calling the `shell_run` function because this function never returns, so any code after won't be executed.

#### **Run the application**

In the terminal, build and run the application for native:
<!-- #endregion -->

<!-- #raw -->
make -C riot/basics/threading all term
<!-- #endraw -->

<!-- #region -->
The message "Hello from thread!" is displayed before the shell starts. If you call the `ps` command, the new thread is not listed because it already exited.


### Start a background thread

If we want to keep the thread running in background and have the possibility to list it in the `ps` command, 2 things must be done:
- prevent the thread from completing: this is done using an endless loop in the thread handler function.
  ```c
  while (1) {}
  ```
- ensure the thread has a lower priority than the main thread. In the previous example, `THREAD_PRIORITY_MAIN - 1` was set, so the thread had a higher priority than the main thread. With an endless busy loop, that would prevent the shell from starting as it would never be scheduled.

1. Modify the thread handler to run a busy loop

Change the thread handler implementation to be the following:
```c
static void *thread_handler(void *arg)
{
    (void)arg;
    while (1) {}
    return NULL;
}
```

2. Lower the priority of the background thread

Change the thread priority to `THREAD_PRIORITY_MAIN + 1` when creating it:
```c
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread_handler, NULL, "new thread");
```

The new priority is `THREAD_PRIORITY_MAIN + 1` so the created thread has a lesser priority than the main thread.

3. Run the application

In the terminal, build and run the application for native:
<!-- #endregion -->

<!-- #raw -->
make -C riot/basics/threading all term
<!-- #endraw -->

Now the thread called `new thread` is listed in the output of the `ps` command but as you can see it remains pending: in fact it is never started because the main thread has a higher priority.
