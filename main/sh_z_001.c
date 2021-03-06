/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "nvs_flash.h"

#include "sh_z_001.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/


//extern void wifi_ap_task(void *pvParameters);
//extern void webserver_task(void *pvParameters);
extern void gprs_task(void *pvParameters);
extern void display_task(void *pvParameters);
extern void rtc_task(void *pvParameters);

void app_main()
{
	esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
//    xTaskCreate(wifi_ap_task, "wifi_ap", 4096, NULL, 4, NULL);
//    xTaskCreate(webserver_task, "webserver", 8192, NULL, 5, NULL);
    xTaskCreate(&display_task, "display", 4096, NULL, 5, NULL);
    xTaskCreate(rtc_task, "rtc", 2048, NULL, 3, NULL);
    xTaskCreate(&gprs_task, "gprs", 8192, NULL, 5, NULL);

}
