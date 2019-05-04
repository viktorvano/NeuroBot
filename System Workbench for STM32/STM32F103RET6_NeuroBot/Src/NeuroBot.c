/*
 * NeuroBot.c
 *
 *  Created on: Dec 5, 2018
 *      Author: CyberPunkTECH
 */

#include "NeuroBot.h"


char buffer[50];
ButtonStruct OK, UP, DOWN, POWER;
int iDistance;
int model=0, revision=0;
uint8_t menu=0, redraw=1, cnt_flag=0;
uint32_t ADC[4];
float Battery=2500;
int percent=100, percent_old;
FATFS myFAT;
uint16_t connection_cnt=1000, cnt_20ms=250;

void NeuroBot_init()
{
	HAL_TIM_Base_Start(&htim3);
	servoX(0.5f);
	servoY(0.0f);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	HAL_GPIO_WritePin(MOSFET_Enable_GPIO_Port, MOSFET_Enable_Pin, SET);
	HAL_GPIO_WritePin(ToF_Enable_GPIO_Port, ToF_Enable_Pin, SET);
	while(I2C2->SR2==2)
	{
	  while(HAL_I2C_DeInit(&hi2c2)!=HAL_OK);
	  while(HAL_I2C_Init(&hi2c2)!=HAL_OK);
	}
	tofInit(1); // set long range mode (up to 2m)
	tofGetModel(&model, &revision);
	iDistance = tofReadDistance();
	while(model!=238 || revision!=16)
	{
	  while(HAL_I2C_DeInit(&hi2c2)!=HAL_OK);
	  while(HAL_I2C_Init(&hi2c2)!=HAL_OK);
	  tofInit(1); // set long range mode (up to 2m)
	  tofGetModel(&model, &revision);
	  iDistance = tofReadDistance();
	}
	HAL_TIM_Base_Start_IT(&htim2);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC,4);
	HAL_ADC_Start_IT(&hadc1);

	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(30, 0);
	ssd1306_WriteString("Hello", Font_11x18, White);
	ssd1306_SetCursor(35, 20);
	ssd1306_WriteString("I am", Font_11x18, White);
	ssd1306_SetCursor(20, 40);
	ssd1306_WriteString("NeuroBot", Font_11x18, White);
	ssd1306_UpdateScreen();

	memset(buffer, 0, sizeof(buffer));
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	GPIO_POWER_Init();
	HAL_Delay(2000);
	buttons_release();

	HAL_GPIO_WritePin(Motor_Enable_GPIO_Port, Motor_Enable_Pin, SET);
	HAL_GPIO_WritePin(Bluetooth_Enable_GPIO_Port, Bluetooth_Enable_Pin, RESET);
	HAL_GPIO_WritePin(LED_Training_GPIO_Port, LED_Training_Pin, RESET);
	HAL_GPIO_WritePin(LED_Run_GPIO_Port, LED_Run_Pin, RESET);
	if(!HAL_GPIO_ReadPin(SD_DETECTION_GPIO_Port, SD_DETECTION_Pin))
	{
		int temp=0;
	  hsd.Init.ClockDiv = 36;
	  while(HAL_SD_Init(&hsd, &SDCardInfo)!=SD_OK)
	  {
		  temp++;
		  HAL_Delay(100);
		  if(temp>=30)
			  NeuroBot_sleep();
	  }
	  temp=0;
	  while(HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B))
	  {
		  temp++;
		  HAL_Delay(100);
		  if(temp>=30)
			  NeuroBot_sleep();
	  }
	}else
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2, 26);
		ssd1306_WriteString("Insert SD", Font_11x18, White);
		ssd1306_SetCursor(2, 26+18);
		ssd1306_WriteString("Press OK to OFF.", Font_7x10, White);
		ssd1306_UpdateScreen();
		while(!OK.released);
		NeuroBot_sleep();
	}

	uint8_t attempts=0;
	while(1)
	{
		attempts++;
		if(f_mount(&myFAT, SD_Path, 1)==FR_OK)
			break;
		if(attempts>10)
			NeuroBot_sleep();
		HAL_Delay(150);
	}

	percent_old=percent;
}

void servoY(float value)
{
	if(value<=0.0f)
		value=0.0f;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 51+(value*52.0f));
}

