#ifndef __POWER_MANAGEMENT_H__
#define __POWER_MANAGEMENT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/drivers/lora.h>
#include <errno.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <helpers/nrfx_reset_reason.h>
#include <nrfx_power.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/lora.h>

#include <hal/nrf_gpio.h>



typedef struct pin_power_management{
    uint8_t                 saved;
    char                    name[30];
    nrf_gpio_pin_sense_t    sensecfg;
    nrf_gpio_pin_dir_t      dircfg;
    nrf_gpio_pin_input_t    inputcfg;
    nrf_gpio_pin_pull_t     pullcfg;
    nrf_gpio_pin_drive_t    drivecfg;
    uint32_t                outcfg;
} ppm_t;

void enable_gpio_pin(const struct device *dev, int port, int pin);
void disable_gpio_pin(const struct device *dev, int port, int pin);
void save_pin_config(const struct device *dev, int port, int pin, char *name);
void XTF_TURN_OFF(const struct device *dev_uart, const struct device *dev_spi, const struct device *dev_gpio0, const struct device *dev_gpio1);
void XTF_TURN_ON(const struct device *dev_uart, const struct device *dev_spi, const struct device *dev_gpio0, const struct device *dev_gpio1);

void save_all_pins();
void enable_all_pins();
void disable_all_pins();



#endif