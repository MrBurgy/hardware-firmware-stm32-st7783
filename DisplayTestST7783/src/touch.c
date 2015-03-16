/*
 * Touch.c
 * 
 * Created: Mar 15, 2015
 *
 * Touch screen function 
 *
 */

#include "Touch.h"

#define TS_ADCx_CLK_ENABLE()                  __ADC1_CLK_ENABLE()
#define TS_ADCx_GPIO_CLK_ENABLE()             __GPIOA_CLK_ENABLE()
#define TS_ADCx_GPIO_CLK_DISABLE()            __GPIOA_CLK_DISABLE()

#define TS_PIN_XP	GPIO_PIN_10
#define TS_PIN_XM	GPIO_PIN_4
#define TS_PIN_YP	GPIO_PIN_1
#define TS_PIN_YM	GPIO_PIN_8

#define TS_PORT_XP	GPIOB
#define TS_PORT_XM	GPIOA
#define TS_PORT_YP	GPIOA
#define TS_PORT_YM	GPIOA

#define TS_ADCCH_XM	ADC_CHANNEL_4
#define TS_ADCCH_YP	ADC_CHANNEL_1
#define TS_ADC_PORT GPIOA
#define TS_ADC		ADC1


static ADC_HandleTypeDef m_hNucleo_Adc;
static ADC_ChannelConfTypeDef m_sConfig;

static void delay(volatile int i)
{
	for(;i;--i);
}

/**
 * \brief Initializes ADC MSP. (MSP MCU Specific Package)
 * 
 * \param	None
 * 
 * \return void
 */
static void ADCx_MspInit(void)
{
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  TS_ADCx_GPIO_CLK_ENABLE();
  
  /*** Configure the ADC peripheral ***/ 
  /* Enable ADC clock */
  TS_ADCx_CLK_ENABLE(); 
}

/**
 * \brief Initializes ADC
 * 
 * \param	None
 * 
 * \return void
 */
static void ADCx_Init(void)
{
  if(HAL_ADC_GetState(&m_hNucleo_Adc) == HAL_ADC_STATE_RESET)
  {
    /* ADC Config */
    m_hNucleo_Adc.Instance                   = TS_ADC;
    m_hNucleo_Adc.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4; /* (must not exceed 36MHz) */
    m_hNucleo_Adc.Init.Resolution            = ADC_RESOLUTION12b;
    m_hNucleo_Adc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    m_hNucleo_Adc.Init.ContinuousConvMode    = DISABLE;
    m_hNucleo_Adc.Init.DiscontinuousConvMode = DISABLE;
    m_hNucleo_Adc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    m_hNucleo_Adc.Init.EOCSelection          = EOC_SINGLE_CONV;
    m_hNucleo_Adc.Init.NbrOfConversion       = 1;
    m_hNucleo_Adc.Init.DMAContinuousRequests = DISABLE;    
    
    //ADCx_MspInit(&hnucleo_Adc);
    HAL_ADC_Init(&m_hNucleo_Adc);    
  }
}

/**
 * \brief Initializes the specified ADC channel
 * 
 * \param channel	Channel to initialize
 * 
 * \return uint8_t
 */
static uint8_t ADCx_Channel_Init(uint16_t channel)
{
	uint8_t status = 1;

	/* Select the ADC Channel to be converted */
	m_sConfig.Channel = channel;
	m_sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	m_sConfig.Rank = 1;
	status = HAL_ADC_ConfigChannel(&m_hNucleo_Adc, &m_sConfig);

	/* Return initialization status */
	return status;
}

/**
 * \brief Sets the specified pin to analog mode
 * 
 * \param GPIOx		GPIO port
 * \param pin		GPIO pin
 * 
 * \return void
 */
static void GPIO_SetAnalog(GPIO_TypeDef  *GPIOx, uint16_t pin)
{	
  GPIO_InitTypeDef GPIO_InitStruct;	
	
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

}

/**
 * \brief Sets the specified pin to output mode
 * 
 * \param GPIOx		GPIO port
 * \param pin		GPIO pin
 * 
 * \return void
 */
static void GPIO_SetOutput(GPIO_TypeDef  *GPIOx, uint16_t pin, int val)
{  
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOx, pin, val);
}

/**
 * \brief Sets the specified pin to input mode
 * 
 * \param GPIOx		GPIO port
 * \param pin		GPIO pin
 * 
 * \return void
 */
static uint16_t ADC_Measure(uint16_t ch)
{
	ADCx_Channel_Init(ch);

	uint16_t  adcValue = 0;

	/* Start the conversion process */
	HAL_ADC_Start(&m_hNucleo_Adc);

	/* Wait for the end of conversion */
	HAL_ADC_PollForConversion(&m_hNucleo_Adc, 10);

	/* Check if the continous conversion of regular channel is finished */
	if(HAL_ADC_GetState(&m_hNucleo_Adc) == HAL_ADC_STATE_EOC_REG)	{
		/* Get the converted value of regular channel */
		adcValue = HAL_ADC_GetValue(&m_hNucleo_Adc);
	}

	return adcValue;
}

/**
 * \brief Touch initialization
 * 
 * \param	None
 * 
 * \return void
 */
void Touch_Begin(void)
{
	ADCx_MspInit();
	ADCx_Init();
}

/**
 * \brief Gets the x-Coordinate raw value
 * 
 * \param	None
 * 
 * \return uint16_t		raw adc value
 */
uint16_t Touch_GetX(void)
{
	GPIO_SetAnalog(TS_PORT_YP, TS_PIN_YP);
	GPIO_SetOutput(TS_PORT_XM, TS_PIN_XM, GPIO_PIN_SET);

	GPIO_SetOutput(TS_PORT_XP, TS_PIN_XP, GPIO_PIN_SET);
	GPIO_SetOutput(TS_PORT_YM, TS_PIN_YM, GPIO_PIN_RESET);

	delay(100);

	uint32_t x = ADC_Measure(TS_ADCCH_YP);

	GPIO_SetOutput(TS_PORT_YP, TS_PIN_YP, GPIO_PIN_RESET);
	GPIO_SetOutput(TS_PORT_XM, TS_PIN_XM, GPIO_PIN_RESET);

	return x;
}

/**
 * \brief Gets the y-Coordinate raw value
 * 
 * \param	none
 * 
 * \return uint16_t		raw adc value
 */
uint16_t Touch_GetY(void)
{
	GPIO_SetOutput(TS_PORT_YP, TS_PIN_YP, GPIO_PIN_SET);
	GPIO_SetAnalog(TS_PORT_XM, TS_PIN_XM);

	GPIO_SetOutput(TS_PORT_XP, TS_PIN_XP, GPIO_PIN_RESET);
	GPIO_SetOutput(TS_PORT_YM, TS_PIN_YM, GPIO_PIN_SET);

	delay(100);

	uint16_t y = ADC_Measure(TS_ADCCH_XM);

	GPIO_SetOutput(TS_PORT_YP, TS_PIN_YP, GPIO_PIN_RESET);
	GPIO_SetOutput(TS_PORT_XM, TS_PIN_XM, GPIO_PIN_RESET);

	return y;
}
