/*
 * internet.c
 *
 *  Created on: Apr 17, 2018
 *      Author: zulolo
 */

#include "sh_z_001.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/semphr.h"

#include "driver/uart.h"

#include "netif/ppp/pppos.h"
#include "netif/ppp/ppp.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/pppapi.h"

#include <esp_event.h>
#include <esp_wifi.h>

#include "apps/sntp/sntp.h"
#include "cJSON.h"

#include "libGSM.h"

static const char *TIME_TAG = "[SNTP]";

void gprs_task(void *pvParameters) {
	if (ppposInit() == 0) {
		ESP_LOGE("PPPoS EXAMPLE", "ERROR: GSM not initialized, HALTED");
		while (1) {
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	}

	// ==== Get time from NTP server =====
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 10;

	time(&now);
	localtime_r(&now, &timeinfo);

	while (1) {
		printf("\r\n");
		ESP_LOGI(TIME_TAG, "OBTAINING TIME");
		ESP_LOGI(TIME_TAG, "Initializing SNTP");
		sntp_setoperatingmode(SNTP_OPMODE_POLL);
		sntp_setservername(0, "pool.ntp.org");
		sntp_init();
		ESP_LOGI(TIME_TAG, "SNTP INITIALIZED");

		// wait for time to be set
		now = 0;
		while ((timeinfo.tm_year < (2016 - 1900)) && (++retry < retry_count)) {
			ESP_LOGI(TIME_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			time(&now);
			localtime_r(&now, &timeinfo);
			if (ppposStatus() != GSM_STATE_CONNECTED)
				break;
		}
		if (ppposStatus() != GSM_STATE_CONNECTED) {
			sntp_stop();
			ESP_LOGE(TIME_TAG, "Disconnected, waiting for reconnect");
			retry = 0;
			while (ppposStatus() != GSM_STATE_CONNECTED) {
				vTaskDelay(100 / portTICK_RATE_MS);
			}
			continue;
		}

		if (retry < retry_count) {
			ESP_LOGI(TIME_TAG, "TIME SET TO %s", asctime(&timeinfo));
			break;
		} else {
			ESP_LOGI(TIME_TAG, "ERROR OBTAINING TIME\n");
		}
		sntp_stop();
		break;
	}

	while (1) {
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
