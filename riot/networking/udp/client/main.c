#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "net/af.h"
#include "net/protnum.h"
#include "net/ipv6/addr.h"
#include "timex.h"
#include "ztimer.h"

#define CLIENT_MESSAGE "Hello Server"

int main(void)
{
    /* Create the socket */

    /* configure the destination address */

    while (1) {
        /* send CLIENT_MESSAGE to the server */

        ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    }

    return 0;
}
