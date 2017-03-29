#include	"Memory.h"
#include	"MemDefines.h"
#include	"Spi_i2c.h"
#include	"Timers.h"
#include	"Ports.h"
#include	<string.h>
#include	<stdio.h>
#if	(TEST == TEST_MEMORY)
	#include	"Debug.h"
#else
	#define	DebugPrint(X)
#endif


#define MEM_TIMEOUT_MS		500
#define MEM_BUFF_MASK		0x3FF
#define MEM_BUFF_SIZE		(MEM_BUFF_MASK + 1)
/*
#if (MEMORY == S25FL512S)
	#define	NEW_WRITE_PAGE		(!(CurrentWriteAddress.u16l & 0x1FF))
	#define	NEW_WRITE_SECTOR	(!(CurrentWriteAddress.u32 & 0x0003FFFF))
#else
//	#define	NEW_WRITE_PAGE		(!(CurrentWriteAddress.u8ll))
//	#define	NEW_WRITE_SECTOR	(!(CurrentWriteAddress.u16l))
	#define	NEW_WRITE_PAGE		(!(CurrentWriteAddress.u16l & PageMask))
	#define	NEW_WRITE_SECTOR	((!CurrentWriteAddress.u16l) && (!(CurrentWriteAddress.u16h & SectorMask)))	
#endif
*/

#define	MEM_CS_ON()				MEM_CS = 0; DELAY_2uS()
#define	MEM_CS_OFF()			DELAY_2uS(); MEM_CS	= 1
#define	MEM_CS_IS_ON()			(!MEM_CS)
#if		(MEMORY1_PORT == SPI_1)
	#define	MEM_SPI_INIT			Spi1_Init
	#define	MEM_IRQ					IRQ_SPI_1
	#define	MEM_WRITE_IE			_SPI1IE
	#define	MEM_WRITE_IF			_SPI1IF
	#define	MEM_WRITE_ACTIVE		_SPI1IE				// if Interrupt enabled -> MemWrite = active
	#define	MEM_SPI_BUF_REG			SPI1BUF
	#define	MEM_SPI_RX_FIFO_EMPTY	SPI1_RX_FIFO_EMPTY
	#define	MEM_SPI_CLEAR			Spi1Clear
	#define	MEM_SPI_BYTE			Spi1Byte
	#define	MEM_SPI_TX_FULL			SPI1_TX_FULL
#elif	(MEMORY1_PORT == SPI_2)
	#define	MEM_SPI_INIT			Spi2_Init
	#define	MEM_IRQ					IRQ_SPI_2
	#define	MEM_WRITE_IE			_SPI2IE
	#define	MEM_WRITE_IF			_SPI2IF
	#define	MEM_WRITE_ACTIVE		_SPI2IE				// if Interrupt enabled -> MemWrite = active
	#define	MEM_SPI_BUF_REG			SPI2BUF
	#define	MEM_SPI_RX_FIFO_EMPTY	SPI2_RX_FIFO_EMPTY
	#define	MEM_SPI_CLEAR			Spi2Clear
	#define	MEM_SPI_BYTE			Spi2Byte
	#define	MEM_SPI_TX_FULL			SPI2_TX_FULL
