//mosquitto_sub -h eu1.cloud.thethings.network --cafile riot/lorawan/ttn-mqtt/isrgrootx1.pem -p 8883 -t 'v3/+/devices/+/up' -u 'picsou@ttn' -P 'NNSXS.3BDUTAKTSCGAFTTEKWSOGLIMZO24M44GVMHYNSY.6GBXYOV26OQFXALPU3ZIUJZF2WJL7GPI7POUB3Z5ITUFPKZL5MCA' -v
//mosquitto_pub -h eu1.cloud.thethings.network --cafile riot/lorawan/ttn-mqtt/isrgrootx1.pem -p 8883 -t 'v3/picsou@ttn/devices/arroseur/down/push' -u 'picsou@ttn' -P 'NNSXS.3BDUTAKTSCGAFTTEKWSOGLIMZO24M44GVMHYNSY.6GBXYOV26OQFXALPU3ZIUJZF2WJL7GPI7POUB3Z5ITUFPKZL5MCA' -m '{"downlinks":[{"f_port": 42,"frm_payload":"U2FsdXQgbGVzIGVuZmFudCBjJ2VzdCBkYWRkeSBjaG9jb2xhdA==","priority": "NORMAL"}]}'



#include <string.h>
#include "board.h"
#include "timex.h"
#include "ztimer.h"
#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"
#include "thread.h"
#include "cayenne_lpp.h"
//loramac
#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
//capteurs
#include "hts221.h"
#include "hts221_params.h"

//define constantes
#define NB_CARTE 2
#define RECV_MSG_QUEUE                   (4U)

static msg_t _recv_queue[RECV_MSG_QUEUE];
static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

//les descripteurs
static sx127x_t sx127x;      /* The sx127x radio driver descriptor */
static semtech_loramac_t loramac;  /* The loramac stack descriptor */
static hts221_t hts221; /* The HTS221 device descriptor */
static cayenne_lpp_t lpp;

//les AppKey et Tutti Canti
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xD7, 0xD8 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x1C, 0x7A, 0x21, 0x39, 0x8A, 0xDF, 0x48, 0x1E, 0x18, 0x06, 0x4B, 0x13, 0xA5, 0x56, 0x48, 0xBD };


//tread de reception
static void *_recv(void *arg)
{
    msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
    (void)arg;
    while (1) {
        /* blocks until a message is received */
        semtech_loramac_recv(&loramac);
        loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
        printf("Data received: %s, port: %d\n",
               (char *)loramac.rx_data.payload, loramac.rx_data.port);
    }
    return NULL;
}


int main(void)
{
    printf("dingo l'ami de mickey");
	
    //Initialisation capteur
    if (hts221_init(&hts221, &hts221_params[0]) != HTS221_OK) {
        puts("Sensor initialization failed");
        return 1;
    }
    if (hts221_power_on(&hts221) != HTS221_OK) {
        puts("Sensor initialization power on failed");
        return 1;
    }
    if (hts221_set_rate(&hts221, hts221.p.rate) != HTS221_OK) {
        puts("Sensor continuous mode setup failed");
        return 1;
    }

	//initialisation Lora
    sx127x_setup(&sx127x, &sx127x_params[0], 0);
    loramac.netdev = &sx127x.netdev;
    loramac.netdev->driver = &sx127x_driver;

    /* initialize the loramac stack */
    semtech_loramac_init(&loramac);
    
    /* configure the device parameters */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    /* change datarate to DR5 (SF7/BW125kHz) */
    semtech_loramac_set_dr(&loramac, 5);
    
    /* start the OTAA join procedure */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

	//creation du tread
    thread_create(_recv_stack, sizeof(_recv_stack),
            THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread");

    while (1) {
        //faire les mesures
        uint16_t humidity = 0;
        int16_t temperature = 0;
        if (hts221_read_humidity(&hts221, &humidity) != HTS221_OK) {
            puts("Cannot read humidity!");
        }
        if (hts221_read_temperature(&hts221, &temperature) != HTS221_OK) {
            puts("Cannot read temperature!");
        }

        /*char message[64];
        sprintf(message, "H: %d.%d%%, T:%d.%dC",
                (humidity / 10), (humidity % 10),
                (temperature / 10), (temperature % 10));
        printf("Sending message '%s'\n", message);*/
		
		//preparation des donnée cayenne
		cayenne_lpp_add_temperature(&lpp, 0, (float)temperature / 10);
		cayenne_lpp_add_relative_humidity(&lpp, 1, (float)humidity / 10);

        //envoit du message
		printf("Sending LPP data\n");
		//envoit du message en lorawan
		uint8_t ret = semtech_loramac_send(&loramac, lpp.buffer, lpp.cursor);
		if (ret != SEMTECH_LORAMAC_TX_DONE) {
			printf("Cannot send lpp message, ret code: %d\n", ret);
		}
        /*if (semtech_loramac_send(&loramac,
                                 (uint8_t *)message, strlen(message)) != SEMTECH_LORAMAC_TX_DONE) {
            printf("Cannot send message '%s'\n", message);
        }
        else {
            printf("Message '%s' sent\n", message);
        }*/
		
		cayenne_lpp_reset(&lpp);
        //delay 30 sec
        ztimer_sleep(ZTIMER_MSEC, 30 * MS_PER_SEC);
    }

    return 0;
}