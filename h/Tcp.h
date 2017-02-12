#ifndef		_tcp_h_
#define		_tcp_h_

#include	"Typedef.h"
#include	"Tcp_Type.h"

#define	TCP_PACKET_LEN		256
#define	TCP_PACKET_HEDER	8
#define	TCP_DATA_LEN		(TCP_PACKET_LEN - TCP_PACKET_HEDER)
#define	TCP_DATA_LEN_16		(TCP_DATA_LEN / 2)
#define	TCP_DATA_LEN_32		(TCP_DATA_LEN / 4)
#define	TCP_DATA_LEN_BOOT	(TCP_DATA_LEN - 2)
#define	TCP_MEM_DATA_ADDR	6
#define	TCP_MEM_LEN_ADDR	(TCP_PACKET_LEN - 2)
#define	TCP_PACKET_TIMEOUT	40					// 400mS

typedef union
{
	U8 bytes[TCP_PACKET_LEN];
	struct
	{								// ADDRESS in								packet / memory
		char	start;				// start simbol									0	memPack[0]
		U8		len;				// lenght of packet data (w/o heder)			1	memPack[254]
		U8		crc;				// controll summ (from type till end of data)	2	memPack[255]
		U8		type;				// type of TCP packet							3	memPack[1]
		U16		id;					// Device ID (device type + device number)		4	memPack[2]
		U16		nr;					// number of packet								6	memPack[4]
		union
		{
			U8	data[TCP_DATA_LEN];			//	8 bit data							8	memPack[6]
			U16	dat_16[TCP_DATA_LEN_16];	// 16 bit data
			U32	dat_32[TCP_DATA_LEN_32];	// 32 bit data
		};
	};
} TCP_PACKET;

extern	U16		OnlineTimeoutSec, OnlineTimer;

U8			Tcp_StartMemPacket		(U8 type);
U8			Tcp_WriteMemPacket		(U8 nr, U8* data, U8 len);
void		Tcp_SendPacket			(U8 nr);
void		Tcp_PreparePacket		(TCP_PACKET* pack, U8 type);
TCP_PACKET*	Tcp_GetUnsent			(void);
inline void	Tcp_PacketReceived		(TCP_PACKET* PacketRec);
void		Tcp_SendText			(char* str);
void		Tcp_BootLoaderProcess	(void);
void		Tcp_PacketResend		(void);
U8			Tcp_AvailableOutBuff	(void);


#endif	//	_tcp_h_

