#include	"Tcp.h"
#include	"Adc.h"
#include	"Gsm.h"
#include	"Wifi.h"
#include	"Ports.h"
#include	"Memory.h"
#include	"Timers.h"
#include	<string.h>
#include	<stdio.h>
  #include		"Debug.h"
    char tmp[64];

#define	OUT_PACKET_COUNT	8
#define	MEMORY_EMPTY_CHAR	0xFF	// for tests - change to 0xFF in future
//#define	DEVICE_ID			1	// TODO Make changable and save to memory
#define	FL_INIT				TcpFlags.flag_0
#define	FL_WIFI_GPRS		TcpFlags.flag_1
#define	FL_ONLINE_MAX_SEC	2

		U16				OnlineTimer		= 0, OnlineTimeoutSec	= FL_ONLINE_MAX_SEC;
static	U16				MemSendSize		= 0;
static	TCP_PACKET		OutPackets[OUT_PACKET_COUNT];
//static TCP_PACKET*	OutPacket;
static	U8				OutPacketIdx	= 0;
//static U16			PacketNbr		= 0;
//static UNI32			Address			= {.u32 = 0};
static	FLAGS			TcpFlags		= {.word = 0};
//static UNI16			DeviceId		= {.u16 = DataFromFlash.DeviceId};
//static U16			MaxPackets		= 0x1000;				// Temporary
static	U16				MemDataSent 	= 0;
static	U16				ResendPacket	= 0;

static enum State
{
	State_Idle = 0,
	State_BootFlash,
	State_MemStatus,
	State_MemBootAddr,
	State_MemWrite,
	State_MemRead,

	State_Uncnown
} TcpBootState = State_Idle;

U8			Tcp_GetPacketBuffer		(void);
TCP_PACKET* Tcp_GetOutPacketBuffer	(void);
//U8			Tcp_AvailableOutBuff	(void);

void Tcp_Init	(void)
{
	register UNI32 wAddr;
	wAddr.u32 = Mem_GetWriteAddress();

	if (wAddr.u8ll)
	{
		wAddr.u8ll = 0;
		wAddr.u32 += 0x100;
		Mem_SetWriteAddress(wAddr.u32);
	}
//	Mem_SetWriteAddress(0);
	memset((char*)&OutPackets, 0, sizeof(OutPackets));
	FL_INIT = 1;
}

U8 Tcp_GetPacketBuffer	(void)
{
	register U16 i;
	for (i = 0; i <= OUT_PACKET_COUNT; i++)
	{
		if (++OutPacketIdx >= OUT_PACKET_COUNT)
			OutPacketIdx = 0;
		if (!OutPackets[OutPacketIdx].start)
			break;
	}
//	TCP_PACKET* packet = &OutPackets[OutPacketIdx];	// point to next packet buffer
	memset(OutPackets[OutPacketIdx].bytes, 0xFF, TCP_PACKET_LEN);
	return OutPacketIdx;
}

TCP_PACKET* Tcp_GetOutPacketBuffer	(void)
{
	register U16 i;
	for (i = 0; i <= OUT_PACKET_COUNT; i++)
	{
		if (++OutPacketIdx >= OUT_PACKET_COUNT)
			OutPacketIdx = 0;
		if (!OutPackets[OutPacketIdx].start)
			break;
	}
	memset(OutPackets[OutPacketIdx].bytes, 0xFF, TCP_PACKET_LEN);
	return &OutPackets[OutPacketIdx];
}

U8	Tcp_AvailableOutBuff	(void)
{
	register U8 i, ret = 0;
	for (i = 0; i < OUT_PACKET_COUNT; i++)
	{
		if (!OutPackets[i].start)
			ret++;
	}
	return ret;
}

