#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"
#include "shell.h"

#include "mutex.h"

/* Add lps331ap related include here */


/* Add lsm303dlhc related include here */


/* Declare the lps331ap device variable here */


/* Declare the lsm303dlhc device variable here */


/* stack memory allocated for the lsm303dlhc thread */
static char lsm303dlhc_stack[THREAD_STACKSIZE_MAIN];
/* mutex used to synchronize the main thread (shell) and the lsm303dlhc thread */
static mutex_t lsm_lock;


static void *lsm303dlhc_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */


        /* Read the accelerometer/magnetometer values here */


        /* Release the mutex here */


        xtimer_usleep(delay_ms);
    }

    return 0;
}

static void _lsm303dlhc_usage(char *cmd)
{
    printf("usage: %s <start|stop>\n", cmd);
}

static int lsm303dlhc_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }

    if (argc == 3) {
        delay_ms = atoi(argv[2]) * US_PER_MS;
    }
    else {
        delay_ms = 500 * US_PER_MS;
    }

    /* Implement the lsm303dlhc start/stop subcommands here */


    return 0;
}

static void _lpsxxx_usage(char *cmd)
{
    printf("usage: %s <temperature|pressure>\n", cmd);
}

static int lpsxxx_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lpsxxx_usage(argv[0]);
        return -1;
    }

    /* Implement the lps331ap temperature/pressure subcommands here */


    return 0;
}

static const shell_command_t commands[] = {
    /* Add the lps331ap command description here */

    /* Add the lsm303dlhc command description here */

    { NULL, NULL, NULL}
};

int main(void)
{
    /* The accelerometer is locked here to make sure the accelerometer
     *  thread is blocked at startup and no accelometer/magnetometer values 
     *  are read if not requested to. */
    mutex_lock(&lsm_lock);

    /* Initialize the lps331ap sensor here */


    /* Initialize the lsm303dlhc sensor here */


    thread_create(lsm303dlhc_stack, sizeof(lsm303dlhc_stack), THREAD_PRIORITY_MAIN - 1,
                  0, lsm303dlhc_thread, NULL, "lsm303dlhc");

    /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
