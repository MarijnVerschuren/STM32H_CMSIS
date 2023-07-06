# TODO
>### Very High Priority
>* VOS is not applied!! (fixed by setting bypass. ok?)
>* Create basic functionality (GPIO, EXTI, TIM, SYS_CFG)

>### High Priority
>* clock security on HSE and LSE (+interrupt!!)
>* find out how systick works (found nothing in RCC)

>### Medium Priority
>* RTC
>* Redo sys_clock_init function to allow re-configuration (VOS downscaling etc...)

>### Low Priority
>* Low power

>### Features
>* RCC backup domain (RCC->BDCR)
>* MCO (micro-controller clock output) (RCC->CFGR)
>* STOP mode config (RCC->CFGR:STOPWUCK)
>* clock trimming (RCC->ICSCR, RCC->HSICFGR, RCC->CRRCR, RCC->CSICFGR)

>### Research
>* oscillator drive?
>* (clock) Domains
>* kernel and "per" clocks

>### Test
>* test if LSE and RTC enable need to be split