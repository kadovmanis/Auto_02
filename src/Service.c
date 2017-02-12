#include	"Service.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Memory.h"
#include	"PrinterInterface.h"
#include	"Uart3.h"
#include	"Spi_i2c.h"
#include	<string.h>
#include	<stdio.h>

#define	CURSOR_UP()			Uart3SendByte(0x1B); Uart3SendByte(0x5B); Uart3SendByte(0x41)
#define	CURSOR_DOWN()		Uart3SendByte(0x1B); Uart3SendByte(0x5B); Uart3SendByte(0x42)
#define	CURSOR_RIGHT()		Uart3SendByte(0x1B); Uart3SendByte(0x5B); Uart3SendByte(0x43)
#define	CURSOR_LEFT()		Uart3SendByte(0x1B); Uart3SendByte(0x5B); Uart3SendByte(0x44)

#define	ANIMATION_TIME_MS    150

static	U8		code		= 0, CommandIdx = 0;
static	U8		buf[128];
static	char*	str			= (char*)&buf[0];
static	char	command[128] = "";
		U32		Percent_10	= 0;
static	U16		BmpPage		= 0, BmpPageHi = 3000;

inline void	Service_Uart3communication	(void);
void    ServiceCommandFunction  	(void);
void	ServiceFunction_System   	(void);
void	ServiceFunction_Reset   	(void);
//void	ServiceFunction_Status		(void);
//void	ServiceFunction_MemId   	(void);
void	ServiceFunction_MemStatus	(void);
//void	ServiceFunction_WriteEnable	(void);
void	ServiceFunction_MemRead		(void);
void	ServiceFunction_MemService	(void);
void	ServiceFunction_Memwrite	(void);
//void	ServiceFunction_MemEraseS	(void);
void	ServiceFunction_MemErase	(void);
void	ServiceFunction_Code		(void);
void	ServiceFunction_Byte		(void);
void	ServiceFunction_Info		(void);
void	ServiceFunction_SetWidth	(void);
void	ServiceFunction_SetAddress	(void);
void	ServiceFunction_FeelMem		(void);
//void	UartPrintBitmap				(void);
void	UartPrintBM_mem				(void);
void	UartPrintBM_1				(void);
U8		UartPrintBmp				(U16 page);
void	PrintMemInfo				(void);
void	CalculateCharRowWidth		(void);
void	MemBufferClear				(void);

void	StopOnError (const char* msg, const char* errFile, int errLine)
{
//	char str[40];
	Uart3SendText("\r\n\n\n*************** ERROR ***************");
	while(Uart3TxBuffResidual() < 250);
	if (msg[0])	{	Uart3SendEnter();		Uart3SendText(msg);	}
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText("\r\nFile: ");			Uart3SendText(errFile);
	sprintf(str, "\r\nLine: %d ", errLine);	Uart3SendText(str);
	Uart3SendText("\r\n*************** ERROR ***************\r\n\n\n");
	while (1);
}

void PrintStart (void)
{
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText("\r\n\n******************************");
	Uart3SendText("\r\nDevice:       ");	Uart3SendText(DEVICE);
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText("\r\nVersion:      ");	Uart3SendText(VERSION);
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText("\r\nBuild Date:   ");	Uart3SendText(__DATE__);
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText("\r\nBuild Time:   ");	Uart3SendText(__TIME__);
	Uart3SendText("\r\n******************************\r\n");
	PrintMemInfo();
}

void PrintMemInfo (void)
{
	U32 size	= GetDataSize();
	U16 width	= Get_BmpWithPix();

	if (width == 0xFFFF)	sprintf(str, "\r\nWidth not fixed, Saved bytes: %lu ", size);
	else
	{
		register U16	percents_x10	= (U16)(size / Percent_10);	// 0.1% no 2 atminam =	13,421,722	(6553) 51200
		U32	rows	= (size / (width / 8));
		sprintf(str, "\r\nWidth: %u, Saved Rows: %lu - %u.%u%% of memory ",
						width, rows, percents_x10 / 10, percents_x10 % 10);
		Uart3SendText(str);

		U16 charRow = GetCharRowWidth();
//		if (charRow != 0xFFFF)	
//			charRow /= width;
		BmpPage	= (U16)((rows + (BmpPageHi - 1)) / BmpPageHi);
		sprintf(str, "\r\nBmp Page Height: %u, Bmp pages: %u, Char row size: %u\r\n", BmpPageHi, BmpPage, charRow);
		while(Uart3TxBuffResidual() < 250);
	}
	Uart3SendText(str);
	if (FLAG_MEM_FULL)		Uart3SendText("\r\nMEMORY FULL\r\n");
}

