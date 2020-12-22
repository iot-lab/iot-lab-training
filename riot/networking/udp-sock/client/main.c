#include <stdio.h>

#include "xtimer.h"

#include "net/af.h"
#include "net/protnum.h"

/* Add the IPv6 and UDP header includes here */


uint8_t buf[128];

#define CLIENT_MESSAGE "Hello Server"

int main(void)
{
    /* Create the sock UDP endpoint here */


    /* Configure the remote sock UDP endpoint here */


    while (1) {
        /* Send the message here */

        /* Wait for the server reply here */
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
                                NULL)) < 0) {
            if (res == -ETIMEDOUT) {
                puts("Timed out");
            }
            else {
                puts("Error receiving message");
            }
        }
        else {
            printf("Reply received: \"");
            for (int i = 0; i < res; i++) {
                printf("%c", buf[i]);
            }
            puts("\"");
        }

        xtimer_sleep(1);
    }
    return 0;
}