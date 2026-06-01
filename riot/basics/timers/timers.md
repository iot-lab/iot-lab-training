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

## Timers

In this exercise, you will use timers to add delays between messages displayed in the terminal.
The messages will be continuously printed from the main thread and from a second thread.

In RIOT, the `ztimer` module allows for multiplexing several timer contexts on top of a single hardware timer. With the `ztimer` module, it is then possible to manage several virtual timers from a single timer. This single can have 3 levels of precision: seconds (module `ztimer_sec`), millieconds (module `ztimer_msec`) and microseconds (module `ztimer_usec`).

### Print messages from the main thread

Let's start by printing a message from the main thread every 500ms.

1. In the application [Makefile](Makefile), add the necessary ztimer module with millisecond precision:

```make
USEMODULE += ztimer_msec
```

2. In the [main.c](main.c) file, include the `ztimer.h` necessary header, below the `thread.h` include:

```c
#include "ztimer.h"
```

3. In the main function, just before `return 0`, add an endless loop that prints a message every 500ms, as follows:

```c
    int counter = 0;
    while (1) {
        counter++;
        printf("Message %d from main thread\n", counter);
        
        /* add 500ms delay */
        ztimer_sleep(ZTIMER_MSEC, 500);
    }
```

4. Open a Jupyter terminal (use `File > New > Terminal`) and test your application on native:

<!-- #raw -->
make -C riot/basics/timers all term
<!-- #endraw -->

### Print messages from a separate thread

Create a thread that will print the message `Message <i> from thread <thread name>` every second.

1. Create the thread in the `main` function:

```c
thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
               0, thread_handler, "my thread", "thread");
```

Note that we set an argument containing a string. We will use this string in the message displayed by the thread.

2. Implement the thread handler function as follows:

```c
static char stack[THREAD_STACKSIZE_MAIN];
static void *thread_handler(void *arg)
{
    int counter = 0;
    
    while (1) {
        counter++;
        printf("Message %d from thread '%s'\n", counter, (char *)arg);
        /* Wait one second */
        ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    }
    
    return NULL;
}
```

3. In the terminal, you can now test your application on native again:

<!-- #raw -->
make -C riot/basics/timers all term
<!-- #endraw -->