#endif
#define	CurrentWriteAddress		ServiceStruct.WriteAddr
#define	ServiceSector			ServiceAddress.u16h
#define	MEM_FLAGS				MemFlags.word
#define	MEM_FLAG_ERASED			MemFlags.flag_0
#define	MEM_FLAG_WRITE_STARTED	MemFlags.flag_1
#define	MEM_FLAG_WRITE_ACTIVE	MemFlags.flag_2
#define	MEM_FLAG_READ_ACTIVE	MemFlags.flag_3
#define	MEM_FLAG_READ_WAIT		MemFlags.flag_4
#define	MEM_FLAG_SERVICE_WRITE	MemFlags.flag_5
typedef struct					tagMEM_STATUS	{
	union
	{
		U8		byte;
		struct
		{
			U8	WIP		:1;
			U8	WEL		:1;
			U8	BP0		:1;
			U8	BP1		:1;
			U8	BP2		:1;
			U8	E_ERR	:1;
			U8	P_ERR	:1;
			U8	SRWD	:1;
		};
	};
} MEM_STATUS;
typedef	struct					tagSERVICE_STR	{
	U8			clear;
	U8			tmp8;
	UNI16		tmp16;
	UNI32		WriteAddr;
} SERVICE_STR;
typedef	volatile enum			tagMEM_STATE	{
	MemState_Idle		= 0,
	MemState_Status,
	MemState_WriteAddress,
	MemState_WriteData,
	MemState_ReadData,

	MemState_ServiceNull,
	MemState_ServiceSave,
	MemState_ServiceClear,
	MemState_SaveAddress,
	MemState_CsOff,
} MEM_STATE;
static	SERVICE_STR				ServiceStruct;
static	MEM_STATE				MemIrqState;
static	UNI32					CurrentReadAddress, ServiceAddress;
static	UNI32*					pWrAddr;
static	U16						MemTimeOut = 0;
static	FLAGS					MemFlags;
		U8						ManufacturerId = 0, MemoryType = 0, MemoryCapacity = 0;
static	U16						MemType = 0, PageMask = 0, SectorMask = 0;
static	U8						MemBuff[MEM_BUFF_SIZE];
static	U16						MemBuff_in, MemBuff_out;

void MemReadDeviceId		(void);
void MemCurrentAddrInit		(void);
void Mem_ReadStart			(void);
U8	 Mem_ReadStatus			(U8 wait);
void Mem_WriteStatus		(U8 status);
void Mem_WriteEnable		(void);
void Mem_WriteDisable		(void);
//void Mem_WriteStart			(void);
//void Mem_PageErase			(void);
extern U8	BootFlashNew	(void);

inline void SpiMemTimer 	(void)	// 100Hz			TODO...!!!!!!!!!!!!!!!!!!
{
	if ((MemTimeOut) && (!--MemTimeOut))
	{
		MEM_WRITE_IF = 1;
		MEM_WRITE_IE = 1;
	}
/*
#if	(TEST == TEST_MEMORY)
	static U16 testTimer = 3000;
	if (!--testTimer)
	{
		testTimer = 30000;
		DebugPrint("MemTest");
	}
#endif
*/
}

MEM_IRQ
	if		(MemIrqState == MemState_Status)	goto mem_irq_status;
	else if	(MemIrqState == MemState_WriteData)	goto mem_irq_writeData;
	else if	(MemBuff_in != MemBuff_out)			goto mem_irq_start;
	else if (!MemTimeOut)						{
		MemIrqState = MemState_Idle;
		MEM_WRITE_IE		= 0;
		MEM_WRITE_IF		= 1;
		MEM_CS_OFF();
	}
	return;
mem_irq_start:			{
	MemIrqState = MemState_Status;
	MEM_SPI_CLEAR();
	MEM_CS_OFF();
}
mem_irq_status:			{
	register	MEM_STATUS	status;
	while (true)
	{
		if (!MEM_CS_IS_ON())					// ChipSelect = Hi -> spi not started jet
		{
			MEM_SPI_CLEAR();
			MEM_CS_ON();
			MEM_SPI_BYTE(MEM_CMD_RDSR);			// Send Status Register Read command
		}
		status.byte = MEM_SPI_BYTE(0);			// Read status
		if (status.WIP)							// Write in Progress (Memory busy)
		{
			MemTimeOut = 10;					// check after 100mS
			return;
		}
		MEM_CS_OFF();
		if (status.WEL)							// Write enabled - device ready to write data
			goto mem_irq_writeAddress;

		MEM_CS_ON();
		MEM_SPI_BYTE(MEM_CMD_WREN);				// Send Write Enable command
		MEM_CS_OFF();
	}
 }