// return index of packets buffers
U8	Tcp_StartMemPacket (U8 type)
{
	if (!FL_INIT)
		Tcp_Init();
	UNI32 addr = {.u32 = Mem_GetWriteAddress()};
	register U16 i;

	if (addr.u8ll)
	{
/*		while (addr.u8ll)
		{
			Mem_WriteByte_1(0xFF);			// 
			addr.u8ll++;
		}
*/
		addr.u8ll = 0;
		addr.u32 += 0x100;
//		Mem_SetWriteAddress(addr.u32);
//		addr.u32 = Mem_GetWriteAddress();
	}
/*
	for (i = 0; i <= OUT_PACKET_COUNT; i++)
	{
		if (++OutPacketIdx >= OUT_PACKET_COUNT)
			OutPacketIdx = 0;
		if (!OutPackets[OutPacketIdx].start)
			break;
	}
//	register TCP_PACKET* packet = &OutPackets[OutPacketIdx];	// point to next packet buffer
	register TCP_PACKET* packet = OutPackets[OutPacketIdx];	// point to next packet buffer
	memset(packet->bytes, 0xFF, TCP_DATA_LEN);
*/
	register U8 idx = Tcp_GetPacketBuffer();
	register TCP_PACKET* packet = &OutPackets[idx];	// point to next packet buffer

//	packet->start	= STX;
	packet->len		= 0;
//	packet->crc		= 0;
	packet->type	= type;
	packet->id		= DataFromFlash.DeviceId;
//	packet->nr		= PacketNbr;
	packet->bytes[6]	= addr.u8lh;		// aa nn NN aa	Packet number = address >> 8
	packet->bytes[7]	= addr.u8hl;		// aa NN nn aa	(little endian)
											// tcp-> STX Len CRC Type ID_ID AD_AD
	Mem_WriteByte_1(STX);					//        |   '   ' /   /  /  /  /
	for (i = 3; i < TCP_PACKET_HEDER; i++)	//        |      /    /  /  /  /
		Mem_WriteByte_1(packet->bytes[i]);	// mem-> STX Type  ID_ID  AD_AD

//	return OutPacketIdx;
	return idx;
}

// Return remain size of packet
U8 Tcp_WriteMemPacket(U8 nr, U8* data, U8 len)
{
	register TCP_PACKET* packet = &OutPackets[nr];	// necessary packet buffer
	register U16	start	= packet->len;			// from previous point
	register U16	end		= start + len;			// till required
	if (end > TCP_DATA_LEN)							// but not exceeding packet max data size
		end = TCP_DATA_LEN;

//	Mem_SetWriteAddress(addr);
//	for (; start < end; start++)
	while (start < end)
	{
		Mem_WriteByte_1(*data);						// copy to memory
		packet->data[start++] = *data++;			// copy to packet buffer
	}
	packet->len = start;							// save reached size
	return (TCP_DATA_LEN - start);
}

void Tcp_SendPacket(U8 nr)
{
	register TCP_PACKET* packet = &OutPackets[nr];	// necessary packet buffer
	register U8		crc = 0;
	register U16	i, end	= (U16)packet->len + TCP_PACKET_HEDER;
	for (i = 3; i < end; i++)						// from packet type till end of data
		crc ^= packet->bytes[i];
//	packet->len		= end;							//		LEN_CHANGES
	packet->crc		= crc;
	packet->start	= STX;

	for (; i < TCP_PACKET_LEN; i++)
		Mem_WriteByte_1(MEMORY_EMPTY_CHAR);			// 
	Mem_WriteByte_1(packet->len);					// write in memory saved packet len
	Mem_WriteByte_1(crc);							// & calculated crc

	FL_TCP_READY = 1;
}

// Feel packet heder:
// STX
// len	(for known types of packet)
// crc	(calculate)
// type	(if type parameter == 0 -> no owerwrite existing)
// ID
void Tcp_PreparePacket	(TCP_PACKET* pack, U8 type)
{
	pack->id = DataFromFlash.DeviceId;	// id
	if (type)
		pack->type	= type;				// type

	switch (pack->type)					//
	{
	case TYPE_OK_GPRS:
	case TYPE_OK_WIFI:		pack->dat_16[0] = Power;
							pack->dat_16[1] = Battery;
							pack->len = 4;				break;	//		LEN_CHANGES
	default:			if (pack->len > TCP_DATA_LEN)			//		LEN_CHANGES
							pack->len = TCP_DATA_LEN;	break;	// 		LEN_CHANGES
	}
	pack->crc = 0;						// crc
	register U16 i, end	= (TCP_PACKET_HEDER + pack->len);		//		LEN_CHANGES
	for (i = 3; i < end; i++)									//		LEN_CHANGES
		pack->crc ^= pack->bytes[i];	// calculate crc
	// STX must be writed as last one,
	// because if packet in OutPackets buffer - now it's VISIBLE TO SEND
	pack->start = STX;					// start
}

