#ifndef	_uart1_h_
#define	_uart1_h_

#include	"Typedef.h"

void Uart1_Init				(BAUDRATE br);
void Uart1_SetBaudrate		(BAUDRATE br);
void Uart2_Flush			(void);

U8	 Uart1_ByteReceived		(void);
U8	 Uart1_StringReceived	(void);
U8	 Uart1_GetByte			(void);
void Uart1_SendByte			(char data);
void Uart1_GetString		(P_U8 str);
void Uart1_SendText			(const char *str);
void Uart1_SendData			(const char *str, U16 len);
void Uart1_SendEnter		(void);

#endif