void servoX(float value)
{
	if(value<=0.0f)
		value=0.0f;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 33+(value*80.0f));
}

float measure_distance()
{
	iDistance = tofReadDistance();
	if(iDistance>2000)
	{
		iDistance=2000;
	}else
	if(iDistance<0)
	{
		iDistance=0;
	}

	return (float)iDistance/2000.0f;
}

uint8_t string_compare(char array1[], char array2[], uint16_t lenght)
{
	 uint8_t comVAR=0, i;
	 for(i=0;i<lenght;i++)
	   	{
	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	   	}
	 if (comVAR==lenght)
		 	return 1;
	 else 	return 0;
}

void Message_handler()
{
	if(string_compare(buffer, "stop", strlen("stop")))
	{
		motor_stop();
	}else
	if(string_compare(buffer, "forward", strlen("forward")))
	{
		motorA_forward();
		motorB_forward();
	}else
	if(string_compare(buffer, "backward", strlen("backward")))
	{
		motorA_backward();
		motorB_backward();
	}else
	if(string_compare(buffer, "right", strlen("right")))
	{
		motorA_forward();
		motorB_backward();
	}else
	if(string_compare(buffer, "left", strlen("left")))
	{
		motorA_backward();
		motorB_forward();
	}
}

void overheat_check()
{
	if(!HAL_GPIO_ReadPin(Motor_nFault_GPIO_Port, Motor_nFault_Pin))//overheat
	{
		HAL_GPIO_WritePin(Motor_A1_GPIO_Port, Motor_A1_Pin, 0);
		HAL_GPIO_WritePin(Motor_A2_GPIO_Port, Motor_A2_Pin, 0);
		HAL_GPIO_WritePin(Motor_B1_GPIO_Port, Motor_B1_Pin, 0);
		HAL_GPIO_WritePin(Motor_B2_GPIO_Port, Motor_B2_Pin, 0);
		ssd1306_Fill(Black);
		ssd1306_SetCursor(30, 0);
		ssd1306_WriteString("Motor", Font_11x18, White);
		ssd1306_SetCursor(35, 20);
		ssd1306_WriteString("driver", Font_11x18, White);
		ssd1306_SetCursor(20, 40);
		ssd1306_WriteString("overheat", Font_11x18, White);
		ssd1306_UpdateScreen();
		while(!HAL_GPIO_ReadPin(Motor_nFault_GPIO_Port, Motor_nFault_Pin));
	}
}

void motor_stop()
{
	  HAL_GPIO_WritePin(Motor_A1_GPIO_Port, Motor_A1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_A2_GPIO_Port, Motor_A2_Pin, 0);
	  HAL_GPIO_WritePin(Motor_B1_GPIO_Port, Motor_B1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_B2_GPIO_Port, Motor_B2_Pin, 0);
	  overheat_check();
}

void motorA_forward()
{
	  HAL_GPIO_WritePin(Motor_A1_GPIO_Port, Motor_A1_Pin, 1);
	  HAL_GPIO_WritePin(Motor_A2_GPIO_Port, Motor_A2_Pin, 0);
}

void motorA_backward()
{
	  HAL_GPIO_WritePin(Motor_A1_GPIO_Port, Motor_A1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_A2_GPIO_Port, Motor_A2_Pin, 1);
}

void motorA_stop()
{
	  HAL_GPIO_WritePin(Motor_A1_GPIO_Port, Motor_A1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_A2_GPIO_Port, Motor_A2_Pin, 0);
}

void motorB_forward()
{
	  HAL_GPIO_WritePin(Motor_B1_GPIO_Port, Motor_B1_Pin, 1);
	  HAL_GPIO_WritePin(Motor_B2_GPIO_Port, Motor_B2_Pin, 0);
}

void motorB_backward()
{
	  HAL_GPIO_WritePin(Motor_B1_GPIO_Port, Motor_B1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_B2_GPIO_Port, Motor_B2_Pin, 1);
}

void motorB_stop()
{
	  HAL_GPIO_WritePin(Motor_B1_GPIO_Port, Motor_B1_Pin, 0);
	  HAL_GPIO_WritePin(Motor_B2_GPIO_Port, Motor_B2_Pin, 0);
}

