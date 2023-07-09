# TODO
>### Very High Priority
>* VOS is not applied!! (fixed by setting bypass. ok?)
>* Create basic functionality (GPIO, EXTI, TIM, SYS_CFG)

>### High Priority
>* Clean up code (make 0xA0UL and 0b10UL consistent!!)
>* clock security on HSE and LSE (+interrupt!!)
>* HRTIM
>* LPTIM

>### Medium Priority
>* Redo sys_clock_init function to allow re-configuration (VOS downscaling etc...)
>* Move MCO config from sys.x to mco.x
>* RTC

>### Low Priority
>* timer BKIN config
>* Low power

>### Features
>* RCC backup domain (RCC->BDCR)
>* MCO (micro-controller clock output) (RCC->CFGR)
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