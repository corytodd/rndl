#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rndl/platform.h>

void platform_delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