void motor_direction(float left, float right)
{
	servoY(1.0f);
	HAL_Delay(500);

	if(left<0.0f)
		left=0.0f;
	else
	if(left>1.0f)
		left=1.0f;

	if(right<0.0f)
		right=0.0f;
	else
	if(right>1.0f)
		right=1.0f;

	if(left<0.5f)
		motorA_backward();
	else
		motorA_forward();

	if(right<0.5f)
		motorB_backward();
	else
		motorB_forward();

	char msg1[15], msg2[15], msg3[15];
	memset(msg1, 0, 15);
	memset(msg2, 0, 15);
	memset(msg3, 0, 15);
	strcat(msg1, "Outputs:");
	strcat(msg2, "L: ");
	strcat(msg3, "R: ");
	ftoa(left, &msg2[strlen(msg2)], 3);
	ftoa(right, &msg3[strlen(msg3)], 3);
	display_message(msg1, msg2, msg3);

	if((right>=0.5f && left>=0.5f) || (right<0.5f && left<0.5f))
	{
		if(right<0.5f)
			right=1.0f-right;

		if(left<0.5f)
			left=1.0f-left;

		float certainty=(right+left)-1.0f;

		cnt_20ms=0;
		cnt_flag=1;
		uint16_t waiting_time=40.0f*certainty + 10;// 200...1000ms
		while(waiting_time>cnt_20ms)
		{
			if(measure_distance()>(90.0f/2000.0f))
			{
				motorA_backward();
				motorB_backward();
				HAL_Delay(500);
				motorA_forward();
				motorB_backward();
				HAL_Delay(1500);
				motor_stop();
				break;
			}
		}
		cnt_flag=0;
		cnt_20ms=0;
	}
	else
	{
		if(right<0.5f)
			right=1.0f-right;

		if(left<0.5f)
			left=1.0f-left;

		float certainty=(right+left)-1.0f;

		HAL_Delay(350.0f*certainty + 200);//200...550ms
	}
	servoY(0.0f);
}

void GPIO_POWER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void GPIO_POWER_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
}

void NeuroBot_sleep(void)
{
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop(&htim3);
	GPIO_POWER_DeInit();
	RCC->APB1ENR|=RCC_APB1ENR_PWREN;
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_EnterSTANDBYMode();
}

void USB_mode()
{
	HAL_GPIO_WritePin(USB_Enable_GPIO_Port, USB_Enable_Pin, SET);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2, 26);
	ssd1306_WriteString("USB mode", Font_11x18, White);
	ssd1306_SetCursor(2, 26+18);
	ssd1306_WriteString("DOWN to exit.", Font_7x10, White);
	ssd1306_UpdateScreen();
	while(!DOWN.released);
	HAL_GPIO_WritePin(USB_Enable_GPIO_Port, USB_Enable_Pin, RESET);
	buttons_release();
	menu=0;
	redraw=1;
}

void Bluetooth_mode()
{
	HAL_GPIO_WritePin(Bluetooth_Enable_GPIO_Port, Bluetooth_Enable_Pin, SET);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2, 0);
	ssd1306_WriteString("Bluetooth", Font_11x18, White);
	ssd1306_SetCursor(2, 20);
	ssd1306_WriteString("control", Font_11x18, White);
	ssd1306_SetCursor(2, 26+18);
	ssd1306_WriteString("DOWN to exit.", Font_7x10, White);
	ssd1306_UpdateScreen();
	while(!DOWN.released)
	{
		memset(buffer, 0, 20);
		HAL_UART_Receive(&huart2, (uint8_t*)buffer, 20, 75);

		if(strlen(buffer)==0)
			connection_cnt++;
		else
			connection_cnt=0;

		if(connection_cnt>5)
			motor_stop();

		if(connection_cnt>30)
			connection_cnt=30;

		Message_handler();
	}
	buttons_release();
	HAL_GPIO_WritePin(Bluetooth_Enable_GPIO_Port, Bluetooth_Enable_Pin, RESET);
	motor_stop();
	menu=0;
	redraw=1;
}

void buttons_release()
{
	HAL_Delay(20);
	UP.released=0;
	DOWN.released=0;
	OK.released=0;
	POWER.released=0;
}

