/*
 * main.c
 *
 * STM7783 Display with touch demo 
 * 
 * Grafic LCD (Chip=ST7783)
 *
 * Created: Mar 15, 2015
 */

#include <stdio.h>
//#include "diag/Trace.h"
#include "st7783.h"
#include "stm32f4xx_hal.h"
#include "touch.h"


// ----------------------------------------------------------------------------
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

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

/**
 * \brief main function
 * 
 * \param argc
 * \param argv
 * 
 * \return int
 */
int main(int argc, char* argv[])
{
	LCD_Begin();
	LCD_SetRotation(0);
	LCD_FillScreen(BLACK);
	LCD_DrawFastHLine(0, 160, 240, WHITE);
	LCD_DrawCircle(120, 160, 100, WHITE);
	LCD_DrawRect(20, 40, 200, 240, WHITE);

	Touch_Begin();

	LCD_SetTextColor(WHITE, BLACK);
	LCD_SetTextSize(3);

	// Infinite loop
	while (1)
	{
		uint16_t x = Touch_GetX();
		uint16_t y = Touch_GetY();

		char buf[20] = { ' ' };
		sprintf(buf, "%d, %d      ", x, y);
		buf[10] = 0;

		LCD_SetCursor(0, 0);
		LCD_Printf(buf);

		HAL_Delay(500);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
