#include	"Uart4.h"
#include	"Ports.h"

#ifdef UART4_WIFI

#define	UART4_TX		U4STAbits.UTXEN
#define	UART4_RX		U4MODEbits.UARTEN

static	volatile U8		TxBuff[256];
static	volatile U8		RxBuff[256];
static	volatile U8		TxWrite, TxSent, RxRec, RxRead, Enter;

extern OSC			Oscillator;
extern const U16	BaudRates[5][8];

// BaudRate_9600
// BaudRate_19200
// BaudRate_38400
// BaudRate_57600
// BaudRate_115200
void Uart4_Init(BAUDRATE br)
{
	U4MODE		= 0;				//	UART reset
	U4STA		= 0;
	U4BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	_U4TXIF		= 0;				// Clear the Transmit Interrupt Flag
	_U4RXIF		= 0;				// Clear the Recieve Interrupt Flag
//	_U4TXIE		= 1;				// Enable Transmit Interrupts
	_U4RXIE		= 1;				// Enable Recieve Interrupts

	UART4_RX	= 1;				// And turn the peripheral on
	UART4_TX	= 1;
}

void Uart4_SetBaudrate	(BAUDRATE br)
{
	U16 tmpMODE	= U4MODE;
	U16 tmpSTA	= U4STA;
	U4MODE		= 0;				//	UART Stop
	U4STA		= 0;
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	U4BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	U4MODE	= tmpMODE;
	U4STA	= tmpSTA;
}

void Uart4_Flush	(void)
{
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;
}

// **********************************************************************
//	Tx4 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U4TXInterrupt( void )
{
	_U4TXIF		= 0;				// Clear the Transmit Interrupt Flag
	if (TxWrite	!= TxSent)			//	if remain bytes in buffer
		U4TXREG	= TxBuff[TxSent++];	//	Transmit next byte
	else
		_U4TXIE		= 0;			// Disable Transmit Interrupt
}

extern inline	void	WifiPacketReceive	(U8 data);
// **********************************************************************
//	Rx4 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U4RXInterrupt( void )
{
	do
	{
		register U8	rxData = U4RXREG;	// read from uart1 Rx buffer
		_U4RXIF		= 0;				// Clear the Recieve Interrupt Flag
		if ((FL_TCP_REC_WIFI) || (rxData == STX))
			WifiPacketReceive(rxData);	// GPRS packet receive in progress
		else
		{
			if (rxData == CR)			// Enter received
			{
				Enter++;
				WIFI_IF = 1;			// Set WIFI Interrupt flag
			}
			RxBuff[RxRec++] = rxData; 	// save to rx buffer
		}
	} while (U4STAbits.URXDA);
	U4STAbits.OERR = 0;
}


U8 Uart4_ByteReceived	(void)
{
	return (RxRec != RxRead);
}

U8 Uart4_StringReceived	(void)
{
	return Enter;
}

U8 Uart4_GetByte(void)
{
	register U8 temp = RxBuff[RxRead++];
	if (temp == CR)					// Enter received
		Enter--;
	return temp;
}

void Uart4_SendByte(char data)
{
	TxBuff[TxWrite++] = data;
	if ((_U4TXIE == 0) && (TxWrite != TxSent))
	{
		_U4TXIE		= 1;				// Enable Transmit Interrupt
		_U4TXIF		= 1;				// Set Transmit Interrupt Flag
	}
}

void Uart4_GetString(P_U8 str)
{
	if (Enter)
	{
		while (1)
		{
			register U8 data = Uart4_GetByte();
			if (data == CR)
				break;
			else if (data != LF)
				*str++ = data;
		}
	}
	*str = '\0';
}

void Uart4_SendText	(const char *str)
{
	while (*str)
		Uart4_SendByte(*str++);
}

void Uart4_SendData	(const char *str, U16 len)
{
	while (len)
	{
		Uart4_SendByte(*str++);
		len--;
	}
}

void Uart4_SendEnter(void)
{
	Uart4_SendByte(CR);
	Uart4_SendByte(LF);
}

#endif  //  UART4_WIFI

