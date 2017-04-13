#include	"Uart2.h"
#include	"Ports.h"
#include	"Gsm.h"

#define	UART2_TX		U2STAbits.UTXEN
#define	UART2_RX		U2MODEbits.UARTEN

static	volatile U8		TxBuff[256];
static	volatile U8		RxBuff[256];
static	volatile U8		TxWrite, TxSent, RxRec, RxRead, Enter;

extern OSC			Oscillator;
extern const U16	BaudRates[5][8];

// BAUD_9600
// BAUD_19200
// BAUD_38400
// BAUD_115200
void Uart2_Init(BAUDRATE br)
{
	U2MODE		= 0;				//	UART reset
	U2STA		= 0;
	U2BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	_U2TXIF		= 0;				// Clear the Transmit Interrupt Flag
	_U2RXIF		= 0;				// Clear the Recieve Interrupt Flag
//	_U2TXIE		= 1;				// Enable Transmit Interrupts
	_U2RXIE		= 1;				// Enable Recieve Interrupts

	UART2_RX	= 1;				// And turn the peripheral on
	UART2_TX	= 1;
}

void Uart2_SetBaudrate	(BAUDRATE br)
{
	U16 tmpMODE	= U2MODE;
	U16 tmpSTA	= U2STA;
	U2MODE		= 0;				//	UART Stop
	U2STA		= 0;
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;

	U2BRG		= BaudRates[br][Oscillator];	//	BAUD Rate

	U2MODE	= tmpMODE;
	U2STA	= tmpSTA;
}

void Uart2_Flush	(void)
{
	TxWrite = TxSent = RxRec = RxRead = Enter = 0;
}

// **********************************************************************
//	Tx2 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt( void )
{
	_U2TXIF		= 0;				// Clear the Transmit Interrupt Flag
	if (TxWrite	!= TxSent)			//	if remain bytes in buffer
		U2TXREG	= TxBuff[TxSent++];	//	Transmit next byte
	else
		_U2TXIE		= 0;			// Disable Transmit Interrupt
}

extern inline	void	GprsPacketReceive	(U8 data);
// **********************************************************************
//	Rx2 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void )
{
	do
	{
		register U8	rxData = U2RXREG;	// read from uart1 Rx buffer
		_U2RXIF		= 0;				// Clear the Recieve Interrupt Flag
		if ((FL_TCP_REC_GPRS) || (rxData == STX))
			GprsPacketReceive(rxData);	// GPRS packet receive in progress
		else
		{
			if (rxData == CR)			// Enter received
			{
				Enter++;
				GSM_IF = 1;				// Set GSM Interrupt flag
			}
			RxBuff[RxRec++] = rxData; 	// save to rx buffer
		}
	} while (U2STAbits.URXDA);
	U2STAbits.OERR = 0;
}


U8 Uart2_ByteReceived	(void)
{
	return (RxRec != RxRead);
}

U8 Uart2_StringReceived	(void)
{
	return Enter;
}

U8 Uart2_GetByte(void)
{
	register U8 temp = RxBuff[RxRead++];
	if (temp == CR)					// Enter received
		Enter--;
	return temp;
}

void Uart2_SendByte(char data)
{
	TxBuff[TxWrite++] = data;
	if ((_U2TXIE == 0) && (TxWrite != TxSent))
	{
		_U2TXIE		= 1;				// Enable Transmit Interrupt
		_U2TXIF		= 1;				// Set Transmit Interrupt Flag
	}
}

void Uart2_GetString(P_U8 str)
{
	if (Enter)
	{
		while (1)
		{
			register U8 data = Uart2_GetByte();
			if (data == CR)
				break;
			else if (data != LF)
				*str++ = data;
		}
	}
	*str = '\0';
}

void Uart2_SendText	(const char *str)
{
	while (*str)
		Uart2_SendByte(*str++);
}

void Uart2_SendData	(const char *str, U16 len)
{
	while (len)
	{
		Uart2_SendByte(*str++);
		len--;
	}
}

void Uart2_SendEnter(void)
{
	Uart2_SendByte(CR);
	Uart2_SendByte(LF);
}