mem_irq_writeAddress:	{
	MEM_CS_ON();
	if (!pWrAddr->u16l)							// New sector check (must be erased before write)
	{
		if (MEM_FLAG_ERASED)					// sector already erased
			MEM_FLAG_ERASED = 0;
		else if ((MemType < MEM_SECTORS_S25FL512S) || (!(pWrAddr->u16h & SectorMask)))
			goto mem_irq_eraseSector;
	}

	if (pWrAddr->u8hh)
	{
		MEM_SPI_BUF_REG = MEM_CMD_4PP;
		MEM_SPI_BUF_REG = pWrAddr->u8hh;
	}
	else
		MEM_SPI_BUF_REG = MEM_CMD_PP;
	MEM_SPI_BUF_REG = pWrAddr->u8hl;
	MEM_SPI_BUF_REG = pWrAddr->u8lh;
	MEM_SPI_BUF_REG = pWrAddr->u8ll;
	if (MEM_FLAG_SERVICE_WRITE)
		goto mem_irq_serviceSector;
	MemIrqState = MemState_WriteData;
	MEM_SPI_CLEAR();
 }
mem_irq_writeData:		{
	while (!MEM_SPI_TX_FULL)
	{
		if (MemBuff_in == MemBuff_out)
		{
			if (!MemTimeOut)
			{
				MEM_FLAG_SERVICE_WRITE = 1;
				pWrAddr = &ServiceAddress;
				goto mem_irq_start;
			}
			return;
		}
		else
		{
			MEM_SPI_BUF_REG = MemBuff[MemBuff_out];
			MemBuff_out = (MemBuff_out + 1) & MEM_BUFF_MASK;
			if (!(++CurrentWriteAddress.u8ll))
			{
				if ((++CurrentWriteAddress.u8lh == 0)	&&
					(++CurrentWriteAddress.u16h >= ServiceSector)	)
					CurrentWriteAddress.u32 = 0;
				
				goto mem_irq_start;
			}
			MemTimeOut		= MEM_SPI_TIMEOUT;
		}
	}
	return;
 }
mem_irq_eraseSector:	{
	if (pWrAddr->u8hh)
	{
		MEM_SPI_BUF_REG = MEM_CMD_4SE;
		MEM_SPI_BUF_REG = pWrAddr->u8hh;
	}
	else
		MEM_SPI_BUF_REG = MEM_CMD_SE;
	MEM_SPI_BUF_REG = pWrAddr->u8hl;
	MEM_SPI_BUF_REG = 0;
	MEM_SPI_BUF_REG = 0;

	MEM_FLAG_ERASED	= 1;				// set Flag to avoid second erase of same sector
	goto mem_irq_start;
 }
mem_irq_serviceSector:	{
	register U8* ptr	= (U8*)&ServiceStruct;
	register U16 i		= sizeof(SERVICE_STR);
	ServiceAddress.u16l += i;
	while (i--)
	{
		while (MEM_SPI_TX_FULL);
		MEM_SPI_BUF_REG = *ptr++;
	}
	pWrAddr = &ServiceStruct.WriteAddr;
	MEM_FLAG_SERVICE_WRITE = 0;
	if (MemBuff_in != MemBuff_out)
		goto mem_irq_start;

	MemIrqState = MemState_Idle;
	MEM_SPI_CLEAR();
	MEM_CS_OFF();
	MEM_WRITE_IE = 0;
	MEM_WRITE_IF = 1;
	return;
 }
}
//	--	Public Functions	------------------------------------------------------------
void MemInit (void)
{
	MEM_CS		= 1;				// MEM_CS_OFF();
	MEM_FLAGS	= 0;
	MemBuff_in	= MemBuff_out = 0;
	pWrAddr = &ServiceStruct.WriteAddr;
	MEM_SPI_INIT();
	MemIrqState = MemState_Idle;
	MemReadDeviceId();
	MemCurrentAddrInit();
}

