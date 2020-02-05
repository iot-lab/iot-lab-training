#include "liblwm2m.h"

/**
 * @brief LWM2M temperature object id
 */
#define LWM2M_TEMP_OBJECT_ID   (3303)

/**
 * @brief LWM2M temperature object instance descriptor
 */
typedef struct lwm2m_temp_instance {
    struct temp_instance *next;  /**< matches lwm2m_list_t::next */
    uint16_t shortID;            /**< matches lwm2m_list_t::id */
    double sensor_value;
} lwm2m_temp_instance_t;


/**
 * @brief LWM2M temperature object resource ids
 */
typedef enum {
    LWM2M_TEMP_RES_SENSOR_VALUE = 5700,
    LWM2M_TEMP_RES_SENSOR_UNITS,
    LWM2M_TEMP_RES_MEASURED_MIN = 5601,
    LWM2M_TEMP_RES_MEASURED_MAX,
    LWM2M_TEMP_RES_RANGE_MIN,
    LWM2M_TEMP_RES_RANGE_MAX,
    LWM2M_TEMP_RES_RESET_MEASURED
} lwm2m_temp_resource_t;


/**
 * @brief Create temperature object
 *
 * @return Pointer to the created object
 */
lwm2m_object_t *lwm2m_client_get_temperature_object(void);
