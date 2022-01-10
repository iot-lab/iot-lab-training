#include <assert.h>

#include "ztimer.h"

#include "nimble_scanner.h"
#include "nimble_scanlist.h"
#include "net/bluetil/ad.h"
#include "nimble/hci_common.h"

#include "nimble_autoadv.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "ble_rss_scan.h"


// local helpers
static void _print_buffer(const uint8_t *addr, uint8_t size, bool reverse);
static void _scanlist_json_print(void);
static void _scanlist_json_print_entry(nimble_scanlist_entry_t *e);

bool ble_rss_scan_init(void)
{
    /* We want Nimble to scan 'full time', so we set the
     * window equal the interval */
    struct ble_gap_disc_params scan_params = {
        .itvl = BLE_GAP_LIM_DISC_SCAN_INT,
        .window = BLE_GAP_LIM_DISC_SCAN_WINDOW,
        .filter_policy = 0,     /* don't use */
        .limited = 0,           /* no limited discovery */
        .passive = 0,           /* no passive scanning */
        .filter_duplicates = 0, /* no duplicate filtering */
    };

    /* initialize the nimble scanner */
    nimble_scanlist_init();
    int ret = nimble_scanner_init(&scan_params, nimble_scanlist_update);

    return ret == NIMBLE_SCANNER_OK;
}


void ble_rss_scan(uint32_t scan_time_out, uint16_t scan_cycles,
                  ble_scan_output_fmt_t output_format)
{

    for (int i = 0; i < scan_cycles; i++) {
        nimble_scanlist_clear();
        nimble_scanner_start();
        ztimer_sleep(ZTIMER_USEC, scan_time_out);
        nimble_scanner_stop();
        if (output_format == BLE_SCAN_JSON_OUTPUT) {
            _scanlist_json_print();
        }
        else {
            nimble_scanlist_print();
        }
    }
}

/* Advertisement management */
static int32_t _adv_duration_ms = BLE_HS_FOREVER;
#define MAX_VENDOR_DATA_SIZE (BLE_ADV_PDU_LEN - 2)
static struct {
    uint8_t data[MAX_VENDOR_DATA_SIZE];
    size_t data_len;
    bool valid;
} manufacturer_specific_data = {.data_len=0, .valid=false};

void ble_rss_autoadv_start(int32_t adv_duration_ms)
{
    int nimlble_ret;
    static struct  ble_gap_adv_params adv_params;
    const char * device_name = ble_svc_gap_device_name();

    nimble_autoadv_init();
    // setup Advertisement
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    nimble_autoadv_set_ble_gap_adv_params(&adv_params);
    _adv_duration_ms = adv_duration_ms; 
    nimble_auto_adv_set_adv_duration(_adv_duration_ms);
    /* setup advertised data to contain:
     - Device name 
     - current tx power
     - Manufacturer specic data if any
    */
    nimlble_ret = nimble_autoadv_add_field(BLE_GAP_AD_NAME_SHORT, device_name, strlen(device_name));
    assert(nimlble_ret==BLUETIL_AD_OK);
    int8_t phy_txpwr_dbm = ble_rss_current_tx_pwr(); // FIXME use services from the ble stack to include this automatically avoiding calling the driver
    nimlble_ret = nimble_autoadv_add_field(BLE_GAP_AD_TX_POWER_LEVEL, &phy_txpwr_dbm, sizeof(phy_txpwr_dbm));
    assert(nimlble_ret==BLUETIL_AD_OK);
    if(manufacturer_specific_data.valid) {
        nimlble_ret = nimble_autoadv_add_field(BLE_GAP_AD_VENDOR, manufacturer_specific_data.data, manufacturer_specific_data.data_len);
        assert(nimlble_ret==BLUETIL_AD_OK);
    }

    // start 
    nimble_autoadv_start();
}

void ble_rss_autoadv_stop(void)
{
    nimble_autoadv_stop();
}

bool ble_rss_autoadv_set_device_name(const char* device_name)
{
    bool ret = !ble_svc_gap_device_name_set(device_name);
    if(ret) {
        // restart advertisement if active to make new name effective
        if(ble_gap_adv_active()) {
            ble_rss_autoadv_stop();
            ble_rss_autoadv_start(_adv_duration_ms);
        }
    }
    return ret;
}

const char * ble_rss_autoadv_get_device_name(void)
{
    return ble_svc_gap_device_name();
}

