#include "cJSON.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MQTT_LED_GPIO GPIO_NUM_4
#define WIFI_LED_GPIO GPIO_NUM_2

static bool mqttLedTaskEnabled = false;
#define BUTTON_GPIO GPIO_NUM_5

static const char *TAG = "MQTT_EXAMPLE";

int aws_iot_demo_main(int argc, char **argv);
void mqtt_message_processor(const char *topic, const char *payload);
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT) {
    if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
      gpio_set_level(WIFI_LED_GPIO, 0);
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
      gpio_set_level(WIFI_LED_GPIO, 1);
    }
  }
}


static void mqtt_led_task(void *arg) {
  ESP_LOGI(TAG, "MQTT LED Task started");
  while (true) {
    if (mqttLedTaskEnabled) {
      gpio_set_level(MQTT_LED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(1000));
      gpio_set_level(MQTT_LED_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    } else {
      gpio_set_level(MQTT_LED_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

static void button_task(void *arg) {
    esp_rom_gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_GPIO);

    while(true) {
        int button_state = gpio_get_level(BUTTON_GPIO);
        if(button_state == 0) {
            mqttLedTaskEnabled = false;
            vTaskDelay(pdMS_TO_TICKS(200)); // Debouncing
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Polling interval
    }
}

void print_hex(const char *s, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ESP_LOGI(TAG, "%02x ", (unsigned char)s[i]);
  }
  ESP_LOGI(TAG, "\n");
}

void mqtt_message_processor(const char *topic, const char *payload) {
  ESP_LOGI(TAG, "Entered mqtt_message_processor");
  ESP_LOGI(TAG, "Received message on topic: %s with payload: %s", topic,
           payload);


  const char *call_comp_str =
      "LED/toggle";


  ESP_LOGI(TAG,
           "Length of received topic: %d, Length of compared call topic: %d",
           strlen(topic), strlen(call_comp_str));


  if (strncmp(topic, call_comp_str, strlen(call_comp_str)) == 0) {

    if (strstr(topic,"LED/toggle{\"message\": \"call\"}llo World!")) {
      ESP_LOGI(TAG, "Payload indicates 'call'");
      mqttLedTaskEnabled =
          true;
      ESP_LOGI(TAG, "Started MQTT LED task");
    } else if (strstr(topic, "LED/toggle{\"message\": \"reset\"}lo World!")) {
                                                           // payload format
      ESP_LOGI(TAG, "Payload indicates 'reset'");
      mqttLedTaskEnabled =
          false;
      ESP_LOGI(TAG, "Stopped MQTT LED task");
    }
  } else {
    ESP_LOGI(TAG, "Topic does not match any known messages");
  }
}

void app_main() {

  esp_rom_gpio_pad_select_gpio(MQTT_LED_GPIO);
  gpio_set_direction(MQTT_LED_GPIO, GPIO_MODE_OUTPUT);

  esp_rom_gpio_pad_select_gpio(WIFI_LED_GPIO);
  gpio_set_direction(WIFI_LED_GPIO, GPIO_MODE_OUTPUT);

  gpio_set_level(MQTT_LED_GPIO, 1);
  gpio_set_level(WIFI_LED_GPIO, 1);
  vTaskDelay(pdMS_TO_TICKS(5000)); // wait for 5 seconds
  gpio_set_level(MQTT_LED_GPIO, 0);
  gpio_set_level(WIFI_LED_GPIO, 0);

  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes",
           esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

  // Initialize NVS (Non-Volatile Storage)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize the TCP/IP stack
  ESP_ERROR_CHECK(esp_netif_init());

  // Initialize the event loop
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Initialize the MQTT LED GPIO (GPIO 2)
  esp_rom_gpio_pad_select_gpio(MQTT_LED_GPIO);
  gpio_set_direction(MQTT_LED_GPIO, GPIO_MODE_OUTPUT);

  // Initialize the WiFi LED GPIO (GPIO 4)
  esp_rom_gpio_pad_select_gpio(WIFI_LED_GPIO);
  gpio_set_direction(WIFI_LED_GPIO, GPIO_MODE_OUTPUT);

  // Register WiFi event handler
  ESP_LOGI(TAG, "[APP] Registering WiFi event handler.");
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &wifi_event_handler, NULL));

  // Connect to WiFi
  ESP_LOGI(TAG, "[APP] Connecting to WiFi.");
  ESP_ERROR_CHECK(example_connect());


  xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);

  xTaskCreate(mqtt_led_task, "mqtt_led_task", 2048, NULL, 5, NULL);

  // Run the AWS IoT demo
  aws_iot_demo_main(0, NULL);
}