void Mem_SetWriteAddress	(U32 addr)
{
	while (MEM_WRITE_IE)	// wait if write process active
	{
		if ((MemBuff_in == MemBuff_out)		&&
			(CurrentWriteAddress.u32 == addr)	)
			return;
	}
	CurrentWriteAddress.u32	= addr;
	if (CurrentWriteAddress.u16h >= ServiceSector)
		CurrentWriteAddress.u32	= 0;
	#if	(TEST == TEST_MEMORY)
		char tmp[60];
		sprintf(tmp, "New Write Addr: 0x%X%04X", CurrentWriteAddress.u16h, CurrentWriteAddress.u16l);
		DebugPrint(tmp);
	#endif
//	MEM_FLAG_SERVICE_WRITE = 1;
//	pWrAddr		= &ServiceAddress;
//	MemIrqState	= MemState_Status;
//	MEM_WRITE_IE = 1;
}

void Mem_SetReadAddress		(U32 addr)
{
	if (CurrentReadAddress.u32	!= addr)
	{
		if (MemIrqState == MemState_ReadData)		// if Read process active
		{											// stop read to init read from new address
			MemIrqState = MemState_Idle;			// in next read
			MEM_CS		= 1;						// MEM_CS_OFF();
		}
		CurrentReadAddress.u32	= addr;
	}
	#if	(TEST == TEST_MEMORY)
		char tmp[60];
		sprintf(tmp, "New Read Addr: 0x%X%04X", CurrentReadAddress.u16h, CurrentReadAddress.u16l);
		DebugPrint(tmp);
	#endif
}

U32	 Mem_GetWriteAddress	(void)
{
	return	CurrentWriteAddress.u32;
}

U32	 Mem_GetReadAddress		(void)
{
	return	CurrentReadAddress.u32;
}

U16	 Mem_GetType			(void)
{
	return	MemType;
}

// Write one byte to memory (save in spi buffer & enable spi interrupt)
// ret = 0:		Ok
// ret = -1:	Nok - spi buffer full, try again later
inline S8	Mem_WriteByte	(U8 data)
{
	register U16 next = (MemBuff_in + 1) & MEM_BUFF_MASK;
	if (next == MemBuff_out)					// previous data not sent
	{
		DebugPrint("Write buffer full");
		MEM_WRITE_IF	= 1;
		MEM_WRITE_IE	= 1;
		return -1;
	}
	MemBuff[MemBuff_in]	= data;					// feel buffer
	MemBuff_in			= next;					// next address in buffer
//	MEM_WRITE_IF	= 1;
	MEM_WRITE_IE		= 1;					// enable irq
	return 0;
}

// Write one byte to memory (save in spi buffer & enable spi interrupt)
// note: no return while not sucess (write buffer full) 
// ret = -1:	Nok - spi buffer full, try again later
inline void	Mem_WriteByte_1	(U8 data)
{
	register U16 next = (MemBuff_in + 1) & MEM_BUFF_MASK;
	while (next == MemBuff_out)					// previous data not sent
	{
		MEM_WRITE_IF	= 1;
		MEM_WRITE_IE	= 1;
	}
	MemBuff[MemBuff_in]	= data;					// feel buffer
	MemBuff_in			= next;					// next address in buffer
//	MEM_WRITE_IF	= 1;
	MEM_WRITE_IE		= 1;					// enable irq
}

inline U8	 MemStatus	(void)
{
	return (MemBuff_in == MemBuff_out)?	0 : 1;
}
// Read one byte from memory by 'CurrentReadAddress'
// ret = 0:		Ok	- data in *ptr
// ret = 1: 	Ok	- data in *ptr, memory roll ower to '0'
// ret = -1:	Nok	- cann't read data, write process active
inline S8	Mem_ReadByte	(U8* ptr)
{
	if (MEM_WRITE_ACTIVE)
	{
		DebugPrint("Write active");
		return -1;
	}
	if (MemIrqState != MemState_ReadData)
		Mem_ReadStart();
	while (MEM_SPI_RX_FIFO_EMPTY);
	*ptr = MEM_SPI_BUF_REG;
	MEM_SPI_BUF_REG = 0;
	if ((!++CurrentReadAddress.u16l) && (++CurrentReadAddress.u16h >= ServiceSector))
	{
		CurrentReadAddress.u16h = 0;
		MemIrqState	= MemState_Idle;
		return 1;
	}
	return 0;
}

