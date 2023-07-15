#include <zephyr/device.h>
#include <nrfx_power.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/logging/log.h>
#include "power_management.h"
#include "hw_config.h"

extern const struct device *gpio0;
extern const struct device *gpio1;
extern const struct device *xdev_spi;
extern const struct device *xdev_uart1;

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "dbg_config.h"
LOG_MODULE_REGISTER(power_management);

ppm_t __global_ppm[2][32] = {0};

static void confirm_pin_config(const struct device *dev, int port, int pin){
	if(__global_ppm[port][pin].drivecfg 	!= 	GPIO_PIN_CNF_DRIVE_S0S1){LOG_ERR("FALHA 5");}
	if(__global_ppm[port][pin].dircfg 		!= 	nrf_gpio_pin_dir_get(NRF_GPIO_PIN_MAP(port, pin))){LOG_ERR("FALHA 0");}
	if(__global_ppm[port][pin].inputcfg 	!= 	nrf_gpio_pin_input_get(NRF_GPIO_PIN_MAP(port, pin))){LOG_ERR("FALHA 1");}
	if(__global_ppm[port][pin].outcfg 		!= 	nrf_gpio_pin_out_read(NRF_GPIO_PIN_MAP(port, pin))){LOG_ERR("FALHA 2");}
	if(__global_ppm[port][pin].pullcfg 		!=	nrf_gpio_pin_pull_get(NRF_GPIO_PIN_MAP(port, pin))){LOG_ERR("FALHA 3");}
	if(__global_ppm[port][pin].sensecfg 	!= 	nrf_gpio_pin_sense_get(NRF_GPIO_PIN_MAP(port, pin))){LOG_ERR("FALHA 4");}
}

void save_pin_config(const struct device *dev, int port, int pin, char *name){
	__global_ppm[port][pin].saved		= 1;
	__global_ppm[port][pin].drivecfg 	= GPIO_PIN_CNF_DRIVE_S0S1;
	__global_ppm[port][pin].dircfg 		= nrf_gpio_pin_dir_get(NRF_GPIO_PIN_MAP(port, pin));
	__global_ppm[port][pin].inputcfg 	= nrf_gpio_pin_input_get(NRF_GPIO_PIN_MAP(port, pin));
	__global_ppm[port][pin].outcfg 		= nrf_gpio_pin_out_read(NRF_GPIO_PIN_MAP(port, pin));
	__global_ppm[port][pin].pullcfg 	= nrf_gpio_pin_pull_get(NRF_GPIO_PIN_MAP(port, pin));
	__global_ppm[port][pin].sensecfg 	= nrf_gpio_pin_sense_get(NRF_GPIO_PIN_MAP(port, pin));
	strncpy(__global_ppm[port][pin].name, name, strlen(name) > 29 ? 29 : strlen(name));
	LOG_INF("%d_%02d drv=%08X dir=%08X in=%08X out=%08X pull=%08X sens=%08X name=%s", 
				port, pin,
				__global_ppm[port][pin].drivecfg,
				__global_ppm[port][pin].dircfg,
				__global_ppm[port][pin].inputcfg,
				__global_ppm[port][pin].outcfg,
				__global_ppm[port][pin].pullcfg,
				__global_ppm[port][pin].sensecfg,
				__global_ppm[port][pin].name);
	
	k_msleep(10);
}

static void load_pin_config(const struct device *dev, int port, int pin){
	if(__global_ppm[port][pin].saved){
		nrf_gpio_cfg(NRF_GPIO_PIN_MAP(port, pin),
					__global_ppm[port][pin].dircfg,
					__global_ppm[port][pin].inputcfg,
					__global_ppm[port][pin].pullcfg,
					__global_ppm[port][pin].drivecfg,
					__global_ppm[port][pin].sensecfg);
		LOG_INF("%d_%02d drv=%08X dir=%08X in=%08X out=%08X pull=%08X sens=%08X name=%s", 
				port, pin,
				__global_ppm[port][pin].drivecfg,
				__global_ppm[port][pin].dircfg,
				__global_ppm[port][pin].inputcfg,
				__global_ppm[port][pin].outcfg,
				__global_ppm[port][pin].pullcfg,
				__global_ppm[port][pin].sensecfg,
				__global_ppm[port][pin].name);					
	}
	k_msleep(10);
}

void disable_gpio_pin(const struct device *dev, int port, int pin){
	nrf_gpio_cfg(NRF_GPIO_PIN_MAP(port, pin),
				NRF_GPIO_PIN_DIR_INPUT,
				NRF_GPIO_PIN_INPUT_DISCONNECT,
				NRF_GPIO_PIN_NOPULL,
				NRF_GPIO_PIN_S0S1,
				NRF_GPIO_PIN_NOSENSE);
}

