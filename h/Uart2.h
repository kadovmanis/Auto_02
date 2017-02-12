#ifndef	_uart2_h_
#define	_uart2_h_

#include	"Typedef.h"

void Uart2_Init				(BAUDRATE br);
void Uart2_SetBaudrate		(BAUDRATE br);
void Uart2_Flush			(void);

U8	 Uart2_ByteReceived		(void);
U8	 Uart2_StringReceived	(void);
U8	 Uart2_GetByte			(void);
void Uart2_SendByte			(char data);
void Uart2_GetString		(P_U8 str);
void Uart2_SendText			(const char *str);
void Uart2_SendData			(const char *str, U16 len);
void Uart2_SendEnter		(void);

#endif

