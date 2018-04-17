/*
 * wifi_config_task.c
 *
 *  Created on: Apr 5, 2018
 *      Author: zulolo
 */


#include "esp_event_loop.h"

#include "nvs_flash.h"
#include "sh_z_001.h"

#define DEFAULT_SSID 			"SH-Z-001"
#define DEFAULT_PASSWORD 		"66668888"
#define DEFAULT_SSID_HIDDEN		0

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

static const char *TAG_WIFI = "[WIFI]";
static const char *WIFI_AP_NVS_NS = "sh_wifi_ap";
static const char *WIFI_AP_SSID_NVS_NS = "ap_ssid";
static const char *WIFI_AP_PSWD_NVS_NS = "ap_pswd";
static const char *WIFI_AP_SSID_HID_NVS_NS = "ap_ssid_hid";
static wifi_config_t wifi_config;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_AP_START:
    	xEventGroupSetBits(wifi_event_group, WIFI_EVENT_GROUP_CONNECTED_BIT);
		break;

	case SYSTEM_EVENT_AP_STACONNECTED:

		break;

	case SYSTEM_EVENT_AP_STADISCONNECTED:

		break;
    default:
        break;
    }
    return ESP_OK;
}

static esp_err_t set_ssid_to_nvs(char *ssid, char *password, uint8_t ssid_hidden)
{
	nvs_handle my_handle;
	esp_err_t err;

    err = nvs_open(WIFI_AP_NVS_NS, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG_WIFI, "nvs open with error %d.\n", err);
        return err;
    }

	if (ESP_OK != nvs_set_str(my_handle, WIFI_AP_SSID_NVS_NS, ssid)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	if (ESP_OK != nvs_set_str(my_handle, WIFI_AP_PSWD_NVS_NS, password)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	if (ESP_OK != nvs_set_u8(my_handle, WIFI_AP_SSID_HID_NVS_NS, ssid_hidden)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	// Close
	nvs_close(my_handle);

	return ESP_OK;
}

static esp_err_t get_ssid_from_nvs(char *ssid, size_t ssid_len, char *password, size_t password_len, uint8_t* ssid_hidden)
{
	nvs_handle my_handle;
	esp_err_t err;

    err = nvs_open(WIFI_AP_NVS_NS, NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG_WIFI, "nvs open with error %d.\n", err);
        return err;
    }

	if (ESP_OK != nvs_get_str(my_handle, WIFI_AP_SSID_NVS_NS, ssid, &ssid_len)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	if (ESP_OK != nvs_get_str(my_handle, WIFI_AP_PSWD_NVS_NS, password, &password_len)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	if (ESP_OK != nvs_get_u8(my_handle, WIFI_AP_SSID_HID_NVS_NS, ssid_hidden)){
		nvs_close(my_handle);
		return ESP_ERR_NOT_FOUND;
	}
	// Close
	nvs_close(my_handle);

	ESP_LOGI(TAG_WIFI, "Read SSID:%s and password:%s from nvs, ", ssid, password);
	return ESP_OK;
}

static void start_wifi_ap(void)
{
    // check if there is configured SSID and password
    if (ESP_OK != get_ssid_from_nvs((char *)(wifi_config.ap.ssid), sizeof(wifi_config.ap.ssid),
    		(char *)(wifi_config.ap.password), sizeof(wifi_config.ap.password), &wifi_config.ap.ssid_hidden)) {
    	set_ssid_to_nvs(DEFAULT_SSID, DEFAULT_PASSWORD, DEFAULT_SSID_HIDDEN);
        strcpy((char *)(wifi_config.ap.ssid), DEFAULT_SSID);
        strcpy((char *)(wifi_config.ap.password), DEFAULT_PASSWORD);
        wifi_config.ap.ssid_hidden = 0;
    }
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = 1;
    wifi_config.ap.beacon_interval = 100;

    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}

int get_light_para_nvs_key_by_index(char *key_name, const char *key_name_prefix, uint8_t light_index)
{
	return snprintf(key_name, 15, "%s_%u", key_name_prefix, light_index);
}

static void save_para_to_nvs(void)
{

}

static void start_dhcp_server(){

    	// initialize the tcp stack
	    tcpip_adapter_init();
        // stop DHCP server
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
        // assign a static IP to the network interface
        tcpip_adapter_ip_info_t info;
        memset(&info, 0, sizeof(info));
        IP4_ADDR(&info.ip, 192, 168, 1, 1);
        IP4_ADDR(&info.gw, 192, 168, 1, 1);//ESP acts as router, so gw addr will be its own addr
        IP4_ADDR(&info.netmask, 255, 255, 255, 0);
        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
        // start the DHCP server
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
        printf("DHCP server started \n");
}

void wifi_ap_task(void *pvParameters)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    xEventGroupClearBits(wifi_event_group, WIFI_EVENT_GROUP_CONNECTED_BIT);
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    start_dhcp_server();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    xEventGroupSetBits(wifi_event_group, WIFI_EVENT_GROUP_AP_ENABLED_BIT);
    while(1) {
    	xEventGroupWaitBits(wifi_event_group, WIFI_EVENT_GROUP_AP_ENABLED_BIT, false, true, portMAX_DELAY);
    	// start wifi ap
    	xEventGroupClearBits(wifi_event_group, WIFI_EVENT_GROUP_AP_ENABLED_BIT);
    	start_wifi_ap();

    	xEventGroupWaitBits(wifi_event_group, WIFI_EVENT_GROUP_AP_DISABLED_BIT, false, true, portMAX_DELAY);
    	// stop wifi ap
    	esp_wifi_stop();
    	// save all parameter into nvs
    	save_para_to_nvs();

    	// ask others to refresh the parameter they are using
    }

}
