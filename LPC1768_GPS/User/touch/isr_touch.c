#include "touch_panel.h"
#include "LPC17xx.h"

void EINT3_IRQHandler(void)
{
    extern void touch_show(void);
    GPIO_ClearInt (0,(1<<19));/* clear pending interrupt on P0.19*/
    touch_debug();
    //LPC_GPIOINT->IO0IntClr |= (1 << 19);     /* clear pending interrupt on P0.19*/
}
