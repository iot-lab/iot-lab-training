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
//Definition des clé de connection TTN
static const uint8_t appeui[NB_CARTE][LORAMAC_APPEUI_LEN] = 
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};
static const uint8_t deveui[NB_CARTE][LORAMAC_DEVEUI_LEN] = 
{
    { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xD7, 0xD8 },  
    { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xDD, 0x4C }
};
static const uint8_t appkey[NB_CARTE][LORAMAC_APPKEY_LEN]= 
{
    { 0x1C, 0x7A, 0x21, 0x39, 0x8A, 0xDF, 0x48, 0x1E, 0x18, 0x06, 0x4B, 0x13, 0xA5, 0x56, 0x48, 0xBD }, 
    { 0x8B, 0x06, 0x21, 0x7C, 0xFC, 0x0F, 0x5A, 0x2B, 0x92, 0xD5, 0xEF, 0xF1, 0xAA, 0xD4, 0xCC, 0x42 }
};


//variable globale
bool automatik = true;
bool arrosage = false;

//tread de reception
static void *_recv(void *arg)
{
    msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
    (void)arg;
    while (1) {
        /* blocks until a message is received */
        //strcpy((char *)loramac.rx_data.payload, "");
        semtech_loramac_recv(&loramac);
        loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
        printf("Data received: %s, port: %d\n",
               (char *)loramac.rx_data.payload, loramac.rx_data.port);
        
        if(strcmp((char *)loramac.rx_data.payload, "start"))
        {
            printf("on me dit de passer en manuel (start)");
            automatik = false;
            arrosage = true;
        }
        if(strcmp((char *)loramac.rx_data.payload, "stop"))
        {
            printf("on me dit de passer en manuel (stop)");
            automatik = false;
            arrosage = false;
        }
        if(strcmp((char *)loramac.rx_data.payload, "auto"))
        {
            printf("on me dit de repasser en auto");
            automatik = true;
        }
    }
    return NULL;
}


int main(void)
{
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
    semtech_loramac_set_deveui(&loramac, deveui[0]);
    semtech_loramac_set_appeui(&loramac, appeui[0]);
    semtech_loramac_set_appkey(&loramac, appkey[0]);

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
        if (hts221_read_humidity(&hts221, &humidity) != HTS221_OK)
        {
            puts("Cannot read humidity!");
        }
        if (hts221_read_temperature(&hts221, &temperature) != HTS221_OK)
        {
            puts("Cannot read temperature!");
        }
        
        if( automatik )
        {
            printf("    Je suis en mode automatique \n");
            if (humidity < 350)
            {
                arrosage = true;
            }
            else
            {
                arrosage = false;
            }
        }
        else
        {
            printf("    Je suis en mode manuel \n");
        }
        
		//preparation des donnée cayenne
		cayenne_lpp_add_temperature(&lpp, 0, (float)temperature / 10);
		cayenne_lpp_add_relative_humidity(&lpp, 1, (float)humidity / 10);
        //cayenne_lpp_add_output(&lpp, 1, automatik);

        //envoit du message
		printf("Sending LPP data\n");
		//envoit du message en lorawan
		uint8_t ret = semtech_loramac_send(&loramac, lpp.buffer, lpp.cursor);
		if (ret != SEMTECH_LORAMAC_TX_DONE) {
			printf("Cannot send lpp message, ret code: %d\n", ret);
		}
		
		cayenne_lpp_reset(&lpp);
        //delay 30 sec
        
        if (arrosage)
        {
            printf("        J'arrose \n");
        }
        else
        {
            printf("        Je n'arrose pas \n");
        }
        
        ztimer_sleep(ZTIMER_MSEC, 30 * MS_PER_SEC);
    }

    return 0;
}