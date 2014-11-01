#include <stdio.h>
#include "BlinkLed.h"

#include "stm32f4xx_hal.h"

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

int main(int argc, char* argv[])
{

  blink_led_init();

  // Infinite loop
  while (1)
    {
      blink_led_on();
      HAL_Delay(500);
      blink_led_off();
      HAL_Delay(500);
    }
  // Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
