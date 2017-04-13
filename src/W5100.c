#include	"Ports.h"
#include	"W51_def.h"
#include	"W5100.h"
#include	"Spi_i2c.h"
#include	"Timers.h"
#include	"Tcp.h"
#include	<string.h>

#include		"Lcd.h"
#include	<stdio.h>

#ifdef	ETH_W5100

const	U16				TCP_SERVER_PORT	=	6000;
const	U16				UDP_SERVER_PORT	=	6001;

static	W5100_STATE	    W5100State = W5100_UNCNOWN;
static	W5100_SOCKET	Sockets[SOCKET_COUNT];
//static	S8				ServerTcp = -1, ServerUdp = -1;

	void	W51_Reset			(void);
inline	U8	W51_SpiOpCode		(register U8 opCode, UNI16 addr, register U8 data);
	U8		W51_WriteByte		(U16 addr, register U8 byte);
	U8		W51_ReadByte		(U16 addr);
	U8		W51_WriteWord		(U16 addr, U16 word);
	U16		W51_ReadWord		(U16 addr);
    U8		W51_WriteData   	(U16 addr, const U8* data, U16 len);
	void	W51_ReadData		(U16 addr, U8* data, U16 len);
	void	W51_InitDevice		(void);
	S8		W51_AllocateSocket	(void);
	void	W51_SocketOpen		(S8 socket, U8 protocol, const U8* ip, U16 port);
	void	W51_SocketClose		(S8 socket);
	void	W51_SocketAbort		(S8 socket);
	U16		W51_DataReceived	(S8 socket, U16 len);
	void	W51_RecDataSort		(U8* ptr, U16 len, S8 socket);
	S16		W51_SendTcp			(const U8* buf, U16 len, S8 socket);
	S16		W51_SendUdp			(const U8* buf, UDP_HEADER* header);

void	W51_Reset		(void)
{
	W51_RST = 0;
	DELAY_2uS();
	DELAY_2uS();
	W51_RST = 1;
}

inline U8	W51_SpiOpCode (register U8 opCode, UNI16 addr, register U8 data)
{
	register U8 i = 4, rc;
	W51_SCS_ON();
	while (!SPI3_RX_FIFO_EMPTY)
		rc = SPI3BUF;
	SPI3BUF = opCode;
	SPI3BUF = addr.u8h;
	SPI3BUF = addr.u8l;
	SPI3BUF = data;
	while (i--)
	{
		while (SPI3_RX_FIFO_EMPTY);
		rc = SPI3BUF;
	}
	W51_SCS_OFF();
	return rc;
}

U8		W51_WriteByte	(U16 addr, register U8 byte)
{
	return (W51_SpiOpCode(OP_CODE_WRITE, (UNI16)addr, byte) == 0x03);
}

U8		W51_ReadByte	(U16 addr)
{
	return W51_SpiOpCode(OP_CODE_READ, (UNI16)addr, 0x00);
}

U8		W51_WriteWord	(U16 addr, U16 word)
{
	UNI16* data = (UNI16*)&word;
	register U8		rc = W51_SpiOpCode(OP_CODE_WRITE, (UNI16)addr, data->u8h);
	addr++;
	if (rc == 0x03)	rc = W51_SpiOpCode(OP_CODE_WRITE, (UNI16)addr, data->u8l);
	return (rc == 0x03);
}

U16		W51_ReadWord	(U16 addr)
{
	register UNI16 rc;
	rc.u8h = W51_SpiOpCode(OP_CODE_READ, (UNI16)addr, 0x00);
	addr++;
	rc.u8l = W51_SpiOpCode(OP_CODE_READ, (UNI16)addr, 0x00);
	return rc.u16;
}

U8		W51_WriteData	(U16 addr, const U8* data, U16 len)
{
	register U16 i;
	register U8  rc = 0;
	for (i = 0; i < len; i++)
	{
		rc = W51_SpiOpCode(OP_CODE_WRITE, (UNI16)addr, *data++);
		if (rc != 0x03)
			break;
		addr++;
	}
	return (rc == 0x03);
}

void	W51_ReadData	(U16 addr, U8* data, U16 len)
{
	register U16 i;
	for (i = 0; i < len; i++)
	{
		*data++ = W51_SpiOpCode(OP_CODE_READ, (UNI16)addr, 0x00);
		addr++;
	}
}