void menu_manager()
{

	if(menu<2 && DOWN.released)
	{
		buttons_release();
		menu++;
		redraw=1;
	}else
	if(menu>0 && UP.released)
	{
		buttons_release();
		menu--;
		redraw=1;
	}else
	if(menu==0 && UP.released)
	{
		buttons_release();
		menu=2;
		redraw=1;
	}else
	if(menu==2 && DOWN.released)
	{
		buttons_release();
		menu=0;
		redraw=1;
	}else

	if(menu==0 && OK.released)
	{
		buttons_release();
		start_NN();
		redraw=1;
	}else

	if(menu==1 && OK.released)
	{
		buttons_release();
		menu=6;
		redraw=1;
	}else
	if(menu==2 && OK.released)
	{
		buttons_release();
		menu=7;
		redraw=1;
	}else

	if(POWER.released)
	{
		buttons_release();
		menu=100;
		redraw=1;
	}

	if(percent!=percent_old
	|| !HAL_GPIO_ReadPin(Battery_Charging_GPIO_Port, Battery_Charging_Pin)
	|| !HAL_GPIO_ReadPin(Battery_Full_GPIO_Port, Battery_Full_Pin))
	{
		if(!HAL_GPIO_ReadPin(Battery_Charging_GPIO_Port, Battery_Charging_Pin)
		|| !HAL_GPIO_ReadPin(Battery_Full_GPIO_Port, Battery_Full_Pin))
			percent_old=210;
		else
			percent_old=percent;
		redraw=1;
	}


	if(redraw==1)
		display_manager();
}

void display_manager()
{
	redraw=0;
	if(menu>=0 && menu<=2)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2, 0);
		ssd1306_WriteString("Main menu", Font_11x18, White);
		display_battery();
		ssd1306_SetCursor(2, 30+0);
		ssd1306_WriteString("Neural Network", Font_7x10, White);
		ssd1306_SetCursor(2, 30+10);
		ssd1306_WriteString("USB mode", Font_7x10, White);
		ssd1306_SetCursor(2, 30+20);
		ssd1306_WriteString("Bluetooth mode", Font_7x10, White);
		ssd1306_SetCursor(100, 30+10*menu);
		ssd1306_WriteString("<<<", Font_7x10, White);
		ssd1306_UpdateScreen();
	}else
	if(menu==6)
	{
		USB_mode();
	}else
	if(menu==7)
	{
		Bluetooth_mode();
	}else
	if(menu==100)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2, 26);
		ssd1306_WriteString("Turn OFF?", Font_11x18, White);
		ssd1306_SetCursor(20, 26+26);
		ssd1306_WriteString("DOWN to keep ON", Font_7x10, White);
		ssd1306_SetCursor(28, 0);
		ssd1306_WriteString("UP to turn OFF", Font_7x10, White);
		ssd1306_UpdateScreen();

		while(1)
		{
			if(UP.released)
			{
				buttons_release();
				NeuroBot_sleep();
			}

			if(DOWN.released)
			{
				buttons_release();
				menu=0;
				redraw=1;
				break;
			}
		}
		buttons_release();
	}
}

void display_battery()
{
	ssd1306_SetCursor(2, 20);
	char string[30];
	memset(string, 0, 30);
	if(!HAL_GPIO_ReadPin(Battery_Charging_GPIO_Port, Battery_Charging_Pin))
	{
		strcat(string, "Charging...");
		ssd1306_WriteString(string, Font_7x10, White);
	}else
	if(!HAL_GPIO_ReadPin(Battery_Full_GPIO_Port, Battery_Full_Pin))
	{
		strcat(string, "Battery Full :)");
		ssd1306_WriteString(string, Font_7x10, White);
	}else
	{
		strcat(string, "Battery: ");
		itoa(percent, &string[strlen(string)], 10);
		strcat(string, " %");
		ssd1306_WriteString(string, Font_7x10, White);
	}

}

void display_message(const char *line_1, const char *line_2, const char *line_3)
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString((char*)line_1, Font_11x18, White);
	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString((char*)line_2, Font_11x18, White);
	ssd1306_SetCursor(0, 40);
	ssd1306_WriteString((char*)line_3, Font_11x18, White);
	ssd1306_UpdateScreen();
}
