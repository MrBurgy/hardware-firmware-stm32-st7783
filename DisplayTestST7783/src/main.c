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


extern uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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
	for (int x = 40; x < 320; x+=40) {
		LCD_DrawFastHLine(0, x, 240, WHITE);
	}

	for (int y = 40; y < 240; y+=40) {
		LCD_DrawFastVLine(y, 40, 280, WHITE);
	}


	Touch_Begin();

	LCD_SetTextColor(WHITE, BLACK);
	LCD_SetTextSize(3);

	// Infinite loop
	while (1)
	{
		uint16_t x = Touch_GetX();
		uint16_t y = Touch_GetY();

		if(x > 100 && y > 100) {
			uint16_t xAvg = 0;
			uint16_t yAvg = 0;
			for(uint8_t n = 0; n < 10; n++) {
				xAvg += Touch_GetX();
				yAvg += Touch_GetY();
			}
			xAvg /= 10;
			yAvg /= 10;

			y = map(yAvg, 220, 1200, 0, 240);
			x = map(xAvg, 2300, 580, 0, 320);

			char buf[20] = { ' ' };
			sprintf(buf, "%d, %d      ", x, y);
			buf[10] = 0;

			LCD_SetCursor(0, 0);
			LCD_Printf(buf);

			LCD_FillRect(y, x, 3, 3, YELLOW);
		}



		//HAL_Delay(500);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
