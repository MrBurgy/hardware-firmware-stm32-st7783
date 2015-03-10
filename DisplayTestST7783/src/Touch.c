#include "Touch.h"

#define TS_ADCx_CLK_ENABLE()                  __ADC1_CLK_ENABLE()
#define TS_ADCx_GPIO_CLK_ENABLE()             __GPIOA_CLK_ENABLE()
#define TS_ADCx_GPIO_CLK_DISABLE()            __GPIOA_CLK_DISABLE()

#define TS_PIN_XP	GPIO_PIN_10
#define TS_PIN_XM	GPIO_PIN_4
#define TS_PIN_YP	GPIO_PIN_1
#define TS_PIN_YM	GPIO_PIN_4

#define TS_PORT_XP	GPIOB
#define TS_PORT_XM	GPIOA
#define TS_PORT_YP	GPIOA
#define TS_PORT_YM	GPIOB

#define TS_ADCCH_XM	ADC_CHANNEL_4
#define TS_ADCCH_YP	ADC_CHANNEL_1
#define TS_ADC_PORT GPIOA
#define TS_ADC		ADC1


static ADC_HandleTypeDef hnucleo_Adc;
static ADC_ChannelConfTypeDef sConfig;

static void delay(volatile int i)
{
	for(;i;--i);
}

/**
  * @brief  Initializes ADC MSP. (MSP MCU Specific Package)
  * @param  None
  * @retval None
  */
static void ADCx_MspInit(void)
{
  //GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  TS_ADCx_GPIO_CLK_ENABLE();
  
//  /* Configure the selected ADC Channel as analog input */
//  GPIO_InitStruct.Pin = NUCLEO_ADCx_GPIO_PIN ;
//  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(TS_ADC_PORT, &GPIO_InitStruct);
  
  /*** Configure the ADC peripheral ***/ 
  /* Enable ADC clock */
  TS_ADCx_CLK_ENABLE(); 
}

static void ADCx_Init(void)
{
  if(HAL_ADC_GetState(&hnucleo_Adc) == HAL_ADC_STATE_RESET)
  {
    /* ADC Config */
    hnucleo_Adc.Instance                   = TS_ADC;
    hnucleo_Adc.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4; /* (must not exceed 36MHz) */
    hnucleo_Adc.Init.Resolution            = ADC_RESOLUTION12b;
    hnucleo_Adc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hnucleo_Adc.Init.ContinuousConvMode    = DISABLE;
    hnucleo_Adc.Init.DiscontinuousConvMode = DISABLE;
    hnucleo_Adc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hnucleo_Adc.Init.EOCSelection          = EOC_SINGLE_CONV;
    hnucleo_Adc.Init.NbrOfConversion       = 1;
    hnucleo_Adc.Init.DMAContinuousRequests = DISABLE;    
    
    //ADCx_MspInit(&hnucleo_Adc);
    HAL_ADC_Init(&hnucleo_Adc);    
  }
}

static uint8_t ADCx_Channel_Init(uint16_t channel)
{
  uint8_t status = 1;
   
  ADCx_Init();
   
  /* Select the ADC Channel to be converted */
  sConfig.Channel = channel;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Rank = 1;
  status = HAL_ADC_ConfigChannel(&hnucleo_Adc, &sConfig);
  
  /* Return initialization status */
  return status;
}

static void GPIO_SetAnalog(GPIO_TypeDef  *GPIOx, uint16_t pin)
{	
  GPIO_InitTypeDef GPIO_InitStruct;	
	
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


static void GPIO_SetOutput(GPIO_TypeDef  *GPIOx, uint16_t pin, int val)
{  
  GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
  
  if(val) {		
    HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_SET);
	}
}

static void GPIO_SetInputPulledUp(GPIO_TypeDef  *GPIOx, uint16_t pin)
{	  
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;	
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct); 
}

static int GPIO_GetInputState(GPIO_TypeDef  *GPIOx, uint16_t pin)
{
	return 0;//GPIO_ReadInputDataBit(TS_PORT, pin);
}

static uint16_t ADC_Measure(uint16_t ch)
{
	ADCx_Channel_Init(ch);

	uint16_t  adcValue = 0;

	/* Start the conversion process */
	HAL_ADC_Start(&hnucleo_Adc);

	/* Wait for the end of conversion */
	HAL_ADC_PollForConversion(&hnucleo_Adc, 10);

	/* Check if the continous conversion of regular channel is finished */
	if(HAL_ADC_GetState(&hnucleo_Adc) == HAL_ADC_STATE_EOC_REG)	{
		/* Get the converted value of regular channel */
		adcValue = HAL_ADC_GetValue(&hnucleo_Adc);
	}

	return adcValue;
}

