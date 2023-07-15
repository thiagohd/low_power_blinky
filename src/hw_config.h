#ifndef __HW_CONFIG__
#define __HW_CONFIG__

#define HARDWARE_ARCH               1


#define GPIO_HIGH                   1
#define GPIO_LOW                    0

#define GPIO_XTF_ENABLE_PIN         26
#define GPIO_XTF_ENABLE_PORT        0

#define GPIO_LORA_RST_PIN           24 // Na placa v1.0 ainda está 18!!!
#define GPIO_LORA_RST_PORT          0

#define GPIO_LORA_DIO0_PIN          12
#define GPIO_LORA_DIO0_PORT         0

#define GPIO_LORA_DIO1_PIN          9
#define GPIO_LORA_DIO1_PORT         1

#define GPIO_LORA_DIO2_PIN          8
#define GPIO_LORA_DIO2_PORT         0

#define GPIO_LORA_CS_PIN            13
#define GPIO_LORA_CS_PORT           0

#define GPIO_LORA_MOSI_PIN          20
#define GPIO_LORA_MOSI_PORT         0

#define GPIO_LORA_MISO_PIN          22
#define GPIO_LORA_MISO_PORT         0

#define GPIO_LORA_SCKL_PIN          15
#define GPIO_LORA_SCKL_PORT         0

#if HARDWARE_ARCH == 1



#endif

#endif