TCP_PACKET* Tcp_GetUnsent	(void)
{
	register TCP_PACKET* pack = 0;
	register U16 i;
	for (i = 0; i < OUT_PACKET_COUNT; i++)
	{
		if (OutPackets[i].start == STX)
		{
			if (!pack)
				pack = &OutPackets[i];
			else if (OutPackets[i].nr < pack->nr)
				pack = &OutPackets[i];
		}
	}
	return pack;
}

inline void Tcp_PacketReceived	(TCP_PACKET* PacketRec)
{
	register TCP_PACKET* outPack = NULL;

//	DebugSprintf(tmp,"Packet received: %02X", PacketRec->type);
//	DebugPrint(tmp);

	switch (PacketRec->type)
	{
	case TYPE_GPS_DATA:		{
		if (!FL_TCP_RESEND)
		{
			if (ResendPacket != PacketRec->nr)
			{
				ResendPacket = PacketRec->nr;
				FL_TCP_RESEND = 1;
			}
			else
				ResendPacket = 0;
		}
		if (!SysTime.year)
			SetTcpTime((U32*)&PacketRec->data[0]);
		break;				}
	case TYPE_CURRENT_DATA:	{
		OnlineTimeoutSec	= FL_ONLINE_MAX_SEC;
		OnlineTimer			= (FL_ONLINE_MAX_SEC + 1);
		FL_WEB_ONLINE	= 1;
		break;				}
	case TYPE_ONLINE:		{
		OnlineTimer				= *(U16*)&PacketRec->data[0];
//		OnlineTimer				= (PacketRec->len > TCP_PACKET_HEDER)? *(U16*)&PacketRec->data[0] : 0;
		if (PacketRec->len > (TCP_PACKET_HEDER + 2))
			OnlineTimeoutSec	= *(U16*)&PacketRec->data[2];
		FL_WEB_ONLINE			= (OnlineTimer)?	1 : 0;
//		DebugSprintf(tmp,"Online flag: %d, timer: %d, timeout %d", FL_WEB_ONLINE, OnlineTimer, OnlineTimeoutSec);
//		DebugPrint(tmp);
		break;				}
	case TYPE_RESET:		{	// Reset
		if (PacketRec->id == 0xA55A)
		{
			__asm__ ("RESET");
			while(1);
		}
		break;				}

	case TYPE_FLASH:		{	TcpBootState = State_BootFlash;		FL_BOOT_TCP = 1;	break;	}
	case TYPE_MEM_STATUS:	{	TcpBootState = State_MemStatus;		FL_BOOT_TCP = 1;	break;	}
	case TYPE_MEM_BOOT_ADDR:{	TcpBootState = State_MemBootAddr;	FL_BOOT_TCP = 1;	break;	}
	case TYPE_MEM_WRITE:	{
		register U8 i;											//		LEN_CHANGES
		for (i = 0; i < PacketRec->len; i++)					//		LEN_CHANGES
		{	while (Mem_WriteByte(PacketRec->data[i]) < 0);	}	//		LEN_CHANGES
								TcpBootState = State_MemWrite;		FL_BOOT_TCP = 1;	break;	}
	case TYPE_MEM_READ:		{
		FL_WIFI_GPRS	= FL_WIFI_OFFLINE;
		if (PacketRec->nr)
		{
			Mem_SetReadAddress (*(U32*)&PacketRec->data[0]);
			MemSendSize		=	*(U16*)&PacketRec->data[4];
			MemDataSent		= 0;
		}						TcpBootState = State_MemRead;		FL_BOOT_TCP = 1;	break;	}
	case TYPE_DEV_SW_INFO:	{	TcpBootState = State_Idle;			FL_BOOT_TCP	= 1;	break;	}
	case TYPE_GSM_SETUP:	{	Gsm_Setup();											break;	}

	default:				{
//		outPack = Tcp_GetOutPacketBuffer();
//		outPack->len = TCP_PACKET_HEDER;
//		DebugPrint("Uncnown Packet Type");
		break;				}
	}
	if (outPack)
	{
		Tcp_PreparePacket(outPack, 0);
		FL_TCP_READY = 1;
	}
}

