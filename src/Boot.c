#include	"Ports.h"

//	"G:\Prog\16bit\Auto_01\Out\hex2bin.exe" "G:\Prog\16bit\Auto_01\Out\Auto_01.hex"
#ifdef		BOOT_LOADER
#define		BOOT_TEST			OFF				// ON/OFF

#define		BOOT_VERSION		"Boot_03R"

#define		FLASH_CLEARED		0b11111111		// 0xFF	- flash write no completed
#define		FLASH_PROGRAMMED	0b11111110		// 0xFE	- flash write complete (main programm must acctept new code)
#define		FLASH_OK			0b11111100		// 0xFC	- flash programm complete & accepted by main programm
#define		FLASH_IN_MEM		0b11111000		// 0xF8	- New programm writed in spi memory
#define		FLASH_ROW_INSTR		64
#define		FLASH_ROW_WORDS		128				// (FLASH_ROW_INSTR * 2)
#define		FLASH_ROW_BYTES		256				// (FLASH_ROW_INSTR * 4)

#define		RESET_ADDR			0x000100
#define		BOOT_LOADER_ADDR	0x000400
#define		BOOT_LOADER_END		0x000C00

#ifdef	ROM_DATA_ADDR
	#define	NO_LOAD_END			(ROM_DATA_ADDR + ROM_DATA_SIZE)
#else
	#define	NO_LOAD_END			BOOT_LOADER_END
#endif
//#define		BOOT_FNC_SIZE		0x0014
//#define		BALANCE_ADDR		0x000A8C	// w/o optimisation
#define		BALANCE_ADDR		0x000ABA		// optimisation enabled
#define		BALANCE_SIZE		((BOOT_LOADER_END - BALANCE_ADDR) / 2)
// * BootLoader data transfer Protocol *************************
//	 x		len(0)	   1		2,3,4,5		 6...		n
// START	LEN		COMMAND		ADDRESS		DATA	CHECK_SUM
//	0xA5	16bit	8bit_CMD	32bit		8bit[]		8bit
#define	BOOT_DATA_STRUCT_LEN	512						//	len			start		command			address
#define	BOOT_DATA_LEN			(BOOT_DATA_STRUCT_LEN - (sizeof(U16) + sizeof(U8) + sizeof(U8) + sizeof(UNI32)))

#define		BOOT_SECTOR_FNC		__attribute__ ((section(".bootLoader"),	address(BOOT_LOADER_ADDR)))
#define		BOOT_SECTOR_DAT		__attribute__((space(prog), section(".bootLoader")))
#define		BOOT_SECTOR_BALANCE	__attribute__((space(prog), section(".bootLoader"), fillupper(0xFF)))
#define		PROGRAMM_IN_MEM()	(resCom == FLASH_IN_MEM)
#define		PROGRAMM_UART()		(resCom != FLASH_IN_MEM)
#define		RESET_PROGRAMMED()	((resCom != FLASH_CLEARED) && (resCom >= FLASH_OK))
#define		RESET_OK()			((resAddr != 0xFFFF) && (resAddr != 0))
#define		TxByte(X)			while (UART_TX_FULL); UART_TX_DATA = X
#define		Boot_TxString_p(X)	{	__prog__ char* str = X;	while (*str) {TxByte(*str++);}	}
#define		BootSpiByte(S,R)	BOOT_SPI_BUF = S; while (BOOT_SPI_RX_FIFO_EMPTY); R = BOOT_SPI_BUF
#define		BootSpiByteGet(X)	while (BOOT_SPI_RX_FIFO_EMPTY); X = BOOT_SPI_BUF; BOOT_SPI_BUF = 0

#define		Boot_CmdErase()		{	NVMCON	= NVMCON_ERASE_PAGE;											\
									TBLPAG	= BootData.address.u16h;										\
									__builtin_tblwtl(BootData.address.u16l, 0x0000);						\
									asm("DISI #5");															\
									__builtin_write_NVM();													}
#define		Boot_CmdProgramm()	{	register unsigned int	i		asm ("w5") = FLASH_ROW_INSTR;			\
									register unsigned int	offset	asm ("w6") = BootData.address.u16l;		\
									register unsigned int*	pData	asm ("w7") = BootData.data16;			\
									NVMCON	= NVMCON_PROG_ROW;												\
									TBLPAG	= BootData.address.u16h;										\
									if ((!BootData.address.u16h) && (BootData.address.u16l == RESET_ADDR))	\
									{	resetL	= BootData.data16[0];	BootData.data16[0] = 0xFFFF;		\
										resetH	= BootData.data16[1];										\
										if (PROGRAMM_UART())			BootData.data16[1] = 0xFFFF;	}	\
									do																		\
									{	__builtin_tblwtl(offset, *pData++);									\
										__builtin_tblwth(offset, *pData++);									\
										offset += 2;														\
									} while (--i);															\
									asm("DISI #5");															\
									__builtin_write_NVM();													}
