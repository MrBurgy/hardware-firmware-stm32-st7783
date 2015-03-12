//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
//#include "diag/Trace.h"
#include "ST7783.h"
#include "stm32f4xx_hal.h"
#include "Touch.h"

/*################################ ADC1 ######################################*/
/**
  * @brief  ADC Interface pins
  *         used to detect motion of Joystick available on Adafruit 1.8" TFT shield
  */
#define NUCLEO_ADCx                                 ADC1
#define NUCLEO_ADCx_CLK_ENABLE()                  __ADC1_CLK_ENABLE()

#define NUCLEO_ADCx_CHANNEL                       ADC_CHANNEL_4
   
#define NUCLEO_ADCx_GPIO_PORT                       GPIOA
#define NUCLEO_ADCx_GPIO_PIN                        GPIO_PIN_4
#define NUCLEO_ADCx_GPIO_CLK_ENABLE()             __GPIOA_CLK_ENABLE()
#define NUCLEO_ADCx_GPIO_CLK_DISABLE()            __GPIOA_CLK_DISABLE()

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

static ADC_HandleTypeDef hnucleo_Adc;
static ADC_ChannelConfTypeDef sConfig;

static void MX_GPIO_Init(void);

static void ADCx_Init(void);
static void ADCx_MspInit(ADC_HandleTypeDef *hadc);
static uint8_t BSP_JOY_Init(void);
static uint16_t BSP_JOY_GetState(void);

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}


void setPin2Output(GPIO_TypeDef  *GPIOx, uint32_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void setPin2Input(GPIO_TypeDef  *GPIOx, uint32_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void setPin2Analog(GPIO_TypeDef  *GPIOx, uint32_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin ;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

int
main(int argc, char* argv[])
{
	MX_GPIO_Init();

	LCD_Begin();
	LCD_FillScreen(BLACK);
	LCD_DrawFastHLine(0, 160, 240, WHITE);
	LCD_DrawCircle(120, 160, 100, WHITE);
	LCD_DrawRect(10, 10, 300, 220, WHITE);
	LCD_SetCursor(0, 0);
	LCD_Printf("Display Test");

	BSP_JOY_Init();

	//Touch_Init();

	// Infinite loop
	while (1)
	{
		//uint16_t x = Touch_GetX();
		uint16_t y = Touch_GetY();

		//Touch_Getraw(&x, &y);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(1000);
	}
}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA3   ------> USART2_RX
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
 // __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA2 PA5 PA8
                           PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10 | GPIO_PIN_1|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
//  GPIO_InitStruct.Pin = GPIO_PIN_3;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
//  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB10 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  Initializes ADC MSP. (MSP MCU Specific Package)
  * @param  None
  * @retval None
  */
static void ADCx_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  NUCLEO_ADCx_GPIO_CLK_ENABLE();
  
  /* Configure the selected ADC Channel as analog input */
  GPIO_InitStruct.Pin = NUCLEO_ADCx_GPIO_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NUCLEO_ADCx_GPIO_PORT, &GPIO_InitStruct);
  
  /*** Configure the ADC peripheral ***/ 
  /* Enable ADC clock */
  NUCLEO_ADCx_CLK_ENABLE(); 
}

/**
  * @brief  Initializes ADC HAL.
  * @param  None
  * @retval None
  */
static void ADCx_Init(void)
{
  if(HAL_ADC_GetState(&hnucleo_Adc) == HAL_ADC_STATE_RESET)
  {
    /* ADC Config */
    hnucleo_Adc.Instance                   = NUCLEO_ADCx;
    hnucleo_Adc.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4; /* (must not exceed 36MHz) */
    hnucleo_Adc.Init.Resolution            = ADC_RESOLUTION12b;
    hnucleo_Adc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hnucleo_Adc.Init.ContinuousConvMode    = DISABLE;
    hnucleo_Adc.Init.DiscontinuousConvMode = DISABLE;
    hnucleo_Adc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hnucleo_Adc.Init.EOCSelection          = EOC_SINGLE_CONV;
    hnucleo_Adc.Init.NbrOfConversion       = 1;
    hnucleo_Adc.Init.DMAContinuousRequests = DISABLE;    
    
    ADCx_MspInit(&hnucleo_Adc);
    HAL_ADC_Init(&hnucleo_Adc);    
  }
}

/**
  * @brief  Configures joystick available on adafruit 1.8" TFT shield 
  *         managed through ADC to detect motion.
  * @param  None
  * @retval Joystickstatus (0=> success, 1=> fail) 
  */
uint8_t BSP_JOY_Init(void)
{
  uint8_t status = 1;
   
  ADCx_Init();
   
  /* Select the ADC Channel to be converted */
  sConfig.Channel = NUCLEO_ADCx_CHANNEL;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Rank = 1;
  status = HAL_ADC_ConfigChannel(&hnucleo_Adc, &sConfig);
  
  /* Return Joystick initialization status */
  return status;
}

/**
  * @brief  Returns the Joystick key pressed.
  * @note   To know which Joystick key is pressed we need to detect the voltage
  *         level on each key output
  *           - None  : 3.3 V / 4095
  *           - SEL   : 1.055 V / 1308
  *           - DOWN  : 0.71 V / 88
  *           - LEFT  : 3.0 V / 3720 
  *           - RIGHT : 0.595 V / 737
  *           - UP    : 1.65 V / 2046
  */
uint16_t BSP_JOY_GetState(void)
{  
  uint16_t  keyconvertedvalue = 0;
  
  /* Start the conversion process */
  HAL_ADC_Start(&hnucleo_Adc);
  
  /* Wait for the end of conversion */
  HAL_ADC_PollForConversion(&hnucleo_Adc, 10);
  
  /* Check if the continous conversion of regular channel is finished */
  if(HAL_ADC_GetState(&hnucleo_Adc) == HAL_ADC_STATE_EOC_REG)
  {
    /* Get the converted value of regular channel */
    keyconvertedvalue = HAL_ADC_GetValue(&hnucleo_Adc);
  }
  
  return keyconvertedvalue;
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
