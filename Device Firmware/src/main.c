/**
  ******************************************************************************
  * @file    main.c
  * @author  Microcontroller Division
  * @version V1.0.3
  * @date    May-2013
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */
 
/* Includes ------------------------------------------------------------------*/

#include "stm32l1xx.h"
#include <stdio.h>
//#include "discover_board.h"
//#include "stm32l_discovery_lcd.h"
#include "drivers.h"
#include "MPU6050.h"
#include "BT.h"
#include "library.h"

#include "global.h"
#include <stdlib.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define DEBUG_SWD_PIN  1  /* needs to be set to 1 to enable SWD debug pins, set to 0 for power consumption measurement*/



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*Variables for Leaf State definitions*/
enum leaf_state{
  INITIALIZE,
  PAIR,
  POLL
};
typedef int STATE;
STATE state; 

int flag_UART;
int flag_delay;

char buffer[100];
int counter;
int result;

int event_count;
//volatile bool flag_ADCDMA_TransferComplete;
//volatile bool flag_UserButton;

static volatile uint32_t TimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

__IO FLASH_Status FLASHStatus = FLASH_COMPLETE; 


/* Private function prototypes -----------------------------------------------*/
void  RCC_Configuration(void);
void  RTC_Configuration(void);
void  Init_GPIOs (void);
void  configureWakeup (void);
void clearUserButtonFlag(void);
/*******************************************************************************/

void MCU_Init(void) {
  
  /* Set internal voltage regulator to 1.8V */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;

  /* Enable debug features in low power modes (Sleep, STOP and STANDBY) */
#ifdef  DEBUG_SWD_PIN
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif
  
  /* Configure SysTick IRQ and SysTick Timer to generate interrupts */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 500);
}


void RTC_Configuration(void) {
  
/* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  
  RCC_RTCCLKCmd(ENABLE);
   
  /* LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

}
void RCC_Configuration(void) {  
  
  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
  RCC_MSIRangeConfig(RCC_MSIRange_6); //CHANGED FROM 1

  RCC_HSEConfig(RCC_HSE_OFF);  
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  {
    while(1);
  }

  /* Enable  comparator clock LCD and PWR mngt */
  //ENABLE LATER 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LCD | RCC_APB1Periph_PWR, ENABLE);
   
  /* Enable SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}
void CLK_Config(void) {
  /* Configure clk for std peripherals */
  RCC_Configuration();
  /* Configure RTC clk */
  RTC_Configuration();
}


void Init_GPIOs(void) {
   /* Enable GPIOA, GPIOB (+I2C1), GPIOC */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOH, &GPIO_InitStructure);

#if  DEBUG_SWD_PIN == 1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & (~GPIO_Pin_13) & (~GPIO_Pin_14);
#endif
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  EXTI_InitTypeDef EXTI_InitStructure;
  /* Configure User Button pin as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource0);
  /* Configure User Button and IDD_WakeUP EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 ;  // PA0 for User button AND IDD_WakeUP
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


/* Configure the GPIO_LED pins  LD3 & LD4*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}  

void NVIC_GPIOs(void) {
  
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /* Enable and set User Button and IDD_WakeUP EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}

void DRIVER_Config(void) {
  Init_GPIOs();
  Init_USART2();
  Init_I2C1();
}

void NVIC_Config(void) {
  NVIC_GPIOs();
  NVIC_USART2(1,0);
  NVIC_I2C1(2,0);

}

void DRIVER_Enable(void) {
  USART2_Enable();
  I2C1_Enable();

}
//
//void delay(int delayTime) {
//  uint16_t start = TIM2->CNT;
//  while((uint16_t)(TIM2->CNT - start) <= (uint16_t)delayTime);
//}

//in ms
void delay(int delayTime) {
//  flag_delay = 0;
//    Init_TIM2(delayTime);
//    NVIC_TIM2(3,0);
//      TIM2_Enable();
//  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  unsigned long i;
  unsigned long value = (CLK_FREQ*(delayTime/1000));
  for(i=0; i<value; i++) {
    asm volatile ("nop\n");
  }
}

void InitGPIO() {
    //GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
}



int main(void)
{
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_md.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */ 

  state = INITIALIZE;
  flag_UART = 0;
  counter = 0;
   result = 1;
   
   event_count = 0;
   //GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
  
  uint16_t x,y,z;
  while(1){
    switch(state)
    {
    case INITIALIZE:
     
      printf("Initializing\n");
      /* Initialize MCU core components */
      MCU_Init();
      /* Configure Clocks */
      CLK_Config();
      /* Configure StdPerph drivers */
      DRIVER_Config();
      /* Configure NVIC */
      NVIC_Config();
      /* Enable StdPerph (Make sure last, due to interrupts occuring)*/
      DRIVER_Enable();


      MPU6050_Init();
      //BT_Init();
      GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);    
      delay(1000);
      state = PAIR;
      printf("Done initializing\n");
      GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); 
      break;
    case PAIR:
     // printf("Pairing\n");
      //printf("%s\n", buffer);
//      if(counter >= 16) {
//        printf("%s\n", buffer);
//      }
      if(counter >= 18) {
        result = strncmp(buffer, "AT-AB ConnectionUp", 18);
        if(result == 0) {
          printf("%s\n", buffer);
      
          USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
          ClearString(buffer);
          counter = 0;   
          
          state = POLL;
        }
        else {
          ClearString(buffer);
          counter = 0;
        }
      }
      break;
    case POLL:
       x = getRawAccelX();
       if(x >= 0x0FFF) {
         USART_TX("Event");
         USART_TX((char*)(event_count+48));
         USART_TX("\n");
         event_count++;
       }
       delay(500);

      break;
    default:
      state = INITIALIZE;
   }
  }
}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
