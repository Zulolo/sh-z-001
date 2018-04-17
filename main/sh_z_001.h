/*
 * sh_z_001.h
 *
 *  Created on: Apr 5, 2018
 *      Author: zulolo
 */

#ifndef MAIN_SH_Z_001_H_
#define MAIN_SH_Z_001_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "time.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "os.h"

#define WIFI_EVENT_GROUP_CONNECTED_BIT			BIT0
#define WIFI_EVENT_GROUP_AP_ENABLED_BIT			BIT1
#define WIFI_EVENT_GROUP_AP_DISABLED_BIT		BIT2
#define MAX_STREET_LIGHT_NUM					16
#define STREET_LIGHT_NAME_LEN					32
#define DAYS_PER_WEEK							7

#define ON_OFF_FUNC_ENABLE_DAY_TIME_ON_BIT			BIT0
#define ON_OFF_FUNC_ENABLE_DAY_TIME_OFF_BIT			BIT1
#define ON_OFF_FUNC_ENABLE_POS_CAL_TIME_ON_BIT		BIT2
#define ON_OFF_FUNC_ENABLE_POS_CAL_TIME_OFF_BIT		BIT3
#define ON_OFF_FUNC_ENABLE_BRIGHT_CTL_ON_BIT		BIT4
#define ON_OFF_FUNC_ENABLE_BRIGHT_CTL_OFF_BIT		BIT5

#define PARA_NVX_PREFIX_DAY_TIME_ON				"TIME_ON"
#define PARA_NVX_PREFIX_DAY_TIME_OFF			"TIME_OFF"
#define PARA_NVX_PREFIX_POS_CAL_TIME_ON			"POS_ON"
#define PARA_NVX_PREFIX_POS_CAL_TIME_OFF		"POS_OFF"

typedef struct {
	float longitude;
	float latitude;
}global_position_t;

typedef struct {
	wifi_config_t wifi_config;
	global_position_t global_position;
	struct tm on_off_pos_cal_time[MAX_STREET_LIGHT_NUM];
	struct tm on_off_set_time[MAX_STREET_LIGHT_NUM][DAYS_PER_WEEK];
	uint16_t on_off_bright[MAX_STREET_LIGHT_NUM];
	uint32_t on_off_enable_bits[MAX_STREET_LIGHT_NUM];
	char street_light_name[STREET_LIGHT_NAME_LEN][MAX_STREET_LIGHT_NUM];
} sh_z_001_para_t;

extern EventGroupHandle_t wifi_event_group;
extern int get_light_para_nvs_key_by_index(char *key_name, const char *key_name_prefix, uint8_t light_index);
#endif /* MAIN_SH_Z_001_H_ */
