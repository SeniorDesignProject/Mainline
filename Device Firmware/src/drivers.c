#include "drivers.h"
#include "main.h"

/*Timer2*/
//time in ms
void Init_TIM2(int time) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	  
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = 0x0;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Period = (uint32_t)(CLK_FREQ/(time*1000));
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

	/* TIM2 Compare */
//	TIM_OCInitTypeDef TIM_OCInitStruct;
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Active;
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; 
//	TIM_OCInitStruct.TIM_Pulse = 0xFFFF;
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OC1Init(TIM2, &TIM_OCInitStruct);
}
void NVIC_TIM2(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority) {
  	NVIC_InitTypeDef NVIC_InitStructure;
 
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
void TIM2_Enable(void) {
  	TIM_Cmd(TIM2, ENABLE);
}

/*ADC4*/
void Init_ADC4(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  	
  	ADC_InitTypeDef ADC_InitStructure;
  
  	ADC_StructInit(&ADC_InitStructure);
  	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	          // Set conversion resolution to 12bit
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	                          // Enable Scan mode (single conversion for each channel of the group)
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			  // Disable Continuous conversion
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None; // Disable external conversion trigger
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  // Set conversion data alignement to right
  	ADC_InitStructure.ADC_NbrOfConversion = 1;             // Set conversion data alignement to ADC_CONV_BUFF_SIZE
  	ADC_Init(ADC1, &ADC_InitStructure);
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_384Cycles);
}
void NVIC_ADC4(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority) {
 	NVIC_InitTypeDef NVIC_InitStructure;
   
  	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn ;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
}
void ADC4_Enable(void) {
	ADC_DeInit(ADC1);
  	ADC_Cmd(ADC1, ENABLE); 
    /* Wait until the ADC1 is ready */
  	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET); 

  	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); 
        
        ADC_SoftwareStartConv(ADC1);
}


/*I2C*/
void Init_I2C1(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
   	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
        
   	GPIO_InitTypeDef GPIO_InitStructure;
   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef I2C_InitStructure;
   	
   	I2C_StructInit(&I2C_InitStructure);
   	I2C_InitStructure.I2C_ClockSpeed = 400;///////////////////////////////////////////////////////
   	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
   	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   	I2C_InitStructure.I2C_OwnAddress1 = 0x68;
   	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
   	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   	I2C_Init(I2C1, &I2C_InitStructure);
}
void NVIC_I2C1(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority) {
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}
void I2C1_Enable(void) {
	//I2C_DeInit(I2C1);
	I2C_Cmd(I2C1, ENABLE);

   	//I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
}

uint8_t I2C_Read(uint8_t RA) {
  uint32_t timeout = I2C_TIMEOUT_MAX;
  uint8_t Data = 0;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on I2C1 EV5 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2C1, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);

  /* Test on I2C1 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Send I2C1 location address LSB */
  I2C_SendData(I2C1, RA);

  /* Test on I2C1 EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Clear AF flag if arised */
  //I2C1-&gt;SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on I2C1 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress(I2C1, MEM_DEVICE_READ_ADDR, I2C_Direction_Receiver);

  /* Test on I2C1 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(I2C1, DISABLE);  

  /* Test on I2C1 EV7 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
  
  I2C_GenerateSTOP(I2C1, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(I2C1);

  /* return the read data */
  return Data;
}
uint8_t I2C_Write(uint8_t RA, uint8_t Data) {
  uint32_t timeout = I2C_TIMEOUT_MAX;
	
  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);
 
  /* Test on I2C1 EV5, Start trnsmitted successfully and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
 
  /* Send Memory device slave Address for write */
  I2C_Send7bitAddress(I2C1, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);
 
  /* Test on I2C1 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Send I2C1 location address LSB */
  I2C_SendData(I2C1, RA);
 
  /* Test on I2C1 EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
  /* Send Data */
  I2C_SendData(I2C1, Data);
 
  /* Test on I2C1 EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send I2C1 STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
 
  /* If operation is OK, return 0 */
  return 0;
}


/*USART*/
void Init_USART2(void) {
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
   
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   USART_InitTypeDef USART_InitStructure;
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b; //@ref USART_Word_Length
   USART_InitStructure.USART_StopBits = USART_StopBits_1; //@ref USART_Stop_Bits
   USART_InitStructure.USART_Parity = USART_Parity_No; //@ref USART_Parity
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //@ref USART_Mode
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //@ref USART_Hardware_Flow_Control
   USART_Init(USART2, &USART_InitStructure);
}

void NVIC_USART2(uint32_t NVIC_IRQChannelPreemptionPriority, uint32_t NVIC_IRQChannelSubPriority) {
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}
void USART2_Enable(void) {
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2, ENABLE);
}

void USART_TX(const char *str) {
  while(*str) {
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, *str++);
  }
}
void USART_RX(void) {
  
  while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  
  printf("%c", (char)USART_ReceiveData(USART2));
}