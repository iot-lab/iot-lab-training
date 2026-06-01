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

## IPC messages between threads

> We recommend that you follow the [threading notebook](../threading/threading.md) before following this one.

In this exercise, you will discover the IPC mechanism available in RIOT to exchange messages between threads.

You will write a shell command that sends a string to a thread using an IPC message. The receiver thread prints each received message. This exercise provides a starting RIOT application with the shell handler already implemented ((see [Makefile](Makefile) and [main.c](main.c)).

### Implement the new thread

Let's start by editing the [main.c](main.c) file and implement the listener thread.

1. Add the includes for thread and ipc message APIs:

```c
#include "thread.h"
#include "msg.h"
```

2. Declare 2 static variables: the pid of the listener thread and the memory stack that will be used by the thread:

```c
static kernel_pid_t pid;
static char stack[THREAD_STACKSIZE_MAIN];
```

3. We can now add the listener thread function:

```c
static void *listener_thread(void *arg)
{
    (void)arg;
    
    while (1) {
        msg_t msg;
        msg_receive(&msg); /* Blocks until a message is received */
        printf("Message received: %s\n", (char *)msg.content.ptr);
    }

    return NULL;
}
```

As you can see, the thread listens endlessly for incoming IPC messages. The call to `msg_receive` is blocking until a message is received and the content of the message is a pointer to the string.

4. Finally, we can create the thread in the `main` function:

```c
    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN - 1,
                        0,
                        listener_thread,
                        NULL, "listener thread");
```

The `thread_create` function returns the `pid` of the listener thread, we will use this variable later in the send_command function. The new thread has a higher priority than the main thread, so this ensures it's started immediately.

5. Let's verify that the code is building fine and that the thread is correctly created: open a new terminal with menu `File > New > Terminal` and launch there the following command:

<!-- #raw -->
make -C riot/basics/threading-ipc all term
<!-- #endraw -->

Note that we are building for native by default.

6. In the RIOT shell, you can list the available commands using `help`:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
send                 send a message via ipc
reboot               Reboot the node
ps                   Prints information about running threads.
<!-- #endraw -->

As you can see, the `send` shell command is present but it does nothing for the moment.

Now, let's check the state of the listener thread using the `ps` command:

<!-- #raw -->
> ps
ps
        pid | name                 | state    Q | pri | stack  ( used) | base addr  | current     
          - | isr_stack            | -        - |   - |   8192 (   -1) | 0x565bc380 | 0x565bc380
          1 | idle                 | pending  Q |  15 |   8192 (  420) | 0x565ba0a0 | 0x565bbf10 
          2 | main                 | running  Q |   7 |  12288 ( 3136) | 0x565b70a0 | 0x565b9f10 
          3 | listener thread      | bl rx    _ |   6 |  12288 (12284) | 0x565b40a0 | 0x565b6f10 
            | SUM                  |            |     |  40960 (15840)
<!-- #endraw -->

Besides the default `idle` and `main` threads, the listener thread has the `pid` number 3 and is in `bl rx` state, which means that it's blocked, waiting for incoming IPC messages. This is what we are expecting.

### Implement the shell send command

Implementing the `send` command is just a matter of calling `msg_send` with a message containing the string parameter. This is simply done by adding in the `send_command` function, the following snippet:

```c
    msg_t msg;
    msg.content.ptr = (void *)argv[1];
    msg_send(&msg, pid);
```

### Test the application

In your Jupyterlab terminal, rebuild and start the RIOT native instance:

<!-- #raw -->
make -C riot/basics/threading-ipc all term
<!-- #endraw -->

If the build is successful, you can now send messages from the shell to the listener thread:

<!-- #raw -->
> send "My message"
send "My message"
Message received: My message
<!-- #endraw -->
