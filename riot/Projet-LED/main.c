#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clk.h"

#include "thread.h"
#include "ztimer.h"
#include "shell.h"
#include "led.h"

#include "board.h"
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

/* Declare the lps331ap device variable here */
static lpsxxx_t lpsxxx;

/* Declare the lsm303dlhc device variable here */
static lsm303dlhc_t lsm303dlhc;

/* Declare the isl & 13g device variable here */
static isl29020_t isl29020;

//static l3g4200d_t l3g4200d;
//static l3g4200d_data_t acc_data;

/* Declare and initialize the lsm303dlhc thread lock here */
static mutex_t lsm_lock = MUTEX_INIT_LOCKED;


/* stack memory allocated for the lsm303dlhc thread */
static char lsm303dlhc_stack[THREAD_STACKSIZE_MAIN];


/* Beginning of functions declaration */
/* ********* */


/* LUXmeter */
static void _isl_usage(char *cmd)
{
    printf("usage: %s <start>\n", cmd);
}

/* LEDS */
static void _led_usage(char *cmd)
{
    printf("usage: %s <start>\n", cmd);
}


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

/* LEds */
static int led_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _led_usage(argv[0]);
        return -1;
    }

    /* Implement the LED start subcommands here */
    if (!strcmp(argv[1], "start")) {
        printf("Allumage des LEDS\n");
            LED0_ON;
            LED1_ON;
            LED2_ON;
            LED3_ON;
            LED4_ON;
            LED5_ON;
            LED6_ON;       
            LED7_ON;
    }
    else {
        _led_usage(argv[0]);
        return -1;
    }
    return 0;
}


/* Accelerometer */
static void *lsm303dlhc_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */
        mutex_lock(&lsm_lock);

        /* Read the accelerometer/magnetometer values here */
        lsm303dlhc_3d_data_t mag_value;
        lsm303dlhc_3d_data_t acc_value;
        lsm303dlhc_read_acc(&lsm303dlhc, &acc_value);
        printf("Accelerometer x: %i y: %i z: %i\n",
        acc_value.x_axis, acc_value.y_axis, acc_value.z_axis);
        lsm303dlhc_read_mag(&lsm303dlhc, &mag_value);
        printf("Magnetometer x: %i y: %i z: %i\n",
        mag_value.x_axis, mag_value.y_axis, mag_value.z_axis);

        /* Release the mutex here */
        mutex_unlock(&lsm_lock);

        ztimer_sleep(ZTIMER_MSEC, 500);
    }

    return 0;
}


static void _lsm303dlhc_usage(char *cmd)
{
    printf("usage: %s <start|stop>\n", cmd);
}

static int lsm303dlhc_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }

    /* Implement the lsm303dlhc start/stop subcommands here */
    if (!strcmp(argv[1], "start")) {
        mutex_unlock(&lsm_lock);
    }
    else if (!strcmp(argv[1], "stop")) {
        mutex_trylock(&lsm_lock);
    }
    else {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }

    return 0;
}




/* Thermometer & Barometer */

static void _lpsxxx_usage(char *cmd)
{
    printf("usage: %s <temperature|pressure>\n", cmd);
}

static int lpsxxx_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lpsxxx_usage(argv[0]);
        return -1;
    }

    /* Implement the lps331ap temperature/pressure subcommands here */
    if (!strcmp(argv[1], "temperature")) {
        int16_t temp = 0;
        lpsxxx_read_temp(&lpsxxx, &temp);
        printf("Temperature: %i.%u°C\n", (temp / 100), (temp % 100));
    }
    else if (!strcmp(argv[1], "pressure")) {
        uint16_t pres = 0;
        lpsxxx_read_pres(&lpsxxx, &pres);
        printf("Pressure: %uhPa\n", pres);
    }
    else {
        _lpsxxx_usage(argv[0]);
        return -1;
    }

    return 0;
}

/* Commandes SHELL */

static const shell_command_t commands[] = {
    /* lsm303dlhc shell command handler */
    { "lsm", "start/stop reading accelerometer values", lsm303dlhc_handler },

    /* Add the lps331ap command description here */
    { "lps", "read the lps331ap values", lpsxxx_handler },
    
    /* Add the isl command description here */
    { "isl", "read the isl29020 values", isl_handler },
    
    /* Add the Leds command description here */
    { "led", "Turn the led ON", led_handler },
    
    /* l3g4200d shell command handler 
    { "l3g", "start/stop reading gyro data values", l3g4200d_read },
    */
    
    { NULL, NULL, NULL}
};


int main(void)
{
    
    /* Initialize the lps331ap sensor here */
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);

    /* Initialize the lsm303dlhc sensor here */
    lsm303dlhc_init(&lsm303dlhc, &lsm303dlhc_params[0]);

    /* Initialize the isl29020_init sensor here */
    if (isl29020_init(&isl29020, &isl29020_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }
    
    int numof = 0;

        thread_create(lsm303dlhc_stack, sizeof(lsm303dlhc_stack), THREAD_PRIORITY_MAIN - 1,
                  0, lsm303dlhc_thread, NULL, "lsm303dlhc");
    
    
    /* get the number of available LED's and turn them all off*/
#ifdef LED0_ON
    ++numof;
    LED0_OFF;
#endif
#ifdef LED1_ON
    ++numof;
    LED1_OFF;
#endif
#ifdef LED2_ON
    ++numof;
    LED2_OFF;
#endif
#ifdef LED3_ON
    ++numof;
    LED3_OFF;
#endif
#ifdef LED4_ON
    ++numof;
    LED4_OFF;
#endif
#ifdef LED5_ON
    ++numof;
    LED5_OFF;
#endif
#ifdef LED6_ON
    ++numof;
    LED6_OFF;
#endif
#ifdef LED7_ON
    ++numof;
    LED7_OFF;
#endif

    puts("On-board LED test\n");
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: board-dependent ifdefs) */
    if (numof == 0) {
        puts("NO LEDs AVAILABLE");
    }
    else {
        printf("Available LEDs: %i\n\n", numof);
    }
    
    /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    
    return 0;
}
