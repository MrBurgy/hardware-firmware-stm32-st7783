/*
 * Touch.h
 *
 * Created: Mar 15, 2015
 */

#ifndef TOUCHSCREEN_HW_H
#define TOUCHSCREEN_HW_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


void Touch_Begin(void);

void Touch_Getraw(uint16_t *adc_x, uint16_t *adc_y);

uint16_t Touch_GetY(void);
uint16_t Touch_GetX(void);

//void tshw_prepare_wait(void);

//void tshw_poll(uint16_t *adc_x, uint16_t *adc_y, int *pressed);


#ifdef __cplusplus
}
#endif

#endif	/* TOUCHSCREEN_HW_H */
