#include	"Uart3.h"
#include	"Ports.h"
//#include	"Memory.h"
#include	<stdio.h>

#define	UART3_TX		U3STAbits.UTXEN
#define	UART3_RX		U3MODEbits.UARTEN

static	volatile U8		TxBuff[256];
static	volatile U8		RxBuff[256];
static	volatile U8		TxWrite, TxSent, RxRec, RxRead, Enter;


// BAUD_9600
// BAUD_19200
// BAUD_38400
// BAUD_115200
void Uart3_Init	(void)
{
	U3MODE		= 0;				//	UART reset
	U3STA		= 0;
//	U3BRG		= baudRate;			//	BAUD Rate

	TxWrite = TxSent = RxRec = RxRead = Enter = 0;
//	GsmDataAutoSort = 0;

	_U3TXIF		= 0;				// Clear the Transmit Interrupt Flag
	_U3RXIF		= 0;				// Clear the Recieve Interrupt Flag
//	_U3TXIE		= 1;				// Enable Transmit Interrupts
	_U3RXIE		= 1;				// Enable Recieve Interrupts

	UART3_RX	= 1;				// And turn the peripheral on
	UART3_TX	= 1;
}

void Uart3_SetBaudrate	(U16 baudRate)
{
	U16 tmpMODE	= U3MODE;
	U16 tmpSTA	= U3STA;
	U3MODE		= 0;				//	UART Stop
	U3STA		= 0;

	U3BRG		= baudRate;			//	BAUD Rate

	U3MODE	= tmpMODE;
	U3STA	= tmpSTA;
}

// **********************************************************************
//	Tx3 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U3TXInterrupt( void )
{
	_U3TXIF		= 0;				// Clear the Transmit Interrupt Flag
//	if		(FLAG_MEM_TO_UART3)		// Direct Mem data send mode
//	{
//		do
//		{
//			register U8 a = DataFromSpiMem();			// get next byte from Spi memory
//			U3TXREG = (a == 0xE7)?	0x1C: (a ^ 0xFF);	// send to uart inverted bits (if data is magic bitmap code - change it)
//		} while (FLAG_MEM_TO_UART3 && !U3STAbits.UTXBF);
//	}
//	else if	(TxWrite	!= TxSent)	//	if remain bytes in buffer
	if	(TxWrite	!= TxSent)	//	if remain bytes in buffer
		U3TXREG	= TxBuff[TxSent++];	//	Transmit next byte
	else
		_U3TXIE		= 0;			// Disable Transmit Interrupt
}

// **********************************************************************
//	Rx3 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U3RXInterrupt( void )
{
	U8	temp = U3RXREG;				// read from uart1 Rx buffer
	_U3RXIF		= 0;				// Clear the Recieve Interrupt Flag

	{
		if (temp == CR)					// Enter received
			Enter++;
		RxBuff[RxRec++] = temp; 		// save to rx buffer
	}
}


U8 Uart3ByteReceived(void)
{
	return (RxRec != RxRead);
}

U8 Uart3StringReceived(void)
{
	return Enter;
}

U8 Uart3GetByte(void)
{
	U8 temp = RxBuff[RxRead++];
	if (temp == CR)					// Enter received
		Enter--;
	return temp;
}

void Uart3SendByte(char data)
{
	TxBuff[TxWrite++] = data;
	if ((_U3TXIE == 0) && (TxWrite != TxSent))
	{
		_U3TXIE		= 1;				// Enable Transmit Interrupt
		_U3TXIF		= 1;				// Set Transmit Interrupt Flag
	}
}

void Uart3GetString(P_U8 str)
{
	if (Enter)
	{
		while (1)
		{
			U8 data = Uart3GetByte();
			if (data == CR)
				break;
			else if (data != LF)
				*str++ = data;
		}
	}
	*str = '\0';
}
/*
void Uart3SendString(P_R8 str)
{
	while (*str)
		Uart3SendByte(*str++);

	Uart3SendEnter();
}
*/
void Uart3SendText(const char *str)
{
	while (*str)
		Uart3SendByte(*str++);

//	Uart3SendEnter();
}

void Uart3SendData(const U8 *data, U16 len)
{
	while (len)
	{
		Uart3SendByte(*data++);
		len--;
	}

//	Uart3SendEnter();
}

void Uart3SendEnter(void)
{
	Uart3SendByte(CR);
	Uart3SendByte(LF);
}

void Uart3PrintDebugData(const U8 *data, U8 len)
{
//	Uart3SendText("\r\nData: ");
	Uart3SendEnter();
	char str[5];
	while(len)
	{
		sprintf(str,"%02X ", *data++);		Uart3SendText(str);
		len--;
	}
//	Uart3SendEnter();
}

// return number of remain empty bytes in transmit buffer
U8 Uart3TxBuffResidual(void)
{
	return (TxWrite < TxSent)?		(TxSent - TxWrite):((255 - TxWrite) + TxSent);
}
