#include <string.h>

#include "board.h"
#include "xtimer.h"

/* Add necessary include here */



/* Declare the loramac descriptor globally here */



/* Device and application parameters required for OTAA activation here */



int main(void)
{
    /* initialize loramac stack */

    
    /* configure the device parameters */


    /* change datarate to DR5 (SF7/BW125kHz) */

    
    /* start the OTAA join procedure */


    while (1) {
        char *message = "This is RIOT!";

        /* send the message here */


        /* wait 20 seconds between each message */
        xtimer_sleep(20);
    }

    return 0; /* should never be reached */
}