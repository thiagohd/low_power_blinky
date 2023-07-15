#ifndef __DBG_CONFIG_H__
#define __DBG_CONFIG_H__


#if 1

    #include <zephyr/logging/log.h>
    #include <zephyr/sys/printk.h>
    #include <zephyr/kernel.h>
#if 1
    #define dbgI(...) LOG_INF(__VA_ARGS__); \
                        k_msleep(1)
    #define dbgE(...) LOG_ERR(__VA_ARGS__); \
                        k_msleep(1)
#else
    #define dbgI(...) printk(__VA_ARGS__); \
                        printk("\n"); \
                        k_msleep(2)
    #define dbgE(...) printk(__VA_ARGS__); \
                        printk("\n"); \
                        k_msleep(2)
#endif

    #define get_time k_uptime_get

    static inline uint32_t get_time_u32(){
        return (uint32_t) k_uptime_get();
    }

#else

    #include <time.h>
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"

    #define dbgI(...) ESP_LOGI(TAG, __VA_ARGS__)
    #define dbgE(...) ESP_LOGE(TAG, __VA_ARGS__)

    #define log_strdup(...) (__VA_ARGS__)

    #define get_time() clock()/CLOCKS_PER_SEC

#endif

#endif
