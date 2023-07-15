/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <nrfx_power.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/logging/log.h>
#include "power_management.h"


#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
LOG_MODULE_REGISTER(blinky_low_power);
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   2000


const struct device *gpio0;
const struct device *gpio1;
const struct device *xdev_spi;
const struct device *xdev_uart1;
const struct device *lora_dev = DEVICE_DT_GET(DT_NODELABEL(lora));

// void XTF_TURN_ON(){
// 	pm_device_action_run(xdev_spi, PM_DEVICE_ACTION_RESUME);
// 	pm_device_action_run(xdev_uart, PM_DEVICE_ACTION_RESUME);
// 	pm_device_action_run(xdev_uart1, PM_DEVICE_ACTION_RESUME);
// 	LOG_INF("UART START");
	
// } 	

// void XTF_TURN_OFF(){
// 	LOG_INF("UART STOP");
// 	pm_device_action_run(xdev_spi, PM_DEVICE_ACTION_SUSPEND);
// 	pm_device_action_run(xdev_uart, PM_DEVICE_ACTION_SUSPEND);
// 	pm_device_action_run(xdev_uart1, PM_DEVICE_ACTION_SUSPEND);
// }
static inline int wrapper_lora_recv(const struct device *dev, uint8_t *data, uint8_t size, k_timeout_t timeout, int16_t *rssi, int8_t *snr)
{
#if 0 && SIMULATE_LORA
	return 0;
#else
	return lora_recv(dev, data, size, timeout, rssi, snr);
#endif
}

static inline int wrapper_lora_send(const struct device *dev, uint8_t *data, uint32_t data_len)
{
#if SIMULATE_LORA
	return 0;
#else
	return lora_send(dev, data, data_len);
#endif
}
int wrapper_config_lora_tx(const struct device *lora_device, struct lora_modem_config *config)
{
	int ret;
	if (lora_device == NULL || config == NULL)
	{
		LOG_ERR("lora_dev = %08X; config = %08X", (uint32_t)lora_device, (uint32_t)config);
		return -1;
	}
#if SIMULATE_LORA
	ret = 0;
#else
	ret = lora_config(lora_device, config);
#endif
	return ret;
}

struct lora_modem_config config;
int config_lora_tx(bool set_tx)
{
	if (lora_dev == NULL)
	{
		LOG_ERR("Erro fatal no LoRa!");
		return -1;
	}
	config.frequency = 915000000;
	config.bandwidth = BW_125_KHZ;
	config.datarate = SF_7;
	config.preamble_len = 8;
	config.coding_rate = CR_4_5;
	config.tx_power = 20;
	config.tx = set_tx;
	int ret = wrapper_config_lora_tx(lora_dev, &config);
	k_sleep(K_MSEC(1));
	return ret;
}


void main(void)
{
	int ret, rssi, snr;
	uint8_t buff[10];
	const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	const struct device *gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));
	xdev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));
	xdev_spi = DEVICE_DT_GET(DT_NODELABEL(spi1));
	__ASSERT(xdev_uart1, "Falhou ao pegar ponteiro para a UART1");
	__ASSERT(xdev_spi, "Falhou ao pegar ponteiro para a SPI");

	
	while (!device_is_ready(xdev_uart1) || !device_is_ready(xdev_spi) ||!device_is_ready(gpio0) || !device_is_ready(gpio1)){
		LOG_ERR("Dispositivos nao estao prontos");
		k_msleep(100);
	}
	if (NRF_UICR->REGOUT0 != UICR_REGOUT0_VOUT_3V0)
	{
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
		{
		}
		NRF_UICR->REGOUT0 = UICR_REGOUT0_VOUT_3V0;
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
		{
		}
	}
	nrf_power_dcdcen_set(NRF_POWER, false);
	nrf_power_dcdcen_vddh_set(NRF_POWER, false);
	k_msleep(5000);
	save_all_pins();
	//pm_device_init_suspended(gpio0);
	//pm_device_runtime_enable(gpio0);
	while (1) {		
		//pm_device_runtime_put_async(gpio0, PM_DEVICE_ACTION_RESUME);
		
		XTF_TURN_OFF(xdev_uart1, xdev_spi, gpio0, gpio1);
		// wrapper_lora_send(xdev_spi, "Teste", 5);
		// config_lora_tx(false);
		// wrapper_lora_recv(xdev_spi, buff, 5, K_MSEC(5000), &rssi, &snr);
		//pm_device_action_run(gpio0, PM_DEVICE_ACTION_RESUME);
		//pm_device_runtime_get(gpio0);
		k_msleep(100);
		//pinctrl_apply_state(gpio0->config, PINCTRL_STATE_DEFAULT);
		//pinctrl_apply_state(gpio1->config, PINCTRL_STATE_DEFAULT);
		k_msleep(100);
		// gpio_pin_set_dt(&led0, 1);
		// gpio_pin_set_dt(&led1, 1);
		// gpio_pin_set_dt(&led2, 1);
		// gpio_pin_set_dt(&led3, 1);
		k_msleep(SLEEP_TIME_MS);
		k_msleep(100);
		// pinctrl_apply_state(gpio0->config, PINCTRL_STATE_SLEEP);
		// pinctrl_apply_state(gpio1->config, PINCTRL_STATE_SLEEP);
		k_msleep(100);
		// gpio_pin_set_dt(&led0, 0);
		// gpio_pin_set_dt(&led1, 0);
		// gpio_pin_set_dt(&led2, 0);
		// gpio_pin_set_dt(&led3, 0);
		//pm_device_runtime_put_async(gpio0, PM_DEVICE_ACTION_SUSPEND);
		// pm_device_action_run(gpio0, PM_DEVICE_ACTION_SUSPEND);
		// pm_device_action_run(gpio1, PM_DEVICE_ACTION_SUSPEND);
		//pm_device_runtime_put(gpio0);
		
		LOG_INF("UART TRASH1");	
		LOG_INF("UART TRASH2");	
		LOG_INF("UART TRASH3");	
		XTF_TURN_ON(xdev_uart1, xdev_spi, gpio0, gpio1);
		config_lora_tx(true);
		k_msleep(SLEEP_TIME_MS);
		LOG_INF("UART TRASH4");	
	}
}
