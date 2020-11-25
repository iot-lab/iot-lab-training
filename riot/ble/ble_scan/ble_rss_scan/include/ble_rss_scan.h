#ifndef BLE_RSS_SCAN_H
#define BLE_RSS_SCAN_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BLE_SCAN_TXT_OUTPUT,
    BLE_SCAN_JSON_OUTPUT
} ble_scan_output_fmt_t;


/**
 * @brief   Initialize the ble scan
 *
 * @return true if init went fine, false otherwise
 */
bool ble_rss_scan_init(void);


/**
 * @brief   Performs a ble scan.
 *
 * @param[in] scan_time_out     timeout in ms for the scan window
 * @param[in] scan_cycles       number of scan rounds
 * @param[in] output_format     format of the scan report on stdio
 *
 */
void ble_rss_scan(uint32_t scan_time_out, uint16_t scan_cycles,
                  ble_scan_output_fmt_t output_format);


/**
 * @brief   Starts the advertisement using the autoadv module
 *
 * @param[in] adv_duration_ms   advertisement lifetime in ms, set to INT32_MAX for inifinite advertismeent
 *
 */
void ble_rss_autoadv_start(int32_t adv_duration_ms);


/**
 * @brief   Stops ongoinf advertismenents
 *
 *
 */
void ble_rss_autoadv_stop(void);


/**
 * @brief   Sets the device name on the advertisement packet. Resets advertisement if necessay
 * 
 * @return true if init went fine, false otherwise
 *
 */
bool ble_rss_autoadv_set_device_name(const char* device_name);


/**
 * @brief   Getter for current device name
 *
 * @return the device name string
 *
 */
const char * ble_rss_autoadv_get_device_name(void);

/**
 * @brief  Sets manufacturer specific data in advertisement packet. Resets advertisement if necessay
 * 
 * @return true if init went fine, false otherwise
 *
 */
bool ble_rss_autoadv_set_manufacturer_data(const void *data, size_t data_len);


/**
 * @brief   Helper for getting the current tx power from the BLE phy
 *
 * @return current tx power in dBm
 *
 */
static inline int8_t ble_rss_current_tx_pwr (void) {
    extern int ble_phy_txpwr_get(void); 
    return (int8_t)ble_phy_txpwr_get(); // tx power is only one byte wide
}


#endif /* BLE_RSS_SCAN_H */
/** @} */
