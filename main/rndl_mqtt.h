#include <esp_err.h>

typedef void (*rndl_on_mqtt_event_data_t)(const char *data);

esp_err_t rndl_mqtt_start(rndl_on_mqtt_event_data_t cb);