void	W51_InitDevice	(void)
{
	W5100State = W5100_DISCONNECTED;
	register U8 retry = 10;
	Spi3_Init();
	while (retry)
	{
		if (W51_WriteByte(MR, MODE_RESET))	break;		// Reset W5100 device
		if (!--retry)						return;
	}
	memset (Sockets, 0, sizeof(Sockets));
	W5100State = W5100_INIT_REQUIRED;

	while (!W51_WriteByte(MR, 0x00));					// wait for initialisation

	W51_WriteByte(IMR, 0);								// TODO: configure IRQ Mask Register
	W51_WriteWord(RTR, (RETRY_TIME_MS * 10));			// Retry Time-value 1 = 100 uS
	W51_WriteByte(RCR, RETRY_COUNT);					// Retry Count
	// Setting network information
	W51_WriteData(GAR,	DataFromFlash.GatewayIp,	4);	// Gateway Address
	W51_WriteData(SHAR,	DataFromFlash.MacAddress,	6);	// Mac Address
	W51_WriteData(SUBR,	DataFromFlash.SubnetMask,	4);	// Subnet Mask
	W51_WriteData(SIPR,	DataFromFlash.MyIp,			4);	// Source IP

	W51_WriteByte(RMSR, RX_MEMORY_SHARE);				// assign 2K rx memory per socket
	W51_WriteByte(TMSR, TX_MEMORY_SHARE);				// assign 2K tx memory per socket

	W5100State = W5100_ACTIVE;
}

#if (TEST == TEST_W5100)
void	W51_SpiTestCode (UNI16 addr)
{
	register U8 i = 0;
	U8 data[4];
	char str[40];

	W51_SCS_ON();
	while (!SPI3_RX_FIFO_EMPTY)
		i = SPI3BUF;
	SPI3BUF = OP_CODE_READ;
	SPI3BUF = addr.u8h;
	SPI3BUF = addr.u8l;
	SPI3BUF = 0;
	for (i = 0; i < 4; i++)
	{
		while (SPI3_RX_FIFO_EMPTY);
		data[i] = SPI3BUF;
	}
	W51_SCS_OFF();

	sprintf(str, "TEST = %02X,%02X,%02X,%02X ", data[0], data[1], data[2], data[3]);
	LcdString(7, 0, str, font_7x5);
}

void	W51_Test	(void)
{
	static U8 test = 1;
	char str[40];

	if (test)
	{
		test = 0;
        sprintf(str, "State: %d", W5100State);
        LcdString(0, 0, str, font_7x5);
		
		register U16 t16 = W51_ReadWord(RTR);
        sprintf(str, "RTR:   %d", t16);
        LcdString(1, 0, str, font_7x5);

		register U8 t8 = W51_ReadByte(RCR);
        sprintf(str, "RCR:   %d", t8);
        LcdString(2, 0, str, font_7x5);

		t8 = W51_ReadByte(RMSR);
        sprintf(str, "RMSR:  %02X", t8);
        LcdString(3, 0, str, font_7x5);

		t8 = W51_ReadByte(TMSR);
		sprintf(str, "TMSR:  %02X", t8);
		LcdString(4, 0, str, font_7x5);

		W51_SpiTestCode((UNI16)TMSR);
	}
	sprintf(str, "Receive: %d, %d, %d, %d", Sockets[0].dataInReceiveBuffer, Sockets[1].dataInReceiveBuffer, Sockets[2].dataInReceiveBuffer, Sockets[3].dataInReceiveBuffer);
	LcdString(5, 0, str, font_7x5);
	sprintf(str, "Sockets: %d, %d, %d, %d", Sockets[0].state, Sockets[1].state, Sockets[2].state, Sockets[3].state);
	LcdString(6, 0, str, font_7x5);
	sprintf(str, "Status: %02X %02X %02X %02X", Sockets[0].status, Sockets[1].status, Sockets[2].status, Sockets[3].status);
	LcdString(7, 0, str, font_7x5);
}
#endif

/*
void __attribute__((interrupt, no_auto_psv)) _SPI3Interrupt(void)
{
	_SPI3IF	= 0;				// Clear SPI3 Interrupt Flag
	
	register U8 data;
	SPI3STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
}
*/

 W5100_STATE	W51_GetEthDeviceState	(void)
 {
	 return W5100State;
 }
 