void	CalculateCharRowWidth		(void)
{
	static U32	prevSize	= 0;
	static U8	oddEven		= 0;
	U32 datA	= GetDataSize();								// data size in bytes
	U32	datB	= datA - prevSize;								// calculate difference (last saved bytes only)
	prevSize	= datA;

	U16 datC	= Get_BmpWithPix();								// saved pixel row WIDTH
	if ((datC == 0xFFFF) || (!datB))							// pixel row WIDTH not fixed
		return;

	datB		= (datB / (datC / 8));							// count of currently saved rows in bytes
	datC		= GetCharRowWidth();							// already saved WIDTH of char row

	if (datB < datC)				{	datC = datB; oddEven = 0;	}	// count of currently saved rows < saved char row width? - recalculate
	while (datB % datC)					datC--;
	if ((datB / datC) & 0x0001)			oddEven |= 0x01;
	else								oddEven |= 0x02;

	if (datC != GetCharRowWidth())		SetCharRowWidth(datC, 0);
	else if (oddEven >= 0x03)			SetCharRowWidth(datC, 1);
/*	
	U16 rowWidth	= Get_BmpWithPix();							// row width in pixels
	if (rowWidth == 0xFFFF)										// row width not fixed
		return;
	U16	savedSize	= GetCharRowWidth();						// previous saved char row width (bytes)
	U32	savedRows	= (datSize / (rowWidth / 8));				// pixel rows in bytes
	rowWidth = (savedSize > savedRows)? savedRows: savedSize;	// new char row width = saved rows : saved char row width

	while (savedRows % rowWidth)
		rowWidth--;
//	SetCharRowWidth(rowWidth, (rowWidth == savedSize));
	if (rowWidth != savedSize)
		SetCharRowWidth(rowWidth, 0);
*/
}
/*
void	MemBufferClear				(void)
{
	static char*	pBuffOutLast = 0xFFFF;
	U8 i = 0xFF;
	while ((pBuffOutLast != GetMemBuffOut()) && (--i))
	{
		if (pBuffOutLast > pMemBuff_last)
			pBuffOutLast = pMemBuff;
//		if (pBuffOutLast == GetMemBuffIn())
//			pBuffOutLast = GetMemBuffOut();
		*pBuffOutLast++ = 0;
	}
}
*/
void Service(void)
{
//	static	U16	animationTimeS	= 0;
//	static	U16	animationTimeMs	= 0;

//	PrinterMotorTest();
	Service_Uart3communication();

//	if(GetTimeSinceMs(animationTimeMs) >= ANIMATION_TIME_MS)
//	{
//		animationTimeMs = GetTicsMs();
//	}
//	if(GetTimeSinceSec(animationTimeS) >= 1)
//	{
//		animationTimeS = GetSec();
//	}
	if (FLAG_MEM_NEW_DATA)
	{
		FLAG_MEM_NEW_DATA = 0;
		CalculateCharRowWidth();
	}
//	MemBufferClear();
}

inline void	Service_Uart3communication	(void)
{
	if (Uart3ByteReceived())                        // byte received from service uart
    {
		U8 a = Uart3GetByte();                      // get Service byte
		if(code)
		{
			static U8 i = 0;
			command[i++] = a;                   	// feel bufer
			if(i >= 20)
			{
				i = 0;
				Uart3PrintDebugData((U8*)command, 20);
			}
		}
		else
		{
			if ((a == CR) || (a == LF) || (CommandIdx >= (sizeof(command) - 1)))
			{
				if (CommandIdx)
				{
					command[CommandIdx] = '\0';
					ServiceCommandFunction();  		// execute function by received command
					CommandIdx = 0;
				}
			}
			else
			{
				command[CommandIdx++] = a;			// feel command string
				Uart3SendByte((char)a);				// send echo
			}
        }
    }
}

//___________________________________________________________________
typedef struct
{
    const char* command;
    void (*funct)(void);
} COMAND_FUNCTION;

