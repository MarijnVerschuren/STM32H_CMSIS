//
// Created by marijn on 7/15/23.
//

#include "watchdog.h"


/*!<
 * init
 * */
void config_watchdog(uint8_t prescaler, uint16_t reload) {
	IWDG1->KR = 0x5555UL;				// enable access to PR and RLR
	while (IWDG1->SR & IWDG_SR_PVU);	// wait until the Prescaler Value Update (PVU) flag is reset
	IWDG1->PR = prescaler & 0x7UL;		// clk / (4 << prescaler)  (max = 256)
	while (IWDG1->SR & IWDG_SR_RVU);	// wait until the Reload Value Update (RVU) flag is reset
	IWDG1->RLR = reload & 0xFFFUL;
}


/*!<
 * actions
 * */
void start_watchdog() { IWDG1->KR = 0xCCCCUL; }
void reset_watchdog() { IWDG1->KR = 0xAAAAUL; }