S8		W51_AllocateSocket	(void)
{
	register S8 socket;
	for (socket = 0; socket < SOCKET_COUNT; socket++)
	{
		if (Sockets[socket].state == SOCKET_NOT_ALLOCATED)
		{
			memset (Sockets[socket].ip.u8, 0, sizeof(W5100_SOCKET));
			W51_SocketClose(socket);
			Sockets[socket].state	= SOCKET_CLOSED;
			if	(	(socket == SOCKET_TCP_SERVER) ||
					(socket == SOCKET_TCP_SERVER_2)	)
			{
				Sockets[socket].protocol	= S_MODE_TCP;
				Sockets[socket].port		= TCP_SERVER_PORT;
				Sockets[socket].autoReconnect = 1;
			}
			else if	(socket == SOCKET_UDP_SERVER)
			{
				Sockets[socket].protocol	= S_MODE_UDP;
				Sockets[socket].port		= UDP_SERVER_PORT;
				Sockets[socket].autoReconnect = 1;
			}
			return socket;
		}
	}
	W5100State = W5100_ERROR;
	return -1;
}

void	W51_SocketOpen	(S8 socket, U8 protocol, const U8* ip, U16 port)
{
	if (W51_ReadByte(Sn_SR(socket)) != S_STATUS_CLOSED)
		W51_SocketAbort(socket);
	if ((protocol == S_MODE_TCP) &&
		((*(U32*)ip == 0) || (Sockets[socket].activityTimeoutMs == 0)))
		Sockets[socket].autoReconnect = 1;

	Sockets[socket].ip.u32			= *(U32*)ip;
	Sockets[socket].port			= port;
	Sockets[socket].protocol		= protocol;
	Sockets[socket].delayStartMs	= GetTicsMs();
	Sockets[socket].state			= SOCKET_INIT;
}

void	W51_SocketClose	(S8 socket)
{
	if (W5100State < W5100_ACTIVE)
		return;

	if (Sockets[socket].state == SOCKET_CONNECTED)
	{
		W51_WriteByte(Sn_CR(socket), CMD_DISCON);
		Sockets[socket].state = SOCKET_CLOSING;
	}
	else
	{
		W51_WriteByte(Sn_CR(socket), CMD_CLOSE);
		Sockets[socket].state = SOCKET_CLOSED;
	}
}

void	W51_SocketAbort	(S8 socket)
{
	if (W5100State < W5100_ACTIVE)
		return;

	W51_WriteByte(Sn_CR(socket), CMD_CLOSE);
	while (W51_ReadByte(Sn_CR(socket)));
}