const COMAND_FUNCTION CommandFunctions [] = 
{
//	{"sys",				PrintStart					},
	{"sys",				ServiceFunction_System		},
	{"reset",			ServiceFunction_Reset		},
	{"erase",			ServiceFunction_MemErase	},
	{"BMP",				UartPrintBM_1				},
	{"BM",				UartPrintBM_mem				},
	{"width",			ServiceFunction_SetWidth	},
	{"addr",			ServiceFunction_SetAddress	},
	{"feel",			ServiceFunction_FeelMem		},
//	{"id",				ServiceFunction_MemId		},
	{"status",			ServiceFunction_MemStatus	},
//	{"write enable",	ServiceFunction_WriteEnable	},
	{"read",			ServiceFunction_MemRead		},
	{"service",			ServiceFunction_MemService	},
	{"write",			ServiceFunction_Memwrite	},
//	{"sector",			ServiceFunction_MemEraseS	},
//	{"code",			ServiceFunction_Code		},
//	{"BM",				UartPrintBitmap				},
	{"pr",				PrinterDataPrint			},
//	{"st",				PrinterDataReset			},
//	{"byte",			ServiceFunction_Byte		},

//	{"",                ServiceFunction_Info    	}
};

void ServiceCommandFunction()
{
	if(command[0] == 0)
		return;
    
    U16 i;
    for(i = 0; i < (sizeof(CommandFunctions) / sizeof(COMAND_FUNCTION)); i++)
    {
		U16	len = strlen(CommandFunctions[i].command);
        if(!(memcmp(CommandFunctions[i].command, command, len)))
//		if(!(strcmp(CommandFunctions[i].command, command)))
		{
			CommandIdx = (CommandIdx > len)?	len: 0;
			if(command[0] != 'B')
			{
				Uart3SendText(" command    \r\n");
				#if (TEST != NO_TEST)
					Uart3SendText(CommandFunctions[i].command);
					Uart3SendText("_TEST\n\r");
				#endif
			}
			return CommandFunctions[i].funct();
		}
    }
	ServiceFunction_Info();
}

void ServiceFunction_System	(void)
{
//	U16 page, pageLen, rc = 0;//BmpPageHi, BmpPage
	if (CommandIdx)
		sscanf(&command[CommandIdx], "%u", &BmpPageHi);
//		rc = sscanf(&command[CommandIdx], "%u", &BmpPage);

	U16	rowSize		= GetCharRowWidth();
	if (rowSize < BmpPageHi)
		BmpPageHi = ((BmpPageHi / rowSize) * rowSize);

	PrintStart();
}

void ServiceFunction_Reset (void)
{
    Uart3SendText(" RESET");
    DelayMs(200);
    __asm__ ("RESET");
}
/*
void ServiceFunction_Status (void)
{
//	char str[60];
	U32	size		= GetDataSize();
	U16 widthPix	= Get_BmpWithPix();
	if (widthPix != 0xFFFF)
		sprintf(str, "\r\nWith: %u, Saved Rows: %lu ", widthPix, (size / widthPix));
	else
		sprintf(str, "\r\nWith not fixed, Saved bytes: %lu ", size);
	Uart3SendText(str);
}

extern U16	MemSectors (void);
void	ServiceFunction_MemId (void)
{
//	char str[60];
	sprintf(str, "\r\nMem ID: %02X, Type: %02X, Size: %02X, sectors: %d ", MemId(), MemType(), MemCapacity(), MemSectors());
	Uart3SendText(str);
}
*/

void	ServiceFunction_MemStatus (void)
{
	MemDebugInfo();
	MemDebugStatus();
	MemDebugServiceStatus();
}
/*
extern void MemWriteEnable (void);
void	ServiceFunction_WriteEnable (void)
{
	MemWriteEnable();
//	char str[40];
	sprintf(str, "\r\nWRITE ENABLE, StatusReg: %02X ", MemReadStatus(0));
	Uart3SendText(str);
}
*/
void	ServiceFunction_MemRead		(void)
{
	Uart3SendEnter();
	while(_U3TXIE);
	MemDataToUart(0, 0);
	while(_U3TXIE);
	Uart3SendEnter();
//	U8 data[10];
//	MemReadData(data, 0, 10);
//	Uart3PrintDebugData(data, 10);

	MemDebugStatus();
}