void Touch_Init(void)
{
	ADCx_MspInit();
	ADCx_Init();
}

uint16_t Touch_GetX(void)
{
	GPIO_SetInputPulledUp(TS_PORT_YP, TS_PIN_YP);
	GPIO_SetInputPulledUp(TS_PORT_YM, TS_PIN_YM);

	GPIO_SetOutput(TS_PORT_XP, TS_PIN_XP, GPIO_PIN_SET);
	GPIO_SetOutput(TS_PORT_XM, TS_PIN_XM, GPIO_PIN_RESET);

	return ADC_Measure(TS_ADCCH_YP);
}

uint16_t Touch_GetY(void)
{
	GPIO_SetInputPulledUp(TS_PORT_XP, TS_PIN_XP);
	GPIO_SetInputPulledUp(TS_PORT_XM, TS_PIN_XM);

	GPIO_SetOutput(TS_PORT_YP, TS_PIN_YP, GPIO_PIN_SET);
	GPIO_SetOutput(TS_PORT_YM, TS_PIN_YM, GPIO_PIN_RESET);

	return ADC_Measure(TS_ADCCH_XM);
}

void Touch_Getraw(uint16_t *adc_x, uint16_t *adc_y)
{
	//* x ************************************
	GPIO_SetInputPulledUp(TS_PORT_YP, TS_PIN_YP);
	GPIO_SetInputPulledUp(TS_PORT_YM, TS_PIN_YM);

	GPIO_SetOutput(TS_PORT_XP, TS_PIN_XP, GPIO_PIN_SET);
	GPIO_SetOutput(TS_PORT_XM, TS_PIN_XM, GPIO_PIN_RESET);

	*adc_x = ADC_Measure(TS_ADCCH_YP);

	//* y ************************************
	GPIO_SetInputPulledUp(TS_PORT_XP, TS_PIN_XP);
	GPIO_SetInputPulledUp(TS_PORT_XM, TS_PIN_XM);

	GPIO_SetOutput(TS_PORT_YP, TS_PIN_YP, GPIO_PIN_SET);
	GPIO_SetOutput(TS_PORT_YM, TS_PIN_YM, GPIO_PIN_RESET);

	*adc_y = ADC_Measure(TS_ADCCH_XM);



	
	/*
	 * YD=0 YU=1, measure XL and XR
	 * YD=1 YU=0, measure XL and XR
	 * The average of the previous four samples is the X value.
	 *
	 * XL=0 XR=1, measure YD and YU
	 * XL=1 XR=0, measure YD and YU
	 * The average of the previous four samples is the Y value.
	 *
	 * A total of 8 ADC measurements are needed!
	 */
	
//	GPIO_SetAnalog(GPIOA, TS_PIN_YP | TS_PIN_XP);
//	GPIO_SetOutput(GPIOB, TS_PIN_YM, 0);
//	GPIO_SetOutput(GPIOB, TS_PIN_XM, 1);
//	delay(100);
//	//val1 = (ADC_Measure(TS_ADCCH_XL) + ADC_Measure(TS_ADCCH_XM))/2;
//
//	GPIO_SetOutput(GPIOB, TS_PIN_YM, 1);
//	GPIO_SetOutput(GPIOB,TS_PIN_XM, 0);
//	delay(100);
//	//val2 = (ADC_Measure(TS_ADCCH_XL) + ADC_Measure(TS_ADCCH_XM))/2;
//
//	*adc_y = (val1+((1<<12)-val2))/4;
//
//	GPIO_SetAnalog(GPIOA, TS_PIN_YM | TS_PIN_XM);
//	GPIO_SetOutput(GPIOB, TS_PIN_YP, 0);
//	GPIO_SetOutput(GPIOB, TS_PIN_XP, 1);
//	delay(100);
//	//val1 = (ADC_Measure(TS_ADCCH_YD) + ADC_Measure(TS_ADCCH_YP))/2;
//
//	GPIO_SetOutput(GPIOB, TS_PIN_YP, 1);
//	GPIO_SetOutput(GPIOB, TS_PIN_XP, 0);
//	delay(100);
//	//val2 = (ADC_Measure(TS_ADCCH_YD) + ADC_Measure(TS_ADCCH_YP))/2;
//
//	*adc_x = (val1+((1<<12)-val2))/4;
}
