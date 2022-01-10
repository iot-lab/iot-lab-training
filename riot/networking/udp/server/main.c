#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "timex.h"
#include "ztimer.h"

#include "shell.h"

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

static char server_buffer[128];

int main(void)
{
    /* print network addresses */
    ztimer_sleep(ZTIMER_MSEC, 5 * MS_PER_SEC);
    puts("Configured network interfaces:");
    _gnrc_netif_config(0, NULL);

    /* Create the socket */

    /* Configure the server address */

    /* Bind the socket to the local address */

    while (1) {
        /* Wait for incoming messages, if a message is received */
    }

    return 0;
}