#define BLE_COMPANY_IDENTIFIER 0xCAFE
bool ble_rss_autoadv_set_manufacturer_data(const void *data, size_t data_len)
{
    if (data_len > MAX_VENDOR_DATA_SIZE) {
        return false;
    }

    if (data != NULL) {
        // Prefix header with company idetifier: 0xCAFE ;)
        manufacturer_specific_data.data[0] = BLE_COMPANY_IDENTIFIER & 0xFF;
        manufacturer_specific_data.data[1] = (BLE_COMPANY_IDENTIFIER & 0xFF00) >> 8;
        // Copy Payload
        memcpy(&(manufacturer_specific_data.data[2]), data, data_len);
        manufacturer_specific_data.data_len = data_len + 2;   // payload + header
        manufacturer_specific_data.valid = true;
    }
    else {
        // diasable advertisement
        manufacturer_specific_data.valid = false;
    }
    
    // restart advertisement if active to make new data effective
    if (ble_gap_adv_active()) {
        ble_rss_autoadv_stop();
        ble_rss_autoadv_start(_adv_duration_ms);
    }
    return true;
}

// local helpers implementation

/**
 * @brief Json dumps a byte array in hex format. Each byte as hex string.
 *
 * @param[in] addr         starting adress
 * @param[in] size         number of bytes to dump
 * @param[in] reverse      dump from end to start if true, or from start to end otherwise
 *
 */
static void _print_buffer(const uint8_t *addr, uint8_t size, bool reverse)
{
    printf("[");
    if (reverse) {
        for (int i = size - 1; i >= 0; i--) {
            printf("\"%02X\"", addr[i]);
            if (i > 0) {
                printf(",");
            }
        }
    }
    else {
        for (int i = 0; i < size; i++) {
            printf("\"%02X\"", addr[i]);
            if (i < size - 1) {
                printf(",");
            }
        }
    }
    printf("]");
}

/**
 * @brief   Json dump the entire scanlist to STDIO using nimble_scanlist_print_entry()
 */
static void _scanlist_json_print(void)
{
    nimble_scanlist_entry_t *e = nimble_scanlist_get_next(NULL);

    printf("{\"nodes\": [");
    while (e) {
        _scanlist_json_print_entry(e);
        e = nimble_scanlist_get_next(e);
        if (e) {
            printf(", ");
        }
    }
    printf("]}\n");
}

/**
 * @brief   Json dump a single scanlist entry to STDIO
 *
 * @param[in] e         entry to dump
 */
static void _scanlist_json_print_entry(nimble_scanlist_entry_t *e)
{
    assert(e);

    /* try to find a device name short or complete */
    char name[(BLE_ADV_PDU_LEN + 1)] = { 0 };
    bluetil_ad_t ad = BLUETIL_AD_INIT(e->ad, e->ad_len, e->ad_len);
    int res = bluetil_ad_find_str(&ad, BLE_GAP_AD_NAME, name, sizeof(name));

    if (res != BLUETIL_AD_OK) {
        res =
            bluetil_ad_find_str(&ad, BLE_GAP_AD_NAME_SHORT, name, sizeof(name));
    }
    if (res != BLUETIL_AD_OK) {
        strncpy(name, "undefined", sizeof(name));
    }
    /* try to find tx power */
    int8_t phy_txpwr_dbm;
    bluetil_ad_data_t field;
    res = bluetil_ad_find(&ad, BLE_GAP_AD_TX_POWER_LEVEL, &field);                
    if (res == BLUETIL_AD_OK) {
        phy_txpwr_dbm = field.data[0]; // expected length is one byte
    } else {
        phy_txpwr_dbm = INT8_MAX; // value when not found
    }

    printf("{");
    printf("\"addr\":"); _print_buffer(e->addr.val, 6, true);
    printf(",\"addr_type\":%d", e->addr.type);
    printf(",\"name\":\"%s\"", name);
    printf(",\"adv_type\":%d", e->type);
    printf(",\"ad_data\":"); _print_buffer(e->ad, e->ad_len, false);
    printf(",\"last_rssi\":%i", (int)e->last_rssi);
    printf(",\"txpwr_dbm\":%i", phy_txpwr_dbm);
    printf(",\"adv_msg_cnt\":%u", (unsigned)e->adv_msg_cnt);
    printf(",\"first_update\":%u", (unsigned)e->first_update);
    printf(",\"last_update\":%u", (unsigned)e->last_update);
    printf("}");
}
