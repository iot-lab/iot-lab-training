#include <string.h>

#include "board.h"
#include "timex.h"
#include "ztimer.h"

/* Add sx127x radio driver necessary includes here */


/* Add loramac necessary includes here */


/* Declare the sx127x radio driver descriptor globally here */


/* Declare the loramac descriptor globally here */


/* Device and application parameters required for OTAA activation here */



int main(void)
{
    /* initialize the radio driver */


    /* initialize loramac stack */


    /* configure the device parameters */


    /* change datarate to DR5 (SF7/BW125kHz) */

    
    /* start the OTAA join procedure */


    while (1) {
        char *message = "This is RIOT!";

        /* send the message here */


        /* wait 20 seconds between each message */
        ztimer_sleep(ZTIMER_MSEC, 20 * MS_PER_SEC);
    }

    return 0; /* should never be reached */
}