#define		Boot_CmdReadMem()	{	register unsigned int	i		asm ("w5") = (FLASH_ROW_BYTES);			\
									register unsigned char*	pData	asm ("w7") = BootData.data;				\
									do{	BootSpiByteGet(*pData++);	}	while (--i);						}

#ifndef		MEM_CMD_READ
	#define	MEM_CMD_READ		0x03					// Read Data bytes (Spi Memory)
	#define	MEM_CMD_RDSR		0x05
#endif
#define		BOOT_START			0xA5
#define		ACK_CS				0x33
#define		NAK_CS				0x34
#define		ACK_ERASE			0x35
#define		ACK_PROG			0x36
#define		ACK_TIMEOUT			0x60
#define		BOOT_CMD_RESET		0xB0
#define		BOOT_CMD_ERASE		0xB1
#define		BOOT_CMD_PROGRAM	0xB2

#define		NVMCON_PROG_ROW		0x4001
#define		NVMCON_PROG_WORD	0x4003
#define		NVMCON_ERASE_PAGE	0x4042
#define		NVMCON_ERASE_BULK	0x404F

void	 			BOOT_SECTOR_FNC		BootLoader		(void);

__prog__ char		BOOT_SECTOR_DAT		Msg_Test[]		= "\r\nBoot TEST";
__prog__ char		BOOT_SECTOR_DAT		Msg_Start[]		= "\r\nBoot Loader Start";
__prog__ char		BOOT_SECTOR_DAT		Msg_B_Vers[]	= "\r\nVersion:      ";
__prog__ char		BOOT_SECTOR_DAT		Msg_B_Date[]	= "\r\nBuild Date:   ";
__prog__ char		BOOT_SECTOR_DAT		Msg_B_Time[]	= "\r\nBuild Time:   ";
__prog__ char		BOOT_SECTOR_DAT		Msg_Vers[]		= BOOT_VERSION;
__prog__ char		BOOT_SECTOR_DAT		Msg_Date[]		= __DATE__;
__prog__ char		BOOT_SECTOR_DAT		Msg_Time[]		= __TIME__;
__prog__ int		BOOT_SECTOR_BALANCE	Balance[BALANCE_SIZE];

static union
{
	char		ch[BOOT_DATA_STRUCT_LEN];
	U8			bytes[BOOT_DATA_STRUCT_LEN];
	struct
	{
		union
		{
			U8			sizeBytes[4];
			U32			size;
			struct{
				UNI16	len;			// START & LEN swaped to align
				U8		start;			// len to 16bit var
				U8		command;		// COMMAND
			};
		};
		UNI32	address;
		union
		{
			U8		data[BOOT_DATA_LEN];
			U16		data16[(BOOT_DATA_LEN / 2)];
		};
	};
} BootData						__attribute__ ((address(0x2000)));

static	enum
{
	BootState_Wait = 0,
	BootState_Len,
	BootState_Capture,
	BootState_CheckSum,
	BootState_Command,

	BootState_Idle
} BootState						__attribute__ ((address(0x2200)));
static	unsigned char	csm		__attribute__ ((address(0x2202)));
static	unsigned int	idx		__attribute__ ((address(0x2204)));
static	unsigned char*	rxData	__attribute__ ((address(0x2206)));
volatile unsigned int	resCom	__attribute__ ((address(0x2208),noload));		// space should be allocated for the variable, but that initial values should not be loaded
volatile unsigned int	resAddr	__attribute__ ((address(0x220A),persistent));	// variable should not be initialized or cleared at startup
static	unsigned int	resetH	__attribute__ ((address(0x220C)));
static	unsigned int	resetL	__attribute__ ((address(0x220E)));
extern	UNI32	testVal;
extern	U8		testBuf[20];

