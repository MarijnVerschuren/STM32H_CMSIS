# STM32F_CMSIS
CMSIS HAL library for the STM32H family of micro controllers (ARM Cortex-M7)

>## Features
>* GPIO

# GPIO
```C
#include "gpio.h"

int main(void) {
    fconfig_GPIO(PORT, PIN, MODE, PULLUP, OUTPUT_TYPE, SPEED, ALT_FUNC);
    config_GPIO(PORT, PIN, MODE, PULLUP, OUTPUT_TYPE);
    // MODE:        GPIO_input, GPIO_output, GPIO_alt_func, GPIO_analog
    // PULLUP:      GPIO_no_pull, GPIO_pull_up, GPIO_pull_down
    // OUTPUT_TYPE: GPIO_push_pull, GPIO_open_drain
    // SPEED:       GPIO_low_speed (default), GPIO_medium_speed, GPIO_high_speed, GPIO_very_high_speed,
    // ALT_FUNC:    uint8_t [0, 15] (only applies when using GPIO_alt_func)
    GPIO_write(PORT, PIN, DATA);
    // DATA:        uint8_t [0, 1]
    GPIO_toggle(PORT, PIN);
    uint8_t data = GPIO_read(PORT, PIN);
    
    disable_GPIO(PORT);
}
```