void	W51_Run		(void)
{
	if		(W5100State  < W5100_ACTIVE)		return	W51_InitDevice();
	else if	(W5100State  > W5100_ACTIVE)		return	Reset();	// W5100_ERROR

	register S8 socket;
	for (socket = 0; socket < SOCKET_COUNT; socket++)
	{
		#if (TEST == TEST_W5100)
			Sockets[socket].status = W51_ReadByte(Sn_SR(socket));
		#endif
		if (Sockets[socket].state == SOCKET_NOT_ALLOCATED)
		{
			if (socket <= SOCKET_TCP_SERVER_2)	// Socket 0 - always must be allocated as TCP Server
				W51_AllocateSocket();			// Socket 1 - always must be allocated as UDP Server
			continue;
		}

		register U16 reclen = W51_ReadWord (Sn_RX_RSR(socket));
		if (reclen)
			W51_DataReceived(socket, reclen);

		switch (W51_ReadByte(Sn_SR(socket)))
		{
		case S_STATUS_ESTABLISHED:						{
			if (Sockets[socket].state != SOCKET_CONNECTED)
			{
				if ((Sockets[socket].state == SOCKET_LISTEN) ||			// New incoming connection
					(Sockets[socket].state == SOCKET_WAIT_CONNECTION) )
				{
					W51_SendTcp((U8*)SERVER_WELCOME_MSG, sizeof(SERVER_WELCOME_MSG), socket);
//					char welcome[128];
//					sprintf (welcome, "%s state: %d", SERVER_WELCOME_MSG, Sockets[socket].state);
//					W51_SendTcp((U8*)welcome, strlen(welcome), socket);
				}

				Sockets[socket].state  = SOCKET_CONNECTED;
				Sockets[socket].delayStartMs = GetTicsMs();
			}
			else if ((Sockets[socket].activityTimeoutMs != 0) &&
					 (GetTimeSinceMs(Sockets[socket].delayStartMs) > Sockets[socket].activityTimeoutMs) )
				W51_SocketClose (socket);
			break;										}
		case S_STATUS_LISTEN:							{
			if (Sockets[socket].state  == SOCKET_WAIT_CONNECTION)
				Sockets[socket].state	= SOCKET_LISTEN;
			else if (Sockets[socket].state != SOCKET_LISTEN)
			{
				W51_SocketAbort (socket);
				Sockets[socket].state	= SOCKET_INIT;
			}
			break;										}
		case S_STATUS_UDP:								{		// TODO
			if (Sockets[socket].state == SOCKET_MUST_CONNECT)
				Sockets[socket].state	= SOCKET_LISTEN;
			else if (Sockets[socket].state != SOCKET_LISTEN)
			{
				W51_SocketAbort (socket);
				Sockets[socket].state	= SOCKET_INIT;
			}
			break;										}
		case S_STATUS_CLOSED:							{
			if (Sockets[socket].state == SOCKET_INIT)
			{
				W51_WriteByte(Sn_MR(socket),	Sockets[socket].protocol);	// set TCP or UDP mode for socket
				W51_WriteWord(Sn_PORT(socket),	Sockets[socket].port);		// 
				W51_WriteByte(Sn_CR(socket),	CMD_OPEN);
				Sockets[socket].state			= SOCKET_MUST_CONNECT;
				Sockets[socket].delayStartMs	= GetTicsMs();
			}
			else if (Sockets[socket].state == SOCKET_MUST_CONNECT)
			{
				if (GetTimeSinceMs(Sockets[socket].delayStartMs) > SOCKET_INIT_DELAY)
				{
					W51_SocketAbort (socket);
					Sockets[socket].state = SOCKET_INIT;
				}
			}
			else if (Sockets[socket].autoReconnect)
				Sockets[socket].state = SOCKET_INIT;
			else
				Sockets[socket].state = SOCKET_NOT_ALLOCATED;				// SOCKET_CLOSED

			break;										}
		case S_STATUS_INIT:								{
			if (Sockets[socket].state == SOCKET_MUST_CONNECT)
			{
				if (Sockets[socket].ip.u32 != 0)										// TCP Client mode
				{
					W51_WriteData(Sn_DIPR(socket),	Sockets[socket].ip.u8, sizeof(IP));	// Destination IP
					W51_WriteWord(Sn_DPORT(socket),	Sockets[socket].port);				// Destination Port
					W51_WriteByte(Sn_CR(socket),	CMD_CONNECT);						// CONNECT Command
				}
				else																	// TCP Server mode
					W51_WriteByte(Sn_CR(socket),	CMD_LISTEN);						// LISTEN Command

				Sockets[socket].delayStartMs	= GetTicsMs();
				Sockets[socket].state			= SOCKET_WAIT_CONNECTION;
			}
			else if (GetTimeSinceMs(Sockets[socket].delayStartMs) > WAIT_CONNECTION_DELAY)
			{
				W51_WriteByte(Sn_CR(socket),	CMD_CLOSE);
				Sockets[socket].state		=	SOCKET_INIT;
			}
			break;										}
		case S_STATUS_CLOSE_WAIT:						{
			W51_SocketAbort (socket);
			break;										}
		case S_STATUS_UNKNOWN:							{
			if (Sockets[socket].state == SOCKET_FATAL_ERROR)
			{
				if (GetTimeSinceMs(Sockets[socket].delayStartMs) > UNKNOWN_DELAY)
				{
					W51_SocketClose (socket);
					W5100State = W5100_ERROR;
				}
			}
			else
			{
				Sockets[socket].delayStartMs	= GetTicsMs();
				Sockets[socket].state			= SOCKET_FATAL_ERROR;
			}
			break;										}
		case S_STATUS_FIN_WAIT:
		case S_STATUS_CLOSING:
		case S_STATUS_TIME_WAIT:
		case S_STATUS_LAST_ACK:							{
			if (Sockets[socket].state != SOCKET_CLOSING)
			{
				Sockets[socket].state  = SOCKET_CLOSING;
				Sockets[socket].delayStartMs = GetTicsMs();
			}
			else if (GetTimeSinceMs(Sockets[socket].delayStartMs) > WAIT_CONNECTION_DELAY)
				W51_SocketAbort (socket);
			break;										}
		case S_STATUS_SYNSENT:
		case S_STATUS_SYNRECV:							{
			if (Sockets[socket].state == SOCKET_LISTEN)
			{
				Sockets[socket].state  = SOCKET_WAIT_CONNECTION;
				Sockets[socket].delayStartMs = GetTicsMs();
				break;
			}
			break;										}
		default:										{
			if (Sockets[socket].state == SOCKET_WAIT_CONNECTION)
			{
				if (GetTimeSinceMs(Sockets[socket].delayStartMs) > WAIT_CONNECTION_DELAY)
				{
					W51_WriteByte(Sn_CR(socket),	CMD_CLOSE);
					Sockets[socket].state		=	SOCKET_INIT;
				}
			}
			else
				W51_SocketAbort (socket);
		break;											}
		}
	}
}

