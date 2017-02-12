#ifndef	_uart3_h_
#define	_uart3_h_

#include	"Typedef.h"

void Uart3_Init			(void);
void Uart3_SetBaudrate	(U16 baudRate);

U8 Uart3ByteReceived	(void);
U8 Uart3StringReceived	(void);

U8 Uart3GetByte			(void);
void Uart3SendByte		(char data);
void Uart3GetString		(P_U8 str);
//void Uart3SendString	(P_R8 str);
void Uart3SendText		(const char *str);
void Uart3SendData		(const U8 *data, U16 len);
void Uart3SendEnter		(void);
void Uart3PrintDebugData(const U8 *data, U8 len);
U8	Uart3TxBuffResidual	(void);

#endif

