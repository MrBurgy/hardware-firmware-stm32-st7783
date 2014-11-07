//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
//#include "diag/Trace.h"
#include "ST7783.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via NONE).
//
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

int
main(int argc, char* argv[])
{
	// TODO: SysClock, GPIO init

	LCD_Begin();
	LCD_FillScreen(BLACK);
	LCD_DrawFastHLine(0, 160, 240, WHITE);

	// Infinite loop
	while (1)
	{
	   // Add your code here.
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