U16		W51_DataReceived (S8 socket, U16 l)
{
	Sockets[socket].dataInReceiveBuffer = l;
//	if (W5100State  < W5100_ACTIVE)
//		return;

	U8	buf[256];
	U16 len = (l > sizeof(buf))?	sizeof(buf) : l;
	U16	ptr		= W51_ReadWord(Sn_RX_RD(socket));
	U16 offset	= ptr & RX_BUFFER_MASK;
	U16	address	= RX_BASE_ADDRESS + (RX_BUFFER_SIZE * socket);

	if (l > sizeof(buf))
	{
		len = sizeof(buf);
//		Sockets[socket].dataInReceiveBuffer = l - len;
	}
	else
	{
		len = l;
//		Sockets[socket].dataInReceiveBuffer = 0;;
	}

	if ((offset + len) > RX_BUFFER_SIZE)
	{
		U16 upperSize = RX_BUFFER_SIZE - offset;
		W51_ReadData((address + offset), buf, upperSize);
		W51_ReadData(address, &buf[upperSize], (len - upperSize));
	}
	else
		W51_ReadData((address + offset), buf, len);

	W51_WriteWord(Sn_RX_RD(socket),	ptr + len);
	W51_WriteByte(Sn_CR(socket),	CMD_RECV);

//	if (len)
		Sockets[socket].delayStartMs =  GetTicsMs();
	W51_RecDataSort(buf, len, socket);

	return len;
}

void	W51_RecDataSort	(U8* ptr, U16 len, S8 socket)
{
	char str[48];
	sprintf (str, "Received%4d in %d soc", len, socket);
//	LcdString (1, 0, str, font_7x5);
	if (len > 40)
		len = 40;
	if (Sockets[socket].protocol == S_MODE_UDP)
	{
		UDP_HEADER* pH = (UDP_HEADER*)ptr;
//		sprintf (str, "%u.%u.%u.%u:%u", pH->ip.u8[0], pH->ip.u8[1], pH->ip.u8[2], pH->ip.u8[3],pH->port);
//		LcdString (2, 0, str, font_7x5);
//		LcdData	(3, 0, (char*)&ptr[sizeof(UDP_HEADER)], (len - sizeof(UDP_HEADER)));
		W51_SendUdp(&ptr[sizeof(UDP_HEADER)], pH);
	}
	else
	{
        U8 rec[48] = {'\n', 'R', 'E', ':', ' '};
//		LcdData	(2, 0, (char*)ptr, len);
        memcpy(&rec[5], ptr, len);
		W51_SendTcp(rec, len + 5, socket);
	}
}