void Tcp_SendText (char* str)
{
	register TCP_PACKET* outPack = Tcp_GetOutPacketBuffer();
	register U8 i = 0;
	while (i < TCP_DATA_LEN)
	{
		register char a = *str++;
		outPack->data[i++] = (U8)a;
		if (a == '\0')
			break;
	}
	outPack->len = i;

	Tcp_PreparePacket(outPack, TYPE_TEXT);
	FL_TCP_READY = 1;
}

extern void BootFlashInit	(void);

void Tcp_BootLoaderProcess	(void)
{
    static U16 waitCon = 0;

	if ((!Wifi_Connected()) &&	(!GprsConnected()))
		return;

//	DebugSprintf(tmp,"BootLoaderProcess -TcpBootState = %d", TcpBootState);
//	DebugPrint(tmp);

	register TCP_PACKET* outPack = NULL;

	switch (TcpBootState)
	{
	case State_Idle:												{
//	DebugSprintf(tmp,"State_Idle");
//	DebugPrint(tmp);
		outPack = Tcp_GetOutPacketBuffer();
//	DebugSprintf(tmp,"outPack");
//	DebugPrint(tmp);
		sprintf((char*)outPack->data, "%s %s %s %s \r", DEVICE, VERSION, Build_Date, Build_Time);
		outPack->len = 0;										//		LEN_CHANGES
		outPack->type = TYPE_DEV_SW_INFO;
		register U8* ptr = outPack->data;
		while (*ptr++ != '\r')
			outPack->len++;
		FL_BOOT_TCP = 0;
//	DebugSprintf(tmp,"pack created");
//	DebugPrint(tmp);
		break;														}
	case State_BootFlash:											{
		BootFlashInit();
		outPack			= Tcp_GetOutPacketBuffer();
		outPack->type	= TYPE_FLASH;
		outPack->len	= 0;									//		LEN_CHANGES
		TcpBootState	= State_Idle;
		FL_BOOT_TCP		= 0;
		break;														}
	case State_MemStatus:											{
		outPack			= Tcp_GetOutPacketBuffer();
		outPack->type	= TYPE_MEM_STATUS;
		outPack->len	= 0;									//		LEN_CHANGES
		outPack->nr		= MemStatus();
		TcpBootState	= State_Idle;
		FL_BOOT_TCP		= 0;
		break;														}
	case State_MemBootAddr:											{
//	DebugSprintf(tmp,"State_MemBootAddr");
//	DebugPrint(tmp);
		#ifdef GPS_IE			// if defined GPS interrupt
			GPS_IE = 0;			// switch it off to avoid memory write
		#endif
//		FL_BOOT = 1;			// Boot process should stop all other processes
		_AD1IE	= 0;			// Disable ADC1 Interrupt
//	DebugSprintf(tmp,"Mem_SetWriteAddress");
//	DebugPrint(tmp);

		Mem_SetWriteAddress(0);
//	DebugSprintf(tmp,"Send response");
//	DebugPrint(tmp);
		outPack			= Tcp_GetOutPacketBuffer();
		outPack->type	= TYPE_MEM_BOOT_ADDR;
		*(U32*)&outPack->data[0] = Mem_GetWriteAddress();
		*(U16*)&outPack->data[4] = Mem_GetType();
		outPack->len	= (sizeof(U32) + sizeof(U16));			//		LEN_CHANGES
		TcpBootState	= State_Idle;
		FL_BOOT_TCP		= 0;
		break;														}
	case State_MemWrite:											{
		if (!MemStatus())
		{
			outPack			= Tcp_GetOutPacketBuffer();
			outPack->type	= TYPE_MEM_WRITE;
			outPack->len	= 0;								//		LEN_CHANGES
			TcpBootState	= State_Idle;
			FL_BOOT_TCP		= 0;
		}
		break;														}
	case State_MemRead:												{
		if (FL_WIFI_GPRS != FL_WIFI_OFFLINE)
		{
			if (!waitCon)
                waitCon = 0xFFFF;
            else if (!(--waitCon))
            {
                MemDataSent		= 0;
                MemSendSize		= 0;
                TcpBootState	= State_Idle;
                FL_BOOT_TCP		= 0;
                Tcp_SendText("Wifi offline Error");
            }
			return;
		}
        waitCon = 0;
		register UNI16 len	= {.u16 = (MemSendSize - MemDataSent)};
		if (len.u16)
		{
			outPack			= Tcp_GetOutPacketBuffer();
			outPack->type	= TYPE_MEM_READ;
			outPack->nr		= MemDataSent;
//			if (len.u16 >= TCP_DATA_LEN)
//				len.u16 = (TCP_DATA_LEN - 1);
			if (len.u16 > 246)
				len.u16 = 246;
			register U8 i;
			for (i = 0; i < len.u8l; i++)
			{	while (Mem_ReadByte(&outPack->data[i]) < 0);	}
			outPack->len	= i;									//		LEN_CHANGES
			MemDataSent	   += len.u16;
		}
		else
		{
			MemDataSent		= 0;
			MemSendSize		= 0;
			TcpBootState	= State_Idle;
		}
		FL_BOOT_TCP		= 0;
		break;														}
	default:														{
		TcpBootState	= State_Idle;
		FL_BOOT_TCP		= 0;
		break;														}
	}
	if (outPack)
	{
//	DebugSprintf(tmp,"Prepare packet");
//	DebugPrint(tmp);
		Tcp_PreparePacket(outPack, 0);
//	DebugSprintf(tmp,"Packet READY");
//	DebugPrint(tmp);
		FL_TCP_READY = 1;
	}
}