void	ServiceFunction_MemService	(void)
{
	Uart3SendEnter();
	while(_U3TXIE);
	U8 data[80];
//	MemReadData(data, 0x7FFFB0, sizeof(data));
	Uart3PrintDebugData(data, 12);
	Uart3PrintDebugData(&data[12], 16);
	Uart3PrintDebugData(&data[28], 16);
	Uart3PrintDebugData(&data[44], 16);
	Uart3PrintDebugData(&data[60], 16);
	Uart3PrintDebugData(&data[74], 4);
	while(_U3TXIE);
	Uart3SendEnter();

//	MemDebugStatus();
}

void	ServiceFunction_Memwrite	(void)
{
//	static U8 bit = 0xFF;
	U8 data[] = {
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
//		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75
		};
//	data[9] = bit;
//	MemWrite(data, 0, sizeof(data));
	Uart3PrintDebugData(data, sizeof(data));
//	if(!bit)		bit = 0xFF;
//	else			bit >>= 1;

	MemDebugStatus();
}
/*
void	ServiceFunction_MemEraseS	(void)
{
	MemSectorErase(0);
}
*/
void	ServiceFunction_MemErase	(void)
{
	LedStatus1	= LED_STATUS_ERROR;
	MemBulkErase();
    Uart3SendText("\r\nErase");
	U16 lastSec = 0, i = 0;
	while (true)
	{
		if(GetTimeSinceSec(lastSec) >= 1)
		{
			lastSec = GetSec();
			U8 status = MemStatusAll();
			if (!(status &  0x01))
			{
				Uart3SendText("\rErase OK               \r\n");
				MemInit();
				LedStatus1	= LED_STATUS_1;
				return;
			}
			else if (++i > 10)
			{
				i = 0;
				Uart3SendText("\r                          ");
				Uart3SendText("\rErase");
			}
			else
				Uart3SendByte('.');
		}
	}
}

void	ServiceFunction_Code		(void)
{
	code = 1;
}

void	ServiceFunction_Byte		(void)
{
	static U8 d = 'A';
	MemWriteByte(d);
	if(++d > 'z')
		d = 'A';
}

void	ServiceFunction_SetWidth	(void)
{
	U16	width = 0xFFFF;
	if (CommandIdx)
		sscanf(&command[CommandIdx], "%u", &width);
	Save_BmpWithPix(width);
	SetCharRowWidth(GetCharRowWidth(), 0);
	#if (TEST != NO_TEST)
		MemDebugServiceStatus();
	#endif
}

void	ServiceFunction_SetAddress	(void)
{
	UNI32 addr;
	addr.u32 = 0;
	if (CommandIdx)
		sscanf(&command[CommandIdx], "%X %X", &addr.u16h, &addr.u16l);
//		sscanf(&command[CommandIdx], "%X", &addr.u16h);

	U16 width	= Get_BmpWithPix();								// pixel row WIDTH
	U16 widthC	= GetCharRowWidth();							// Char row Hight
	if ((width	!= 0xFFFF) && (width >= 8) &&					// pixel row WIDTH fixed, at least 1 byte
		(widthC	!= 0xFFFF) && (width)			)				// Char row	 Hight fixed, != 0
	{
		width = (width / 8) * widthC;
		addr.u32 = (addr.u32 / width) * width;
	}

	SetDataSize(addr.u32);
	#if (TEST != NO_TEST)
		MemDebugStatus();
	#endif
}

void	ServiceFunction_FeelMem		(void)
{
	static U8	garb = 0x08;
	UNI32 size;
	size.u32 = 0;
	if (CommandIdx)
		sscanf(&command[CommandIdx], "%X %X", &size.u16h, &size.u16l);

	sprintf(str, "\r\nWrite 0x%02X x 0x%X%04X bytes ...", garb, size.u16h, size.u16l);
	while(Uart3TxBuffResidual() < 250);
	Uart3SendText(str);

	MemTest_FeelMem(size.u32, garb);
	if (!(garb >>= 1))	garb = 0x80;

	MemDebugStatus();
	Uart3SendText("\n\rOK\n\r");
}

void ServiceFunction_Info (void)
{
    U16  i;
	Uart3SendText("\n\rAvailable Commands:");
    for (i = 0; i < (sizeof(CommandFunctions) / sizeof(COMAND_FUNCTION)); i++)
        {
            Uart3SendText("\n\r  ");
            Uart3SendText(CommandFunctions[i].command);
        }
	Uart3SendEnter();
//	Uart3SendText("\r");
}


