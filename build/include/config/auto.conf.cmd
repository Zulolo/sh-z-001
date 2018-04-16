deps_config := \
	/home/rorschach/esp-idf/components/app_trace/Kconfig \
	/home/rorschach/esp-idf/components/aws_iot/Kconfig \
	/home/rorschach/esp-idf/components/bt/Kconfig \
	/home/rorschach/esp-idf/components/esp32/Kconfig \
	/home/rorschach/esp-idf/components/esp_adc_cal/Kconfig \
	/home/rorschach/esp-idf/components/ethernet/Kconfig \
	/home/rorschach/esp-idf/components/fatfs/Kconfig \
	/home/rorschach/esp-idf/components/freertos/Kconfig \
	/home/rorschach/esp-idf/components/heap/Kconfig \
	/home/rorschach/esp/sh-z-001/components/libesphttpd/Kconfig \
	/home/rorschach/esp-idf/components/libsodium/Kconfig \
	/home/rorschach/esp-idf/components/log/Kconfig \
	/home/rorschach/esp-idf/components/lwip/Kconfig \
	/home/rorschach/esp-idf/components/mbedtls/Kconfig \
	/home/rorschach/esp-idf/components/openssl/Kconfig \
	/home/rorschach/esp-idf/components/pthread/Kconfig \
	/home/rorschach/esp-idf/components/spi_flash/Kconfig \
	/home/rorschach/esp-idf/components/spiffs/Kconfig \
	/home/rorschach/esp-idf/components/tcpip_adapter/Kconfig \
	/home/rorschach/esp-idf/components/wear_levelling/Kconfig \
	/home/rorschach/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/rorschach/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/rorschach/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/rorschach/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