// ------------------------------------------------------------------------------------------------
void BootLoader	(void)
{
	{	// Initialize Ports	---------------------------------------------------------
		BOOT_LOADER_PORT_INIT();					// must be defined in Ports.h
	}
	{	// Initialize OSC	---------------------------------------------------------
		_PLLEN = 1;									// Enable PLL 96 MHz
		__builtin_write_OSCCONH(Osc_32_MHz);
		__builtin_write_OSCCONL(0x01);				// Initiate Clock Switch
		while (_OSWEN);								// Wait for Clock switch to occur
		while(_LOCK != 1);							// Wait for PLL to lock
	}
	{	// Boot_ReadReset();
		register unsigned int	offset	asm ("w6") = RESET_ADDR;
		TBLPAG	= 0;
		resCom	= __builtin_tblrdh(offset);
		resAddr	= __builtin_tblrdl(offset);
	}

	if (PROGRAMM_IN_MEM())							// resCom = 1111 1000; resAddr = ?
		goto Boot_ProgrammFromMem;
#if	(BOOT_UART != NO_PORT)
	{	// Initialize UART	---------------------------------------------------------
		__builtin_write_OSCCONL(OSCCONL & 0xbf);	// Unlock Registers
		BOOT_LOADER_PPS_UART();						// RB6 < Rx3;  RB7  < Tx3
		__builtin_write_OSCCONL(OSCCONL | 0x40);	// Lock Registers
		BOOT_LOADER_INIT_UART();					// Initialize UART HW Registers
	}
	{	// Boot_WaitStart();						Catch BOOT\n from Uart
		register U16	repeat = 5;
		while (repeat)
		{
			register U16 timeOut = 0xFFFF, chNr = 0;
			TxByte('~');
			while (timeOut--)
			{
				if (!UART_RX_EMPTY)					// Uart have data
				{
					register char a = UART_RX_DATA;	// get byte
					switch (chNr)
					{
					case 0:	if (a != 'B')	chNr = 0;	else	chNr++;		break;
					case 1:	if (a != 'O')	chNr = 0;	else	chNr++;		break;
					case 2:	if (a != 'O')	chNr = 0;	else	chNr++;		break;
					case 3:	if (a != 'T')	chNr = 0;	else	chNr++;		break;
					case 4:	if (a != '\n')	chNr = 0;	else	goto Boot_ProgrammFromUart;	break;
					default:				chNr = 0;						break;
					}
					timeOut = 0xFFFF;
				}
			}
			if (RESET_PROGRAMMED())
				repeat--;
			LED_BOOT = !LED_BOOT;
		}
	}
#endif
//Boot_ProgrammStart:
	if (RESET_OK())									// resAddr = Ok -> goto Main Programm start
	{	// Go to Reset address to start main programm	-----------------------------
		asm("mov	_resAddr,	w6");
		asm("goto				w6");
	}
	else if (resCom == FLASH_CLEARED)
		goto	Boot_ResetDevice;

Boot_ProgrammFromMem:
	{	// Initialize SPI	---------------------------------------------------------
		__builtin_write_OSCCONL(OSCCONL & 0xbf);	// Unlock Registers
		BOOT_LOADER_PPS_MEM_SPI();					// RF4 < SCK2; RB14 < SDI2; RB15 < SDO2
		__builtin_write_OSCCONL(OSCCONL | 0x40);	// Lock Registers
		BOOT_LOADER_INIT_MEM();						// Initialize SPI HW Registers
	}
	{	// Auto programm	---------------------------------------------------------
		register U8		a;
		BootData.address.u32	= 0;
		//	Clear SPI buffer	-----------------------------------------------------
		MEM_CS = 1;										// BOOT_MEM_CS_OFF();
		while ((!BOOT_SPI_SHIFT_EMPTY) || (!BOOT_SPI_RX_FIFO_EMPTY))
				a = BOOT_SPI_BUF;
		//	Memory Status check	-----------------------------------------------------
		BOOT_MEM_CS_ON();
		BootSpiByte(MEM_CMD_RDSR, a);					// Status read command
		do	{	BootSpiByte(0, a);
		} while ( a & 0x01 );
		MEM_CS = 1;										// BOOT_MEM_CS_OFF();
		//	Memory Address set	-----------------------------------------------------
		BOOT_MEM_CS_ON();
		BootSpiByte(MEM_CMD_READ, a);					// Memory Read command
		BootSpiByte(0, a);								// Address 3th byte 0xaAaa
		BootSpiByte(0, a);								// Address 2nd byte 0xaaAa
		BootSpiByte(0, a);								// Address 1st byte 0xaaaA
		BOOT_SPI_BUF = 0;								// Feel spi buffer to
		BOOT_SPI_BUF = 0;								// read next bytes

		for (a = 0; a < 4; a++)
		{	BootSpiByteGet(BootData.sizeBytes[a]);	}	// Memory Read (File Size)
//		testVal.u32 = BootData.size;
		BootData.size >>= 1;							// size in bytes -> addreses in words
//		BootData.size >>= 2;							// size in bytes -> addreses in words
		while (BootData.address.u32 < BootData.size)
		{
			Boot_CmdReadMem();
			if ((BootData.address.u32 >= NO_LOAD_END) || (BootData.address.u32 < BOOT_LOADER_ADDR))
			{
				if (!(BootData.address.u16l & 0x03FF))
				{
					Boot_CmdErase();
					LED_BOOT = !LED_BOOT;
				}
				Boot_CmdProgramm();
			}
			BootData.address.u32 += (FLASH_ROW_WORDS);
		}

//		for (a = 0; a < 20; a++)
//			testBuf[a] = BootData.data[a];
		MEM_CS = 1;										// BOOT_MEM_CS_OFF();
		
	}
	goto Boot_WriteResetAddress;

#if	(BOOT_UART != NO_PORT)
Boot_ProgrammFromUart:
	{	// Uart Programming	---------------------------------------------------------
		unsigned int rxTimeOut = 0, ledTime = 0;
		rxData		= &BootData.command;
		BootState	= BootState_Wait;
		resetL		= 0xFFFF;
		resetH		= 0xFFFF;
		BootData.len.u16 = 0;

		Boot_TxString_p(Msg_Start);
		Boot_TxString_p(Msg_B_Vers);	Boot_TxString_p(Msg_Vers);
		Boot_TxString_p(Msg_B_Date);	Boot_TxString_p(Msg_Date);
		Boot_TxString_p(Msg_B_Time);	Boot_TxString_p(Msg_Time);
		TxByte('\r');	TxByte('\n');

		while (1)
		{
			if (!UART_RX_EMPTY)										// Rx have a byte
			{
				register unsigned char data = UART_RX_DATA;			// get byte
				switch (BootState)
				{
				case BootState_Wait:	if (data == BOOT_START)	{	csm	= 0; idx = 1;		BootState	= BootState_Len;	}
										break;
				case BootState_Len:		if (idx)				{	BootData.len.u8l = data; idx = 0;						}
										else					{	BootData.len.u8h = data;BootState = BootState_Capture;	}
										break;
				case BootState_Capture:	rxData[idx++] = data;
										csm			 ^= data;
										if (idx >= BootData.len.u16)						BootState = BootState_CheckSum;
										break;
				case BootState_CheckSum: rxData[idx] = data;
										BootState = BootState_Wait;
										if (data != csm)		{TxByte(NAK_CS); TxByte(data); TxByte(csm);	break;			}
				case BootState_Command:	TxByte(ACK_CS);
										switch (BootData.command)
										{
										case BOOT_CMD_RESET:	goto Boot_WriteResetAddress;
										case BOOT_CMD_ERASE:	Boot_CmdErase();				TxByte(ACK_ERASE);	break;
										case BOOT_CMD_PROGRAM:	Boot_CmdProgramm();				TxByte(ACK_PROG);	break;
										default:															break;
										}
										break;
				default:				break;
				}
				rxTimeOut = 0;
			}
			else if (!--ledTime)
			{
				if (!rxTimeOut)			{	rxTimeOut = 8;	LED_BOOT = !LED_BOOT;	}
				else					{	rxTimeOut--;	LED_BOOT = 1;			}
			}
		}
	}
#endif
Boot_WriteResetAddress:
	{	// Set Reset Address	-----------------------------------------------------
		register unsigned int	offset	= RESET_ADDR;
		TBLPAG	= 0;								// Initialize PM Page Boundary SFR
		NVMCON	= NVMCON_PROG_WORD;					// Initialize NVMCON
		__builtin_tblwtl(offset, resetL);			// Write to address low word
		__builtin_tblwth(offset, resetH);			// Write to upper byte
		asm("DISI #5");								// Block interrupts with priority <7
		__builtin_write_NVM();						// C30 function to perform unlock
		while(NVMCONbits.WR == 1)	{}
	}
Boot_ResetDevice:
	__asm__ ("RESET");	while(1);
}

