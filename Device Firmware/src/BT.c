#include "BT.h"

char DeviceName[9];

void GiveName(char *DeviceName) {
	DeviceName[0] = 'L';
	DeviceName[1] = 'e';
	DeviceName[2] = 'a';
	DeviceName[3] = 'f';
	DeviceName[4] = ' ';
	DeviceName[5] = RandNum();
	DeviceName[6] = RandNum();
	DeviceName[7] = RandAlpha();
	DeviceName[8] = RandAlpha();
}

void BT_Init(void) {
  /*Make sure in CMD mode*/
  //USART_TX("^#^$^%");
  /*BT Name*/
  GiveName(DeviceName);
  USART_TX("AT+AB DefaultLocalName ");
  USART_TX(DeviceName);
  USART_TX("\r\n");
  /*BT BaudRate*/
  USART_TX("AT+AB ChangeBaud 115200\r\n");
  /*BT DisableBond*/
  USART_TX("AT+AB DisableBond\r\n");
}

//void BT_Connect(void) {
//  USART_Send_String("AT+AB ChangeBaud 115200");
//  USART_Recieve_String();
//  
//  AT-AB Baudrate Changed
//  
//}
// 