//___________________________________________________________________
#define	BMP_HEDER_SIZE		62
#define	BMP_FILE_TYPE_SIZE	2				// to print WITHOUT File Type identifier
typedef struct
{		// Bitmap Heder
	char	fileType[BMP_FILE_TYPE_SIZE];	// Magic identifier of BMP file = 0x42, 0x4D			(BM)
	U32		fileSize;						// File size in bytes (heder 62 bytes + image in bytes)
	U16		reserved1;						//														(0)
	U16		reserved2;						//														(0)
	U32		offset;							// Offset from file beginning to image data, bytes		(62)
		// INFO Heder
	U32		infoHederSize;					// INFO Header size in bytes							(40)
	U32		imageWidth_pix;					// Width  of image (pixels)
	U32		imageHeight_pix;				// Height of image (pixels)
	U16		colourPlanes;					// Number of colour planes								(1)
	U16		bitsPerPixel;					// Bits per pixel										(1)
	U32		compressionType;				// Compression type							currently	(0)
	U32		imageSize;						// Image size in bytes w/o heder
	U32		xResolution;					// Pixels per meter										(0)
	U32		yResolution;					// Pixels per meter										(0)
	U32		nColours;						// Number of colours									(0)
	U32		importantColours;				// Important colours									(0)
	char	remaindBytes[8];				// remain mystic bytes between INFO HEDER & Image start	(00,00,00,00,FF,FF,FF,00) 
} BMP_HEDER;

const BMP_HEDER HederTemplate =
{

	.fileType			= {0x42, 0x4D},
	.fileSize			= 0,				// File size		must be set		(.imageSize + 62 bytes of heder)
	.reserved1			= 0,
	.reserved2			= 0,
	.offset				= BMP_HEDER_SIZE,
	.infoHederSize		= 40,
	.imageWidth_pix		= 0,				// Width  of image	must be set		(real pixels)
	.imageHeight_pix	= 0,				// Height of image	must be set		(rows of real image)
	.colourPlanes		= 1,
	.bitsPerPixel		= 1,
	.compressionType	= 0,
	.imageSize			= 0,				// Image size		must be set		(all image bytes with residual emty bytes)
	.xResolution		= 0,
	.yResolution		= 0,
	.nColours			= 0,
	.importantColours	= 0,
	.remaindBytes		= {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00}
};

#define	TMP_BITMAP_WITH	(50)
#define	TMP_DATA_WITH	(384)
//#define	TMP_DATA_WITH	(648)
/*const U8 bitmap[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

*/
void UartPrintBitmap_ (const U8* data, U16 size, U16 widthPix)
{
	BMP_HEDER BmpHeder;
	memcpy((char*)&BmpHeder, (char*)&HederTemplate, sizeof(BMP_HEDER));

//	U16 widthPix	= TMP_BITMAP_WITH;
	U16 widthBytes	= (widthPix >> 3);																// widthPix / 8bit = withBytes
	if ((widthBytes << 3) < widthPix)		widthBytes++;											// pixel bytes must be in 8bit boundaries
	U16 rowWidth	= (widthBytes >> 2);															// withBytes / 4
	rowWidth		= ((rowWidth << 5) >= widthPix)? (rowWidth << 2): ((rowWidth + 1) << 2);		// pixel bytes in row must be in 4bytes (U32) boundaries
	U32 rows		= (size / widthBytes);

	BmpHeder.imageWidth_pix		= (U32)(widthPix);
	BmpHeder.imageHeight_pix	= rows;
	BmpHeder.imageSize			= (U32)(BmpHeder.imageHeight_pix * rowWidth);
	BmpHeder.fileSize			= (BmpHeder.imageSize + BMP_HEDER_SIZE);

	Uart3SendData((U8*)&BmpHeder + BMP_FILE_TYPE_SIZE, (BMP_HEDER_SIZE - BMP_FILE_TYPE_SIZE));

	U16	i = (U16)rows;
	while (i)
	{
		U16	j	= 0;
		const U8*	bmp	= &data[((--i) * widthBytes)];
		while (Uart3TxBuffResidual() < 250)	;				// wait if Tx buffer is not enough for transmit data
//		while (j++ < widthBytes)			Uart3SendByte((char)(*bmp++ ^ 0xFF));
		while (j++ < widthBytes)
		{
			U8 a = (*bmp++ ^ 0xFF);
			if(a == 0x18)	a |= 0x04;
			Uart3SendByte((char)(a));
		}
		while (j++ < rowWidth)				Uart3SendByte(0);
	}

//	char str[60];
	sprintf(str, "\r\n\npix %d, bytes %d rowBytes %d, rows %d , size %u \r\n", widthPix, widthBytes, rowWidth, (U16)(rows), size);
//	sprintf(str, "\r\n\nBytes %u, transm %u, sh1:0x0%X sh2:0x0%X, sh3:0x0%X , size %u \r\n", count, transm, sh1, sh2, sh3, size);
	Uart3SendText(str);
}
/*
void UartPrintBitmap (void)
{
	if (SpiData)
	{
		UartPrintBitmap_(printerData, SpiData, TMP_DATA_WITH);
		SpiData = 0;
	}
	else
		UartPrintBitmap_(bitmap, sizeof(bitmap), TMP_BITMAP_WITH);

}
*/