void BootFlashInit	(void)
{
	register unsigned int	offset	= RESET_ADDR;
	TBLPAG	= 0;
	resCom	= __builtin_tblrdh(offset);
	resAddr	= __builtin_tblrdl(offset);

	resCom	&= FLASH_IN_MEM;

	NVMCON	= NVMCON_PROG_WORD;					// Initialize NVMCON
	__builtin_tblwtl(offset, resAddr);			// Write to address low word
	__builtin_tblwth(offset, resCom);			// Write to upper byte
	asm("DISI #5");								// Block interrupts with priority <7
	__builtin_write_NVM();						// C30 function to perform unlock
	while(NVMCONbits.WR == 1)	{}
}

U8	 BootFlashNew	(void)
{
	if (resCom == FLASH_PROGRAMMED)
	{
		resCom	&= FLASH_OK;
		register unsigned int	offset	= RESET_ADDR;
		TBLPAG	= 0;
		NVMCON	= NVMCON_PROG_WORD;					// Initialize NVMCON
		__builtin_tblwtl(offset, resAddr);			// Write to address low word
		__builtin_tblwth(offset, resCom);			// Write to upper byte
		asm("DISI #5");								// Block interrupts with priority <7
		__builtin_write_NVM();						// C30 function to perform unlock
		while(NVMCONbits.WR == 1)	{}
		return 1;
	}
	return 0;
}

#else
	#define	BootFlashInit()
	#define BootFlashNew()		0
#endif