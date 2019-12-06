#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

/* Add lps331ap related include here */

/* Add lsm303dlhc related include here */

/* Declare the lps331ap device variable here */

/* Declare the lsm303dlhc device variable here */

static char stack[THREAD_STACKSIZE_MAIN];

void *thread_handler(void *arg)
{
    (void)arg;

    /* Add the lsm303dlhc sensor polling endless loop here */

    return 0;
}

int main(void)
{
    /* Initialize the lps331ap sensor here */
    
    /* Initialize the lsm303dlhc sensor here */
    
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
               0, thread_handler, NULL, "lsm303dlhc");
    
    /* Add the lps331ap sensor polling endless loop here */
    
    return 0;
}
