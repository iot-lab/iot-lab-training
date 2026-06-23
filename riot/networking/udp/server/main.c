#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "timex.h"
#include "ztimer.h"

#include "net/netif.h"

static char server_buffer[128];

int main(void)
{
    /* print network addresses */
    ztimer_sleep(ZTIMER_MSEC, 5 * MS_PER_SEC);
    puts("Configured network interfaces:");
    netifs_print_ipv6("\n");

    /* Create the socket */

    /* Configure the server address */

    /* Bind the socket to the local address */

    while (1) {
        /* Wait for incoming messages, if a message is received */
    }

    return 0;
}