/*
U8 Mem_ReadByte			(void)
{
	if (!MEM_FLAG_READ_ACTIVE)
		Mem_ReadStart();
	register U8 data = SPI1BUF;
	SPI1BUF = 0;
	CurrentReadAddress.u32++;
	return data;
}

void Mem_WriteEnd			(void)
{
	MEM_FLAG_WRITE_ACTIVE	= 0;
	Spi1Clear();
	MEM_CS_OFF();
}
*/
//	--	Internal Functions	------------------------------------------------------------
void MemReadDeviceId		(void)
{
	register U8 Id, Type, Capacity;
	MEM_SPI_CLEAR();
	MEM_CS_ON();
	MEM_SPI_BYTE(MEM_CMD_RDID);
	Id			= MEM_SPI_BYTE(0);
	Type		= MEM_SPI_BYTE(0);
	Capacity	= MEM_SPI_BYTE(0);
	MEM_CS_OFF();

	if	(	(Id == 0x01) &&	(Type == 0x02) &&	(Capacity == 0x20)	)	MemType = MEM_SECTORS_S25FL512S;
	else if	(Id == 0x20)
	{
		if					(Type == 0x20)
		{
			if									(Capacity == 0x16)		MemType = MEM_SECTORS_MP25P32;
			else if								(Capacity == 0x17)		MemType = MEM_SECTORS_MP25P64;
		}
		else if	(			(Type == 0xBA) &&	(Capacity == 0x16)	)	MemType = MEM_SECTORS_MP25P32;
	}	
	switch (MemType)
	{
		case MEM_SECTORS_MP25P32:		ServiceSector = (MEM_SECTORS_MP25P32	- 1);PageMask = 0x00FF;	SectorMask = 0x0000;	break;
		case MEM_SECTORS_MP25P64:		ServiceSector = (MEM_SECTORS_MP25P64	- 1);PageMask = 0x00FF;	SectorMask = 0x0000;	break;
		case MEM_SECTORS_S25FL512S:		ServiceSector = (MEM_SECTORS_S25FL512S	- 4);PageMask = 0x01FF;	SectorMask = 0x0003;	break;
		default:						/* ERROR	*/								break;
	}
}

void MemCurrentAddrInit		(void)
{
	memset((char*)&ServiceStruct, 0, sizeof(ServiceStruct));
	ServiceAddress.u16l = 0;
	MEM_SPI_CLEAR();
	MEM_CS_OFF();
	MEM_CS_ON();
	if (ServiceAddress.u8hh)
	{
		MEM_SPI_BUF_REG = MEM_CMD_4READ;
		MEM_SPI_BUF_REG = ServiceAddress.u8hh;
	}
	else
		MEM_SPI_BUF_REG = MEM_CMD_READ;
	MEM_SPI_BUF_REG = ServiceAddress.u8hl;
	MEM_SPI_BUF_REG = 0;
	MEM_SPI_BUF_REG = 0;
	MEM_SPI_CLEAR();
	MEM_SPI_BUF_REG = 0;
	MEM_SPI_BUF_REG = 0;
	while (true)
	{
		while (MEM_SPI_RX_FIFO_EMPTY);
		register U8	i = MEM_SPI_BUF_REG;				// read 1st byte
		if (i == 0xFF)									// 0xFF -> current area not saved:
			break;										// previously readed area = actual service sector (or cleared sector if memory is clear)
		MEM_SPI_BUF_REG = 0;							// initiate next spi transfer
		register U8* ptr	= (U8*)&ServiceStruct;		// ptr = start of service sector
		*ptr++ = i;										// save already readed byte
		
		for (i = 1; i < sizeof(ServiceStruct); i++)		// read all structure
		{
			while (MEM_SPI_RX_FIFO_EMPTY);
			*ptr++ = MEM_SPI_BUF_REG;
			MEM_SPI_BUF_REG = 0;
		}
		ServiceAddress.u16l += i;						// set service address to next free area
		if (!ServiceAddress.u16l)						// if address returned around - error
		{												// memory is defective - define as clear (will be cleared at first time of writing)
			memset((char*)&ServiceStruct, 0, sizeof(ServiceStruct));
			break;
		}
	}
	MEM_CS_OFF();
	if (BootFlashNew())
	{
		CurrentWriteAddress.u32	= 0;	// Memory used for bootLoader - clear
		FL_BOOT_TCP = 1;
	}
	CurrentReadAddress.u32	= 0;	// temporary
}

