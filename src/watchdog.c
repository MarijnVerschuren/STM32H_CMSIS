//
// Created by marijn on 4/26/23.
//

#include "watchdog.h"


/*!< init / enable */
void config_watchdog(uint8_t prescaler, uint16_t reload) {
	IWDG->KR = 0x5555;  // enable access to PR and RLR
	while (IWDG->SR & IWDG_SR_PVU);  // wait until the Prescaler Value Update (PVU) flag is reset
	IWDG->PR = prescaler & 0x7ul;  // clk / (4 << prescaler)
	while (IWDG->SR & IWDG_SR_RVU);  // wait until the Reload Value Update (RVU) flag is reset
	IWDG->RLR = reload & 0xffful;
}
void config_window_watchdog(uint8_t prescaler, uint16_t reload) {}


/*!< actions */
void start_watchdog() { IWDG->KR = 0xccccul; }
void start_window_watchdog() {}
void stop_window_watchdog() {}
void reset_watchdog() { IWDG->KR = 0xaaaaul; }
void reset_window_watchdog() {}