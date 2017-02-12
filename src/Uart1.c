#include	"Uart1.h"
#include	"Ports.h"
#include	"Gps.h"

#define	UART1_TX		U1STAbits.UTXEN
#define	UART1_RX		U1MODEbits.UARTEN

static	volatile U8		TxBuff[256];
#ifndef	UART1_GPS
	static	volatile U8	RxBuff[256];
#endif
static	volatile U8		TxWrite, TxSent, RxRec, RxRead, Enter;

extern OSC			Oscillator;
extern const U16	BaudRates[5][8];

// BAUD_9600
// BAUD_19200
// BAUD_38400
// BAUD_115200
void Uart1_Init(BAUDRATE br)
{
	U1MODE		= 0;				//	UART reset
	U1STA		= 0;
	U1BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	_U1TXIF		= 0;				// Clear the Transmit Interrupt Flag
	_U1RXIF		= 0;				// Clear the Recieve Interrupt Flag
//	_U1TXIE		= 1;				// Enable Transmit Interrupts
	_U1RXIE		= 1;				// Enable Recieve Interrupts

	UART1_RX	= 1;				// And turn the peripheral on
	UART1_TX	= 1;
}

void Uart1_SetBaudrate	(BAUDRATE br)
{
	U16 tmpMODE	= U1MODE;
	U16 tmpSTA	= U1STA;
	U1MODE		= 0;				//	UART Stop
	U1STA		= 0;
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	U1BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	U1MODE	= tmpMODE;
	U1STA	= tmpSTA;
}

void Uart1_Flush	(void)
{
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;
}

// **********************************************************************
//	Tx1 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt( void )
{
	_U1TXIF		= 0;				// Clear the Transmit Interrupt Flag
	if (TxWrite	!= TxSent)			//	if remain bytes in buffer
		U1TXREG	= TxBuff[TxSent++];	//	Transmit next byte
	else
		_U1TXIE		= 0;			// Disable Transmit Interrupt
}

#ifndef	UART1_GPS
// **********************************************************************
//	Rx1 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void )
{
	U8	temp = U1RXREG;				// read from uart1 Rx buffer
	_U1RXIF		= 0;				// Clear the Recieve Interrupt Flag

	if (temp == CR)					// Enter received
	{
		Enter++;
		GPS_IF = 1;
	}
	RxBuff[RxRec++] = temp; 		// save to rx buffer
}


U8 Uart1_ByteReceived	(void)
{
	return (RxRec != RxRead);
}

U8 Uart1_StringReceived	(void)
{
	return Enter;
}

U8 Uart1_GetByte(void)
{
	register U8 temp = RxBuff[RxRead++];
	if (temp == CR)					// Enter received
		Enter--;
	return temp;
}
#endif
void Uart1_SendByte(char data)
{
	TxBuff[TxWrite++] = data;
	if ((_U1TXIE == 0) && (TxWrite != TxSent))
	{
		_U1TXIE		= 1;				// Enable Transmit Interrupt
		_U1TXIF		= 1;				// Set Transmit Interrupt Flag
	}
}
#ifndef	UART1_GPS
void Uart1_GetString(P_U8 str)
{
	if (Enter)
	{
		while (1)
		{
			register U8 data = Uart1_GetByte();
			if (data == CR)
				break;
			else if (data != LF)
				*str++ = data;
		}
	}
	*str = '\0';
}
#endif
void Uart1_SendText	(const char *str)
{
	while (*str)
		Uart1_SendByte(*str++);
}

void Uart1_SendData	(const char *str, U16 len)
{
	while (len)
	{
		Uart1_SendByte(*str++);
		len--;
	}
}

void Uart1_SendEnter(void)
{
	Uart1_SendByte(CR);
	Uart1_SendByte(LF);
}

