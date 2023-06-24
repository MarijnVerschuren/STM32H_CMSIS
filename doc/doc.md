# STM32H

>## Contents
>* Clocks
>* Devices

## Clocks
> | Name | Definition         | BASE + Offset  | Max Speed | Redefinition |
> |------|--------------------|----------------|-----------|--------------|
> | APB1 | D2_APB1PERIPH_BASE | + 0x00000000UL | 120 MHz   | APB1_BASE    |
> | AHB1 | D2_AHB1PERIPH_BASE | + 0x00020000UL | 200 MHz   | AHB1_BASE    |
> | APB2 | D2_APB2PERIPH_BASE | + 0x00010000UL | 120 MHz   | APB2_BASE    |
> | AHB2 | D2_AHB2PERIPH_BASE | + 0x08020000UL | 200 MHz   | AHB2_BASE    |
> | APB3 | D1_APB1PERIPH_BASE | + 0x10000000UL | ?         | APB3_BASE    |
> | AHB3 | D1_AHB1PERIPH_BASE | + 0x12000000UL | 200 MHz   | AHB3_BASE    |
> | APB4 | D3_APB1PERIPH_BASE | + 0x18000000UL | 100 MHz   | APB4_BASE    |
> | AHB4 | D3_AHB1PERIPH_BASE | + 0x18020000UL | 200 MHz   | AHB4_BASE    |
> 
> APB3 is very hard to find in the chip diagram (it includes LCD-TFT) and the max speed is not shown.

## Devices
> | Name   | Base Definition         | CLOCK + Offset | Clock |
> |--------|-------------------------|----------------|-------|
> | OTG_HS | USB1_OTG_HS_PERIPH_BASE | + 0x00020000UL | AHB1  |
> | OTG_FS | USB2_OTG_FS_PERIPH_BASE | + 0x00060000UL | AHB1  |
>