void UartPrintBM_mem (void)
{
	U16 widthPix	= Get_BmpWithPix();
	if((!widthPix) || (widthPix == 0xFFFF))
	{
		Uart3SendText("\r\nReceipt _Width_ not known - print more receipt rows...\r\n");
		return;
	}

	BMP_HEDER BmpHeder;
	memcpy((char*)&BmpHeder, (char*)&HederTemplate, sizeof(BMP_HEDER));

//	U16 widthPix	= TMP_DATA_WITH;
	U16 widthBytes	= (widthPix >> 3);																// widthPix / 8bit = withBytes
	if ((widthBytes << 3) < widthPix)		widthBytes++;											// pixel bytes must be in 8bit boundaries
	U16 rowWidth	= (widthBytes >> 2);															// withBytes / 4
	rowWidth		= ((rowWidth << 5) >= widthPix)? (rowWidth << 2): ((rowWidth + 1) << 2);		// pixel bytes in row must be in 4bytes (U32) boundaries
	U32	size		= GetDataSize();
	U32 rows		= (size / widthBytes);

	if (rowWidth > sizeof(buf))        
	{
//		char str[60];
		while (Uart3TxBuffResidual() < 250)	;				// wait if Tx buffer is not enough for transmit data
		Uart3SendText("\r\nNecessary to increase BMP row string size ");
		sprintf(str, "\r\n  need: %u, available: %u ", rowWidth, (U16)(sizeof(buf)));		Uart3SendText(str);
		sprintf(str, "\r\n (calculated receipt pixels in row: %u) \r\n", widthPix);	Uart3SendText(str);
		return;
	}

	BmpHeder.imageWidth_pix		= (U32)(widthPix);
	BmpHeder.imageHeight_pix	= rows;
	BmpHeder.imageSize			= (U32)(BmpHeder.imageHeight_pix * rowWidth);
	BmpHeder.fileSize			= (BmpHeder.imageSize + BMP_HEDER_SIZE);

	Uart3SendData((U8*)&BmpHeder + BMP_FILE_TYPE_SIZE, (BMP_HEDER_SIZE - BMP_FILE_TYPE_SIZE));
//*
	U32	i = rows;
	U16	residual = rowWidth - widthBytes;
	while (i)
	{
		i--;
		U16	j = MemDataToUart((i * widthBytes), widthBytes);
		if (j)
		{
			if (j > 16)
			{
				sprintf(str, "\r\n\nMemory Read ERROR! \r\n");
				while(Uart3TxBuffResidual() < 250);
				Uart3SendText(str);
			}
			i++;
			continue;
		}

		for(; j < widthBytes; j++)			// j already = 0
		{
			register U8 a = DataFromSpiMem();
			buf[j] = (a == 0xE7)?	0x1C: (a ^ 0xFF);
		}
		if (residual)
		{
			while(j < rowWidth)
			buf[j++] = 0;
		}
		while (Uart3TxBuffResidual() < 250)	;				// wait if Tx buffer is not enough for transmit data
		Uart3SendData(buf, rowWidth);
	}
	#if (TEST != NO_TEST)
		sprintf(str, "\r\n\npix %d, bytes %d rowBytes %d, rows: %lu, file size: %lu bytes \r\n",
						widthPix, widthBytes, rowWidth, rows, BmpHeder.fileSize);
		while(Uart3TxBuffResidual() < 250);
		Uart3SendText(str);

		MemDebugStatus();
		MemDebugServiceStatus();
	#endif
}

