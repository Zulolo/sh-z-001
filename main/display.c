/*
 * display.c
 *
 *  Created on: Apr 22, 2018
 *      Author: zulolo
 */

#include "sh_z_001.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#define DISPLAY_PIN_NUM_MISO 		12
#define DISPLAY_PIN_NUM_MOSI 		13
#define DISPLAY_PIN_NUM_CLK  		14
#define DISPLAY_PIN_NUM_CS   		15
#define DISPLAY_PIN_NUM_RST   		32
#define DISPLAY_PIN_NUM_BCKL   		33

static spi_device_handle_t spi;
static const char *TAG_DISPLAY = "[DISPLAY]";
/*
 The ILI9341 needs a bunch of command/argument values to be initialized. They are stored in this struct.
 */
typedef struct {
	uint8_t cmd;
	uint8_t data[16];
	uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} ili_init_cmd_t;

static spi_bus_config_t buscfg = { .miso_io_num = DISPLAY_PIN_NUM_MISO, .mosi_io_num = DISPLAY_PIN_NUM_MOSI,
		.sclk_io_num = DISPLAY_PIN_NUM_CLK, .quadwp_io_num = -1, .quadhd_io_num = -1 };

static void display_pre_transfer_callback(spi_transaction_t *t) {
	// do something?
}

static spi_device_interface_config_t devcfg = { .clock_speed_hz = 1000000, .mode = 0,  //SPI mode 0
		.spics_io_num = DISPLAY_PIN_NUM_CS,               //CS pin
		.queue_size = 7,                      //We want to be able to queue 7 transactions at a time
		.pre_cb = display_pre_transfer_callback, //Specify pre-transfer callback to handle D/C line
		};

static void send_cmd(spi_device_handle_t spi, const uint8_t cmd) {
	spi_transaction_t t;
	static uint8_t sendbyte[3];
	ESP_LOGI(TAG_DISPLAY, "Send cmd 0x%02X.", cmd);
	memset(&t, 0, sizeof(t));
	sendbyte[0] = 0xf8;
	sendbyte[1] = cmd & 0xF0;
	sendbyte[2] = (cmd << 4) & 0xF0;

	t.length = sizeof(sendbyte) * 8;
	t.tx_buffer = sendbyte;
	ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}

static void send_data(spi_device_handle_t spi, const uint8_t data) {
	spi_transaction_t t;
	static uint8_t sendbyte[3];
	ESP_LOGI(TAG_DISPLAY, "Send data 0x%02X.", data);
	memset(&t, 0, sizeof(t));
	sendbyte[0] = 0xFA;
	sendbyte[1] = data & 0xF0;
	sendbyte[2] = (data << 4) & 0xF0;

	t.length = sizeof(sendbyte) * 8;
	t.tx_buffer = sendbyte;
	ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}

void display_putstr(spi_device_handle_t spi, const char *str) {
    uint8_t *s = (uint8_t*)str;
    while(*s > 0) {
    	send_data(spi, *s);
        s++;
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

void display_clear(spi_device_handle_t spi) {
	send_cmd(spi, 0x01);
	vTaskDelay(50 / portTICK_RATE_MS);
}

void display_setpos(spi_device_handle_t spi, int y, int x) {
    switch(y) {
        case 0:
        	send_cmd(spi, 0x80 + x);
            return;
        case 1:
        	send_cmd(spi, 0x90 + x);
            return;
        case 2:
        	send_cmd(spi, 0x88 + x);
            return;
        case 3:
        	send_cmd(spi, 0x98 + x);
            return;
    }
}

static void diaplay_spi_init(spi_device_handle_t spi) {

	//Initialize non-SPI GPIOs
	gpio_set_direction(DISPLAY_PIN_NUM_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(DISPLAY_PIN_NUM_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(DISPLAY_PIN_NUM_RST, 1);
	vTaskDelay(1000 / portTICK_RATE_MS);

	//Send all the commands
	send_cmd(spi, 0x30);
	vTaskDelay(100 / portTICK_RATE_MS);
	send_cmd(spi, 0x0C);
	vTaskDelay(100 / portTICK_RATE_MS);
	display_clear(spi);
	display_setpos(spi, 0, 0);
	display_putstr(spi, "Hello world!");
	///Enable backlight
	gpio_set_level(DISPLAY_PIN_NUM_BCKL, 1);
}

void display_task(void *pvParameters) {
	spi_dev_t* spi_hw;
	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));

	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi));

	spi_hw = spicommon_hw_for_host(HSPI_HOST);
	spi_hw->pin.master_cs_pol = 1;

	diaplay_spi_init(spi);

	while (1) {

		vTaskDelay(2000 / portTICK_RATE_MS);
	}

}