void	Tcp_PacketResend	(void)
{
	if (!FL_INIT)
		Tcp_Init();

//	Tcp_SendText("Resend Packet");
	Mem_SetReadAddress((U32)(ResendPacket) << 8);			//        0   1    2   3  4  5  6  7  .   254 255
	TCP_PACKET* outPack = Tcp_GetOutPacketBuffer();

	U8 i;	// Copy header bytes from memory (Len & crc skipped)      0   1    2   3  4  5  6  7  .   254 255
	for (i = 2; i < TCP_PACKET_HEDER; i++)					// mem-> STX Type  ID_ID  AD_AD .  .  .   len crc |
	{	while (Mem_ReadByte(&outPack->bytes[i]) < 0);	}	//        |  \   \   \  \  \  \  \  \  \          |
	for (i = 0; i < TCP_DATA_LEN; i++)						//        |    \   \   \  \  \  \  \  \  \        |
	{		// Copy data bytes, count recorded				//        |      \   \   \  \  \  \  \  \  \      |
		while (Mem_ReadByte(&outPack->data[i]) < 0);		// tcp-> STX Len CRC Type ID_ID AD_AD  .  .  .    |
		if (outPack->data[i] != MEMORY_EMPTY_CHAR)			//        0   1   2   3   4   5  6  7  8  9  .
			outPack->len = i;
	}
/*	if (pack->crc != STX)						// TODO		// Verify STX in memory (shifted to CRC by copy)
	{}														// TODO
	if (pack->type != TYPE_GPS_DATA)						// Verify Tcp Packet Type
	{}														// TODO
	if (pack->id != DataFromFlash.DeviceId)					// Verify Device ID in packet
	{}														// TODO
	if (pack->nr != ResendPacket)							// Verify Packet number
	{}														// TODO	*/
//	while (Mem_ReadByte(&outPack->len) < 0);
	while (Mem_ReadByte(&i) < 0);							// recorded len in memory
	if ((outPack->len != i) && (i <= TCP_DATA_LEN))			// if len in mem > counted & it's correct
		outPack->len = i;									// use len from memory
//	while (Mem_ReadByte(&outPack->crc) < 0);		
//	outPack->start	= STX;
	if (outPack->type < TYPE_CURRENT_DATA)					// TODO		// Check also other types in future
	{
		Tcp_PreparePacket(outPack, 0);
		FL_TCP_READY	= 1;
	}
	FL_TCP_RESEND	= 0;
}