void enable_gpio_pin(const struct device *dev, int port, int pin){
	load_pin_config(dev, port, pin);
	confirm_pin_config(dev, port, pin);
}


void XTF_TURN_ON(const struct device *dev_uart, const struct device *dev_spi, const struct device *dev_gpio0, const struct device *dev_gpio1){

	if(dev_uart != NULL){
		pm_device_action_run(dev_uart, PM_DEVICE_ACTION_RESUME);
	}
	if(dev_spi != NULL){
		pm_device_action_run(dev_spi, PM_DEVICE_ACTION_RESUME);
	}

	enable_all_pins();
	gpio_pin_set_raw(dev_gpio0, GPIO_XTF_ENABLE_PIN, GPIO_HIGH);

	k_msleep(10);

} 	

void XTF_TURN_OFF(const struct device *dev_uart, const struct device *dev_spi, const struct device *dev_gpio0, const struct device *dev_gpio1){

	if(dev_uart != NULL)
		pm_device_action_run(dev_uart, PM_DEVICE_ACTION_SUSPEND);
	if(dev_spi != NULL)
		pm_device_action_run(dev_spi, PM_DEVICE_ACTION_SUSPEND);

	if(dev_gpio0 != NULL){
		for(int i = 0; i < 32; i++){
			disable_gpio_pin(dev_gpio0, 0, i);
		}
	}
	if(dev_gpio1 != NULL){
		for(int i = 0; i < 16; i++){
			disable_gpio_pin(dev_gpio1, 1, i);
		}
	}
	k_msleep(10);
}


const struct device *lora_get_gpio_device_from_port(int port)
{
	if (port == 0)
		return gpio0;
	else
		return gpio1;
}

void save_all_pins()
{
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_RST_PORT), 	GPIO_LORA_RST_PORT, 	GPIO_LORA_RST_PIN,		"GPIO_LORA_RST_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_DIO0_PORT), 	GPIO_LORA_DIO0_PORT, 	GPIO_LORA_DIO0_PIN,		"GPIO_LORA_DIO0_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_DIO1_PORT), 	GPIO_LORA_DIO1_PORT, 	GPIO_LORA_DIO1_PIN,		"GPIO_LORA_DIO1_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_DIO2_PORT), 	GPIO_LORA_DIO2_PORT, 	GPIO_LORA_DIO2_PIN,		"GPIO_LORA_DIO2_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_CS_PORT), 		GPIO_LORA_CS_PORT, 		GPIO_LORA_CS_PIN,		"GPIO_LORA_CS_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_MOSI_PORT), 	GPIO_LORA_MOSI_PORT, 	GPIO_LORA_MOSI_PIN,		"GPIO_LORA_MOSI_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_MISO_PORT), 	GPIO_LORA_MISO_PORT, 	GPIO_LORA_MISO_PIN,		"GPIO_LORA_MISO_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_LORA_SCKL_PORT), 	GPIO_LORA_SCKL_PORT, 	GPIO_LORA_SCKL_PIN,		"GPIO_LORA_SCKL_PIN");
	save_pin_config(lora_get_gpio_device_from_port(GPIO_XTF_ENABLE_PORT), 	GPIO_XTF_ENABLE_PORT, 	GPIO_XTF_ENABLE_PIN,	"GPIO_XTF_ENABLE_PIN");

}

void enable_all_pins()
{

	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_RST_PORT),		GPIO_LORA_RST_PORT, 	GPIO_LORA_RST_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_DIO0_PORT),	GPIO_LORA_DIO0_PORT, 	GPIO_LORA_DIO0_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_DIO1_PORT),	GPIO_LORA_DIO1_PORT, 	GPIO_LORA_DIO1_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_DIO2_PORT),	GPIO_LORA_DIO2_PORT, 	GPIO_LORA_DIO2_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_CS_PORT), 		GPIO_LORA_CS_PORT, 		GPIO_LORA_CS_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_MOSI_PORT), 	GPIO_LORA_MOSI_PORT, 	GPIO_LORA_MOSI_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_MISO_PORT), 	GPIO_LORA_MISO_PORT, 	GPIO_LORA_MISO_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_LORA_SCKL_PORT), 	GPIO_LORA_SCKL_PORT, 	GPIO_LORA_SCKL_PIN);
	enable_gpio_pin(lora_get_gpio_device_from_port(GPIO_XTF_ENABLE_PORT), 	GPIO_XTF_ENABLE_PORT, 	GPIO_XTF_ENABLE_PIN);

	// nrf_spi_pins_set(NRF_SPI1, GPIO_LORA_SCKL_PIN, GPIO_LORA_MOSI_PIN, GPIO_LORA_MISO_PIN);
}