void UartPrintBM_1	(void)
{
	if (CommandIdx)
		sscanf(&command[CommandIdx], "%u", &BmpPage);
	if ((UartPrintBmp(BmpPage)) && (BmpPage))
		BmpPage--;
}

U8 UartPrintBmp	(U16 page)
{
	U8 rc = 0;
	U16 widthPix	= Get_BmpWithPix();
	if((!widthPix) || (widthPix == 0xFFFF))
	{
		Uart3SendText("\r\nReceipt _Width_ not known - print more receipt rows...\r\n");
		return 0;
	}

	BMP_HEDER BmpHeder;
	memcpy((char*)&BmpHeder, (char*)&HederTemplate, sizeof(BMP_HEDER));

//	U16 widthPix	= TMP_DATA_WITH;
	U16 widthBytes	= (widthPix >> 3);																// widthPix / 8bit = withBytes
	if ((widthBytes << 3) < widthPix)		widthBytes++;											// pixel bytes must be in 8bit boundaries
	U16 rowWidth	= (widthBytes >> 2);															// withBytes / 4
	rowWidth		= ((rowWidth << 5) >= widthPix)? (rowWidth << 2): ((rowWidth + 1) << 2);		// pixel bytes in row must be in 4bytes (U32) boundaries
	if (rowWidth > sizeof(buf))        
	{
		while (Uart3TxBuffResidual() < 250)	;				// wait if Tx buffer is not enough for transmit data
		Uart3SendText("\r\nNecessary to increase BMP row string size ");
		sprintf(str, "\r\n  need: %u, available: %u ", rowWidth, (U16)(sizeof(buf)));		Uart3SendText(str);
		sprintf(str, "\r\n (calculated receipt pixels in row: %u) \r\n", widthPix);	Uart3SendText(str);
		return 0;
	}

	U32 rowsEnd		= (GetDataSize() / widthBytes);
	U32 rowStart	= 0;
//	U32	firstRow, lastRow;
//	register U32	i;
	if (page)
	{
		rowStart		= ((U32)(page - 1)	* (U32)BmpPageHi);
		register U32 l	= (rowStart			+ (U32)BmpPageHi);
		if (rowsEnd > l)
			rowsEnd = l;
	}
	BmpHeder.imageWidth_pix		= (U32)(widthPix);
	BmpHeder.imageHeight_pix	= rowsEnd - rowStart;
	BmpHeder.imageSize			= ((U32)BmpHeder.imageHeight_pix * (U32)rowWidth);
	BmpHeder.fileSize			= (BmpHeder.imageSize + BMP_HEDER_SIZE);

	Uart3SendData((U8*)&BmpHeder, BMP_HEDER_SIZE);
//	Uart3SendData((U8*)&BmpHeder + BMP_FILE_TYPE_SIZE, (BMP_HEDER_SIZE - BMP_FILE_TYPE_SIZE));
//	i = lastRow;
	U16	residual = rowWidth - widthBytes;
	while (rowsEnd > rowStart)
	{
		rowsEnd--;
		U16	j = MemDataToUart((rowsEnd * (U32)widthBytes), widthBytes);
		if (j)
		{
			if (j > 16)
			{
				sprintf(str, "\r\n\nMemory Read ERROR! \r\n");
				while(Uart3TxBuffResidual() < 250);
				Uart3SendText(str);
			}
			rowsEnd++;
			continue;
		}

		for(; j < widthBytes; j++)			// j already = 0
		{
			register U8 a = DataFromSpiMem();
			buf[j] = (a == 0xE7)?	0x1C: (a ^ 0xFF);
		}
		if (residual)
		{
			while(j < rowWidth)
			buf[j++] = 0;
		}
		while (Uart3TxBuffResidual() < 250)	;				// wait if Tx buffer is not enough for transmit data
		Uart3SendData(buf, rowWidth);
	}
	#if (TEST != NO_TEST)
		sprintf(str, "\r\n\npix %d, bytes %d rowBytes %d, rows: %lu, file size: %lu bytes \r\n",
						widthPix, widthBytes, rowWidth, BmpHeder.imageHeight_pix, BmpHeder.fileSize);
		while(Uart3TxBuffResidual() < 250);
		Uart3SendText(str);

		MemDebugStatus();
		MemDebugServiceStatus();
	#endif
	return rc;
}

