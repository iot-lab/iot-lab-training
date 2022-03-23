#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clk.h"

#include "thread.h"
#include "ztimer.h"
#include "shell.h"
#include "led.h"

/* Add sx127x radio driver necessary includes here */
//#include "sx127x.h"
//#include "sx127x_netdev.h"
//#include "sx127x_params.h"

/* Add loramac necessary includes here */
//#include "net/loramac.h"     /* core loramac definitions */
//#include "semtech_loramac.h" /* package API */
//#include "hts221.h"
//#include "hts221_params.h"

#include "board.h"
#include "board_common.h"
//#include "gpio_params.h"

#include "periph_conf.h"

#include "mutex.h"

/* Add lps331ap related include here */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Add lsm303dlhc related include here */
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

/* Add isl & 13g related include here */
#include "isl29020.h"
#include "isl29020_params.h"

#include "l3g4200d.h"
#include "l3g4200d_params.h"


/* Declare the isl & 13g device variable here */
static isl29020_t isl29020;


//static l3g4200d_t l3g4200d;
//static l3g4200d_data_t acc_data;

/* LUXmeter */
static void _isl_usage(char *cmd)
{
    printf("usage: %s <start>\n", cmd);
}

/* Declare the sx127x radio driver descriptor globally here */
//static sx127x_t sx127x;      /* The sx127x radio driver descriptor */

/* Declare the loramac descriptor globally here */
//static semtech_loramac_t loramac;  /* The loramac stack descriptor */



/* Device and application parameters required for OTAA activation here */
//static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xC6, 0xFA };
//static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x8B, 0x9A, 0xDC, 0xF0, 0x5C, 0x14, 0x52, 0xDD, 0xDA, 0xBE, 0xD9, 0xE7, 0x95, 0x62, 0x06, 0x1E };

static int isl_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _isl_usage(argv[0]);
        return -1;
    }

    /* Implement the isl start subcommands here */
    if (!strcmp(argv[1], "start")) {
        printf("Light value: %5i LUX\n", isl29020_read(&isl29020));
        
    }
    else {
        _isl_usage(argv[0]);
        return -1;
    }

    return 0;
}


/* Beginning of functions declaration */
/* ********* */

/* Commandes SHELL */

static const shell_command_t commands[] = {

    
    /* Add the isl command description here */
    { "isl", "read the isl29020 values", isl_handler },

    
    /* l3g4200d shell command handler 
    { "l3g", "start/stop reading gyro data values", l3g4200d_read },
    */
    
    { NULL, NULL, NULL}
};



int main(void)
{
    
    /* initialize the radio driver */
    //sx127x_setup(&sx127x, &sx127x_params[0], 0);
    //loramac.netdev = &sx127x.netdev;
    //loramac.netdev->driver = &sx127x_driver;

    /* initialize loramac stack */
    //semtech_loramac_init(&loramac);

    /* configure the device parameters */
    //semtech_loramac_set_deveui(&loramac, deveui);
    //semtech_loramac_set_appeui(&loramac, appeui);
    //semtech_loramac_set_appkey(&loramac, appkey);

    /* change datarate to DR5 (SF7/BW125kHz) */
    //semtech_loramac_set_dr(&loramac, 5);
    
    /* start the OTAA join procedure */
    //if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
      //  puts("Join procedure failed");
      //  return 1;
    //}
    puts("Join procedure succeeded");
    


    /* Initialize the isl29020_init sensor here */
    if (isl29020_init(&isl29020, &isl29020_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }
    

    uint16_t luxmetre = 0;

    //while (1) {
        
        /* do some measurements */
        //printf("Light value: %5i LUX\n", isl29020_read(&isl29020));
        luxmetre=isl29020_read(&isl29020);
    

        char message[64];
        sprintf(message, "LUX: %d",luxmetre);
        printf("Sending message '%s'\n", message);

        /* send the message here */
        //if (semtech_loramac_send(&loramac,
        //                         (uint8_t *)message, strlen(message)) != SEMTECH_LORAMAC_TX_DONE) {
        //    printf("Cannot send message '%s'\n", message);
        //}
        //else {
        //    printf("Message '%s' sent\n", message);
        //}

        /* wait 20 seconds between each message */
        ztimer_sleep(ZTIMER_MSEC, 3 * MS_PER_SEC);
        
            /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);
        
    //}
    
    
    return 0;
}
