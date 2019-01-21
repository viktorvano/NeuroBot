/*
 * NeuroBot.h
 *
 *  Created on: Dec 5, 2018
 *      Author: CyberPunkTECH
 */

#ifndef NEUROBOT_H_
#define NEUROBOT_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include "usb_device.h"
#include "ssd1306_tests.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "tof.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hi2c2;
extern SD_HandleTypeDef hsd;
extern HAL_SD_CardInfoTypedef SDCardInfo;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart2;

#define BUTTON_POWER GPIOA,GPIO_PIN_0

typedef struct
{
	uint8_t pressed:1;
	uint8_t released:1;
}ButtonStruct;

extern char buffer[50];
extern ButtonStruct OK, UP, DOWN, POWER;
extern int iDistance;
extern int model, revision;
extern uint8_t menu, redraw, cnt_flag;
extern uint32_t ADC[4];
extern float Battery;
extern int percent, percent_old;
extern FATFS myFAT;
extern uint16_t connection_cnt, cnt_20ms;

extern void NeuroBot_init();
extern void servoY(float value);
extern void servoX(float value);
extern float measure_distance();
extern uint8_t string_compare(char array1[], char array2[], uint16_t lenght);
extern void Message_handler();
extern void overheat_check();
extern void motor_stop();
extern void motorA_forward();
extern void motorA_backward();
extern void motorA_stop();
extern void motorB_forward();
extern void motorB_backward();
extern void motorB_stop();
extern void motor_direction(float left, float right);
extern void GPIO_POWER_Init(void);
extern void GPIO_POWER_DeInit(void);
extern void NeuroBot_sleep(void);
extern void USB_mode();
extern void Bluetooth_mode();
extern void buttons_release();
extern void menu_manager();
extern void display_manager();
extern void display_battery();
extern void display_message(const char *line_1, const char *line_2, const char *line_3);

#endif /* NEUROBOT_H_ */
