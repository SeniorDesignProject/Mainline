/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRIVERS_H
#define __DRIVERS_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32l1xx.h"
//#include "discover_board.h"
//#include "stm32l_discovery_lcd.h"
//#include <stdio.h>
#include <stdint.h>
#include "global.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#define I2C_TIMEOUT_MAX 20
#define MEM_DEVICE_READ_ADDR    0xD1
#define MEM_DEVICE_WRITE_ADDR   0xD0

/*TIM2*/
void Init_TIM2(int time);
void NVIC_TIM2(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority);
void TIM2_Enable(void);

/*ADC4*/
void Init_ADC4(void);
void NVIC_ADC4(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority);
void ADC4_Enable(void);

/*I2C1*/
void Init_I2C1(void);
void NVIC_I2C1(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority);
void I2C1_Enable(void);
uint8_t I2C_Read(uint8_t RA);
uint8_t I2C_Write(uint8_t RA, uint8_t Data);

/*USART*/
void Init_USART2(void);
void NVIC_USART2(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority);
void USART2_Enable(void);
void USART_TX(const char *str);
void USART_RX(void);

#endif /* __DRIVERS_H */
