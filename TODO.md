# TODO
>### Very High Priority

>### High Priority
>* I2C slave mode
>* UART idle, addressing, timeouts, MSB-first, stop-bit-config
>* Clean up code (make 0xA0UL and 0b10UL consistent!!)
>* clock security on HSE and LSE (+interrupt!!)
>* HRTIM
>* LPTIM

>### Medium Priority
>* Make library components toggleable using defines
>* Redo sys_clock_init function to allow re-configuration (VOS downscaling etc...)
>* RTC
>* USB composite device!!

>### Low Priority
>* timer BKIN config
>* Low power
>* USB host

>### Features
>* RCC backup domain (RCC->BDCR)
>* STOP mode config (RCC->CFGR:STOPWUCK)
>* clock trimming (RCC->ICSCR, RCC->HSICFGR, RCC->CRRCR, RCC->CSICFGR)

>### Research
>* EXTI > 15?
>* BKIN
>* consider moving LPTIM_GPIO into their own enum
>* oscillator drive?
>* (clock) Domains
>* kernel and "per" clocks

>### Test
>* test 'encoder.h' !!!!!!!!!!!!!!!!!!!!!!!!!
>* test if LSE and RTC enable need to be split

>### TODO
>* OTG      (usb.h)
>* RNG
>* CRYP
>* HASH
>* ADC
>* CAN
>* DFSDM
>* FMC
>* HDMI
>* SAI
>* SD_MMC
>* SPI
>* SWPMI


