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
#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
LOG_MODULE_REGISTER(blinky_low_power);
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   2000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

const struct device *xdev_uart;
const struct device *xdev_uart1;

void XTF_TURN_ON(){
	pm_device_action_run(xdev_uart, PM_DEVICE_ACTION_RESUME);
	pm_device_action_run(xdev_uart1, PM_DEVICE_ACTION_RESUME);
	LOG_INF("UART START");
	
} 	

void XTF_TURN_OFF(){
	LOG_INF("UART STOP");
	pm_device_action_run(xdev_uart, PM_DEVICE_ACTION_SUSPEND);
	pm_device_action_run(xdev_uart1, PM_DEVICE_ACTION_SUSPEND);
}


void main(void)
{
	int ret;
	const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	const struct device *gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));
	xdev_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
	xdev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));
	__ASSERT(xdev_uart, "Falhou ao pegar ponteiro para a UART1");
	if (!device_is_ready(gpio0) || !device_is_ready(gpio1)){
		//return;
	}
	if (!device_is_ready(led0.port) || !device_is_ready(led1.port) ||
		!device_is_ready(led2.port) || !device_is_ready(led3.port)) {
		return;
	}
	//pm_device_init_suspended(gpio0);
	//pm_device_runtime_enable(gpio0);
	while (1) {		
		//pm_device_runtime_put_async(gpio0, PM_DEVICE_ACTION_RESUME);
		
		XTF_TURN_ON();
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
		XTF_TURN_OFF();
		
		LOG_INF("UART TRASH1");	
		LOG_INF("UART TRASH2");	
		LOG_INF("UART TRASH3");	
		k_msleep(SLEEP_TIME_MS);
		LOG_INF("UART TRASH4");	
	}
}
