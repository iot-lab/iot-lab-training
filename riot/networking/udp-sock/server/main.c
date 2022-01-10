
#include <stdio.h>

#include "ztimer.h"
#include "shell.h"

/* Add the sock udp header include here */


/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

uint8_t buf[128];

int main(void)
{
    /* print network addresses */
    ztimer_sleep(ZTIMER_MSEC, 5 * MS_PER_SEC);
    puts("Configured network interfaces:");
    _gnrc_netif_config(0, NULL);

    /* Create the sock UDP endpoint here */


    while (1) {
        /* Wait for incoming UDP packets here */

    }
    return 0;
}