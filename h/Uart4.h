#ifndef	_uart4_h_
#define	_uart4_h_

#include	"Typedef.h"

void Uart4_Init				(BAUDRATE br);
void Uart4_SetBaudrate		(BAUDRATE br);
void Uart4_Flush			(void);

U8	 Uart4_ByteReceived		(void);
U8	 Uart4_StringReceived	(void);
U8	 Uart4_GetByte			(void);
void Uart4_SendByte			(char data);
void Uart4_GetString		(P_U8 str);
void Uart4_SendText			(const char *str);
void Uart4_SendData			(const char *str, U16 len);
void Uart4_SendEnter		(void);

#endif	// _uart4_h_