void Mem_ReadStart			(void)
{
	Mem_ReadStatus(1);

//	DebugPrint("Memory Read start");
	MEM_SPI_CLEAR();
	MEM_CS_OFF();
	MEM_CS_ON();
	if (CurrentReadAddress.u8hh)
	{
		MEM_SPI_BUF_REG = MEM_CMD_4READ;
		MEM_SPI_BUF_REG = CurrentReadAddress.u8hh;
	}
	else
		MEM_SPI_BUF_REG = MEM_CMD_READ;
	MEM_SPI_BUF_REG = CurrentReadAddress.u8hl;
	MEM_SPI_BUF_REG = CurrentReadAddress.u8lh;
	MEM_SPI_BUF_REG = CurrentReadAddress.u8ll;
	MemIrqState		= MemState_ReadData;
	MEM_SPI_CLEAR();
	MEM_SPI_BUF_REG	= 0;
	MEM_SPI_BUF_REG	= 0;
}
/*
void Mem_WriteStart			(void)
{
	MEM_FLAG_READ_ACTIVE	= 0;
	Spi1Clear();
	MEM_CS_OFF();
	if (NEW_WRITE_SECTOR)
		Mem_PageErase();
	Mem_WriteEnable();
	MEM_CS_ON();
	#if (MEMORY > 256)
		if (CurrentWriteAddress.u8hh)
		{
			SPI1BUF = MEM_CMD_4PP;
			SPI1BUF = CurrentWriteAddress.u8hh;
		}
		else
	#endif
	SPI1BUF = MEM_CMD_PP;
	SPI1BUF = CurrentWriteAddress.u8hl;
	SPI1BUF = CurrentWriteAddress.u8lh;
	SPI1BUF = CurrentWriteAddress.u8ll;
	MEM_FLAG_WRITE_ACTIVE	= 1;
	Spi1Clear();
}
*/
void Mem_PageErase			(void)
{
	DebugPrint("Erase current sector");
	Spi1Clear();
	Mem_WriteEnable();
	MEM_CS_ON();
	if (CurrentWriteAddress.u8hh)
	{
		SPI1BUF = MEM_CMD_4SE;
		SPI1BUF = CurrentWriteAddress.u8hh;
	}
	else
		SPI1BUF = MEM_CMD_SE;
	SPI1BUF = CurrentWriteAddress.u8hl;
	SPI1BUF = 0;
	SPI1BUF = 0;
	Spi1Clear();
	MEM_CS_OFF();
}

//	--	Service Functions	------------------------------------------------------------
U8	 Mem_ReadStatus		(U8 wait)
{
	Spi1Clear();
	MEM_CS_ON();
	Spi1Byte(MEM_CMD_RDSR);
	U8 status	= Spi1Byte(0);
	while(wait && (status & MEM_STATUS_WIP))
		status	= Spi1Byte(0);
	MEM_CS_OFF();
	return status;
}

void Mem_WriteStatus		(U8 status)
{
	Spi1Clear();
	MEM_CS_ON();
	Spi1Byte(MEM_CMD_WRSR);
	Spi1Byte(status);
	MEM_CS_OFF();
}

void Mem_WriteEnable		(void)
{
	Mem_ReadStatus(1);
	MEM_CS_ON();
	Spi1Byte(MEM_CMD_WREN);
	MEM_CS_OFF();
	Mem_ReadStatus(1);
}

void Mem_WriteDisable		(void)
{
	Mem_ReadStatus(1);
	MEM_CS_ON();
	Spi1Byte(MEM_CMD_WRDI);
	MEM_CS_OFF();
	Mem_ReadStatus(1);
}

