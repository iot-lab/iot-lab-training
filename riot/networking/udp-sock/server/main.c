
#include <stdio.h>

#include "ztimer.h"

/* Add the sock udp header include here */


uint8_t buf[128];

int main(void)
{
    /* print network addresses */
    ztimer_sleep(ZTIMER_MSEC, 5 * MS_PER_SEC);
    puts("Configured network interfaces:");
    netifs_print_ipv6("\n");

    /* Create the sock UDP endpoint here */


    while (1) {
        /* Wait for incoming UDP packets here */

    }
    return 0;
}
