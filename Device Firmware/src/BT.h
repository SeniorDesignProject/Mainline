#ifndef __BT_H
#define __BT_H

#include "drivers.h"
#include "main.h"
#include "library.h"

#define BT_MAX_CHAR     100

void GiveName(char *DeviceName);
void BT_Init(void);
void BT_Connect(void);
//void BT_ChangeBaud(char *baudRate);
//void BT_Connect AT+AB SPPConnect AT-AB ConnectionUp

//char *str_append(char *s, char c);

#endif /* __BT_H */