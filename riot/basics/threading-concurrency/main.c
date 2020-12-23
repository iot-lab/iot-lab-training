#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"

/* Include the mutex header here */


/* Declare here the struct used to handle the buffer and the mutex */


/* Allocate the writer stack here */


static void *writer_thread(void *arg)
{
    (void) arg;

    while (1) {
        /* acquire the lock here */


        /* slowly print some content in the buffer here */


        /* release the mutex here */


        xtimer_usleep(100 * US_PER_MS);
    }

    return NULL;
}

int main(void)
{
    puts("RIOT application with thread safe concurrency");

    /* Create the write thread here */


    while (1) {
        /* safely read the content of the buffer here */


        xtimer_usleep(200 * US_PER_MS);
    }

    return 0;
}