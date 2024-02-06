#include "example/example.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <rndl/utils.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "app";

#define MS_TO_US(ms) ((ms) * 1000)

static rndl_led_driver_handle_t led_driver = NULL;
static const rndl_led_panel_driver_config_t config = {
    .gpio_num = CONFIG_RNDL_GPIO,
    .resolution_hz = RNDL_MHZ(CONFIG_RNDL_RESOLUTION_MHZ),
    .led_count = (CONFIG_RNDL_ROWS * CONFIG_RNDL_COLS),
    .timeout_ms = portMAX_DELAY,
};

static rndl_surface_handle_t surface = NULL;
static const rndl_surface_config_t surface_config = {
    .width = CONFIG_RNDL_COLS,
    .height = CONFIG_RNDL_ROWS,
};

static EventGroupHandle_t core_events;
typedef enum {
    core_event_none = 0,
    core_event_render_timer = BIT0, /*!< A frame is ready to render */
} core_events_t;

static void render_timer_cb(void *arg) {
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;

    xResult = xEventGroupSetBitsFromISR(core_events, core_event_render_timer, &xHigherPriorityTaskWoken);

    if (xResult != pdFAIL) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void render_task(void *pvParameters) {

    const esp_timer_create_args_t render_timer_args = {
        .callback = &render_timer_cb,
        .name = "render_timer",
    };
    esp_timer_handle_t render_timer;
    ESP_ERROR_CHECK(esp_timer_create(&render_timer_args, &render_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(render_timer, MS_TO_US(RNDL_FPS_TO_MS(CONFIG_RNDL_FPS))));

    ESP_LOGI(TAG, "render task started");

    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(core_events, core_event_render_timer, pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & core_event_render_timer) {
            surface->render(surface);
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "initializing");

    ESP_ERROR_CHECK(rndl_led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(rndl_surface_create(&surface_config, led_driver, &surface));

    core_events = xEventGroupCreate();
    ESP_ERROR_CHECK(core_events == NULL ? ESP_FAIL : ESP_OK);

    BaseType_t xResult = xTaskCreate(render_task, "render_task", CONFIG_RNDL_RENDER_STACK_SIZE_BYTES, NULL,
                                     CONFIG_RNDL_RENDER_PRIORITY, NULL);
    ESP_ERROR_CHECK(xResult != pdPASS ? ESP_FAIL : ESP_OK);

    ESP_LOGI(TAG, "startup complete");

    example_bitmap(surface, &surface_config);

    ESP_LOGI(TAG, "pattern complete");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}
