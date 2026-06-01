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

## Thread concurrency

In this exercise, you will learn a way to synchronize threads by using [RIOT mutexes](http://doc.riot-os.org/group__core__sync__mutex.html).

In the proposed application, 2 threads will write and read _concurrently_ in a globally shared buffer. The mutex used will ensure that the reading thread can only read once the writing thread has completed his writing job.

The two threads will behave as follow:
 - the main thread reads the content of a shared buffer and prints its content every 200ms
 - a second thread writes some content in this shared buffer every 100ms. But each write takes 200ms to complete. So if concurrency was not managed correctly, the reading thread could print corrupted content.

### Implement the writer thread

Let's start by implementing a background writer thread that takes a lot of time to write in the buffer.

1. In the [main.c](main.c) file, add the mutex header include:

```c
#include "mutex.h"
```

2. We must now declare and instanciate a global shared buffer with an associated mutex, wrapped in a struct:

```c
typedef struct {
    char buffer[128];
    mutex_t lock;
} data_t;
static data_t data;
```

3. Since a new thread is created, a dedicated memory stack must also be allocated:

```c
static char writer_stack[THREAD_STACKSIZE_MAIN];
```

4. The writer thread endless loop is divided in 3 steps:

  - _step 1_: try to acquire the mutex:

  ```c
           mutex_lock(&data.lock);
  ```

  - _step 2_: write some content in the buffer, slowly. This way concurrency access issues with the buffer are made more obvious: without synchronization, the reader thread could read the content in the middle of the write and get invalid data.

  ```c
           size_t p = sprintf(data.buffer, "start: %"PRIu32"", ztimer_now(ZTIMER_MSEC));
           ztimer_sleep(ZTIMER_MSEC, 200);
           p += sprintf(&data.buffer[p], " - end: %"PRIu32"", ztimer_now(ZTIMER_MSEC));
  ```

  - _step 3_: release the mutex so the reader thread can access the content of the buffer.

  ```c
           mutex_unlock(&data.lock);
  ```

5. In the main function, let's finally create the writer thread:

```c
    thread_create(writer_stack, sizeof(writer_stack), THREAD_PRIORITY_MAIN - 1,
                  0, writer_thread, NULL, "writer thread");
```

### Implement the reader loop

In the main function, in the while loop, let's just add the following code:

```c
        mutex_lock(&data.lock);
        printf("%s\n", data.buffer);
        mutex_unlock(&data.lock);
```

Each access to the buffer must be protected with `mutex_lock`/`mutex_unlock` calls to make sure the buffer can be safely read.

### Test the application

Open a terminal with the `File > New > Terminal` menu and run there the following command:

<!-- #raw -->
make -C riot/basics/threading-concurrency all term
<!-- #endraw -->

If everything works as expected, you should get the following output:

<!-- #raw -->
RIOT application with thread safe concurrency
start: 10 - end: 200403
start: 300480 - end: 500729
start: 600802 - end: 800980
start: 901055 - end: 1101233
start: 1201313 - end: 1401458
start: 1501573 - end: 1701730
start: 1801804 - end: 2002069
<!-- #endraw -->

You can try to remove the use of the mutex everywhere and verify that the output can be incomplete sometimes:

<!-- #raw -->
start: 19 - end: 200087
start: 300221
start: 600336
start: 600336 - end: 800405
start: 900490
start: 1200622
start: 1200622 - end: 1400716
<!-- #endraw -->