S16		W51_SendTcp	(const U8* buf, U16 len, S8 socket)
{
	if (W5100State < W5100_ACTIVE)
		return -1;

	if (W51_ReadByte(Sn_SR(socket)) != S_STATUS_ESTABLISHED)
		return -1;

	U16 txFreeSize, retry = 500;
	do
	{
		txFreeSize = W51_ReadWord(Sn_TX_FSR(socket));
		if (!--retry)
			return -1;

	} while (txFreeSize != W51_ReadWord(Sn_TX_FSR(socket)));

	if (txFreeSize < len)
		return -1;

	U16	ptr		= W51_ReadWord(Sn_TX_WR(socket));
	U16	offset	= ptr & TX_BUFFER_MASK;
	U16	address	= TX_BASE_ADDRESS + (TX_BUFFER_SIZE * socket);

	if ((offset + len) > TX_BUFFER_SIZE)
	{
		U16 upperSize = TX_BUFFER_SIZE - offset;
		W51_WriteData((address + offset), buf, upperSize);
		W51_WriteData(address, &buf[upperSize], (len - upperSize));
	}
	else
		W51_WriteData((address + offset), buf, len);

	W51_WriteWord(Sn_TX_WR(socket), ptr + len);
	Sockets[socket].delayStartMs = GetTicsMs();

	return W51_WriteByte(Sn_CR(socket), CMD_SEND);
}

S16		W51_SendUdp	(const U8* buf, UDP_HEADER* header)
{
	if (W5100State < W5100_ACTIVE)
		return -1;

	if (W51_ReadByte(Sn_SR(SOCKET_UDP_SERVER)) != S_STATUS_UDP)
	{
		char str[40];
		sprintf(str, "Udp server status: %02X", Sn_SR(SOCKET_UDP_SERVER));
		LcdString(7, 0, str, font_7x5);
		return -1;
	}

	U16 txFreeSize, retry = 500, len = header->len;
	do
	{
		txFreeSize = W51_ReadWord(Sn_TX_FSR(SOCKET_UDP_SERVER));
		if (!--retry)
		{
			char str[40];
			sprintf(str, "Sn_TX_FSR error: %d", W51_ReadWord(Sn_TX_FSR(SOCKET_UDP_SERVER)));
			LcdString(7, 0, str, font_7x5);
			return -1;
		}

	} while (txFreeSize != W51_ReadWord(Sn_TX_FSR(SOCKET_UDP_SERVER)));

	if (txFreeSize < len)
	{
		char str[40];
		sprintf(str, "tx:%d len:%d", txFreeSize, len);
		LcdString(7, 0, str, font_7x5);
		return -1;
	}

//	U8 Ip[4] = {192, 168, 2, 99};
//	U16	po	= 8889;

	W51_WriteData(Sn_DIPR(SOCKET_UDP_SERVER),	header->ip.u8, sizeof(IP));	// Destination IP
//	W51_WriteData(Sn_DIPR(SOCKET_UDP_SERVER),	Ip, sizeof(IP));	// Destination IP
	W51_WriteWord(Sn_DPORT(SOCKET_UDP_SERVER),	header->port);				// Destination Port
//	W51_WriteWord(Sn_DPORT(SOCKET_UDP_SERVER),	po);				// Destination Port

	U16	ptr		= W51_ReadWord(Sn_TX_WR(SOCKET_UDP_SERVER));
	U16	offset	= ptr & TX_BUFFER_MASK;
	U16	address	= TX_BASE_ADDRESS + (TX_BUFFER_SIZE * SOCKET_UDP_SERVER);

	if ((offset + len) > TX_BUFFER_SIZE)
	{
		U16 upperSize = TX_BUFFER_SIZE - offset;
		W51_WriteData((address + offset), buf, upperSize);
		W51_WriteData(address, &buf[upperSize], (len - upperSize));
	}
	else
		W51_WriteData((address + offset), buf, len);

	W51_WriteWord(Sn_TX_WR(SOCKET_UDP_SERVER), ptr + len);
	Sockets[SOCKET_UDP_SERVER].delayStartMs = GetTicsMs();

	{
		char str[40];
//		sprintf(str, "Error in line %d", __LINE__);
		sprintf (str, "%u.%u.%u.%u:%u", header->ip.u8[0], header->ip.u8[1], header->ip.u8[2], header->ip.u8[3],header->port);
		LcdString (7, 0, str, font_7x5);
	}
	W51_WriteByte(Sn_CR(SOCKET_UDP_SERVER), CMD_SEND);
	while (W51_ReadByte(Sn_CR(SOCKET_UDP_SERVER)));
	return 0;
}

void	W51_GetSocketStatus	(char* txt)
{
	sprintf(txt, "s0_%u, s1_%u, s2_%u, s3_%u ",
				  Sockets[0].state, Sockets[1].state, Sockets[2].state, Sockets[3].state);
}

#endif
