#include "usb.h"
#include "usb_device_hid.h"
#include "system.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Memory.h"
#include	"Adc.h"
#include	"Gsm.h"
#include	"Gps.h"
#include	"Wifi.h"
#include	"Uart1.h"
#include	"Uart4.h"
#include <string.h>
#include <stdio.h>

extern void BootFlashInit	(void);
extern U8	BootFlashNew	(void);

#define	USB_OK					0x0C
#define	USB_ERROR				0xEE
#define	USB_COM_NONE			0x00
#define	USB_COM_RESET			0x01
#define	USB_COM_FLASH			0x02
#define	USB_COM_SERVICE			0x03

#define	USB_COM_MEM_GET_ADDR	0x10
#define	USB_COM_MEM_SET_ADDR	0x11
#define	USB_COM_MEM_WRITE		0x12
#define	USB_COM_MEM_READ		0x13
#define	USB_COM_MEM_CLOSE		0x14
#define	USB_COM_MEM_S_ERASE		0x15
#define	USB_COM_MEM_B_ERASE		0x16
#define	USB_COM_MEM_BOOT_ADDR	0x1A
#define	USB_COM_MEM_FEEL		0x1E
#define	USB_COM_MEM_STATUS		0x1F

#define	USB_COM_GET_ADC			0xA0
#define	USB_COM_GET_GSM_CSQ		0xA4

#define	USB_COM_ENABLE_GSM		0xE0
#define	USB_COM_ENABLE_GPS		0xE1
#define	USB_COM_ENABLE_PWM		0xE2
#define	USB_COM_ENABLE_WIFI		0xE3
#define	USB_COM_ENABLE_ADC		0xEA
//#define	USB_ERROR			0xEE

#define	USB_COM_TEXT			0xFC
#define	USB_COM_DEV_INFO		0xFD
#define	USB_COM_DEV_STATUS		0xFE
#define	USB_COM_TEST			0xFF

#define	USB_PACKET_SIZE			USB_EP0_BUFF_SIZE
#define	USB_PACKET_DATA_SIZE	(USB_PACKET_SIZE - 2)
#define	USB_WAIT_TX_READY()		if (HIDTxHandleBusy(USBInHandle))	UsbWaitTxReady()
#define	USB_SEND_PACKET()		USB_WAIT_TX_READY(); USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, Out->buf,USB_PACKET_SIZE)
#define	SWITCH_OUT_BUF()		Out = (NextOutBuf)?	&Out2: &Out1; NextOutBuf ^= 1

#define	OUT_PACKET				Out->buf
#define	OUT_COMMAND				Out->UsbCommand
#define	OUT_COM					Out->READwrite
#define	OUT_DATA				Out->Data
#define	OUT_DATA_16				Out->D_16
#define	OUT_DATA_LEN			Out->PackedDateLen
#define	OUT_ADDRESS				Out->Address
#define	OUT_READ_LEN			Out->PageSize

/** VARIABLES ******************************************************/
typedef union	//      1                2               3-64
{				// UsbCommand | ReadAddr/WriteLen | data/Address/ReadLen/...
	U8	buf[USB_PACKET_SIZE];
	struct
	{
		U8			UsbCommand;			// Usb main command
		union
		{
			U8		READwrite;			// 1 - Set Read Address; 0 - Set Address to write
			U8		PackedDateLen;		// size of bytes to write in current package (or read bytes in sendBuffer)
			U8		com;				// 
		};
		union
		{
			U8		Data[62];			// data buffer for read/write bytes
			U16		D_16[31];			// data buffer for read/write bytes (16bit)
			struct
			{
				U32	Address;			// Address / size of data etc
				U16	PageSize;			// to device: size of bytes to read;	from: calculated rows in bmp page
				U16	BmpWithPix;			// to device: set row width pix;		from: get saved/calculated width
				U16	BmpPage;			// to device: awaited Page nr.;			from: device: Page count
				U16	CharRowSize;		// Hight of character row (to calculate rows in bmp page)
				U32	Percent_0_1;		// size of 0.1% of available memory
			};
		};
	};
} USB_PACKET;

static	USB_PACKET			In, Out1, Out2, Text;
static	USB_PACKET*			Out;
static	U8					NextOutBuf = 0;
extern	U32					Percent_10;
extern	U16					BmpPage, BmpPageHi;
extern	U16					resCom, resAddr;

//static	unsigned int	UsbTimeout	= 0;

typedef union	//      1                2               3-64
{				// UsbCommand | ReadAddr/WriteLen | data/Address/ReadLen
	U8				DataBuffer[64];
	struct
	{
		U8			UsbCommand;				// Usb main command
		union
		{
			U8		READwrite;				// 1 - Set Read Address; 0 - Set Address to write
			U8		PackedDateLen;			// size of bytes to write in current package (or read bytes in sendBuffer)
		};
		union
		{
			U8		Data[62];				// data buffer for read/write bytes
			struct
			{
				U32	Address;				// read/write address to set
				U16	PageSize;				// size of bytes to read from mem
			};
		};
	};
} USB2MEM_CAST;

typedef struct
{
	U8		UsbCommand;					// Usb main command
	U8		led;
	U16		pot;
	U32		Raddr;
	U32		Waddr;
//	U8		MemId;
	U16		MemType;
	U8		MemCapacity;
} DEV_STATUS;

volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;

void UsbWaitTxReady			(void);
void UsbAckOk				(void);
void UsbAckError			(void);
void Usb_SendText			(char* str);

void UsbFunctNone			(void);
void UsbCommandReset		(void);		// Command 0x01
void UsbCommandFlash		(void);		// Command 0x02
void UsbCommandService		(void);		// Command 0x03
void UsbCommandService_f	(void);
void UsbCommandService_1	(void);
void UsbCommandService_2	(void);
void UsbCommandService_3	(void);
void UsbCommandService_4	(void);
void UsbCommandService_5	(void);
void UsbCommandService_6	(void);
void UsbCommandService_7	(void);
void UsbCommandService_8	(void);
void UsbCommandService_9	(void);
void UsbMemGetAddress		(void);		// Command 0x10
void UsbMemSetAddress		(void);		// Command 0x11
void UsbMemWrite			(void);		// Command 0x12
void UsbMemRead				(void);		// Command 0x13
void UsbMemClose			(void);		// Command 0x14
void UsbMemSecErase			(void);		// Command 0x15
void UsbMemBulkErase		(void);		// Command 0x16
void UsbMemSetBootAddress	(void);		// Command 0x1A
void UsbMemFeel				(void);		// Command 0x1E
void UsbMemStatus			(void);		// Command 0x1F
void UsbGetAdc				(void);		// Command 0xA0
void UsbGetGsmStatus		(void);		// Command 0xA4
void UsbFunctEnableGsm		(void);		// Command 0xE0
void UsbFunctEnableGps		(void);		// Command 0xE1
void UsbFunctEnablePwm		(void);		// Command 0xE2
void UsbFunctEnableWifi		(void);		// Command 0xE3
void UsbFunctEnableAdc		(void);		// Command 0xEA
void UsbDeviceText			(void);		// Command 0xFC
void UsbDeviceInfo			(void);		// Command 0xFD
void UsbDeviceStatus		(void);		// Command 0xFE
void UsbCommandTest			(void);		// Command 0xFF

void UsbPrintRecBuf			(void);

void (*UsbFunct[256])(void) =
{
	UsbFunctNone,			// 0x00
	UsbCommandReset,		// 0x01	Ok
	UsbCommandFlash,		// 0x02	Ok
	UsbCommandService,		// 0x03	Ok
	UsbFunctNone,			// 0x04
	UsbFunctNone,			// 0x05
	UsbFunctNone,			// 0x06
	UsbFunctNone,			// 0x07
	UsbFunctNone,			// 0x08
	UsbFunctNone,			// 0x09
	UsbFunctNone,			// 0x0A
	UsbFunctNone,			// 0x0B
	UsbFunctNone,			// 0x0C
	UsbFunctNone,			// 0x0D
	UsbFunctNone,			// 0x0E
	UsbFunctNone,			// 0x0F
	UsbMemGetAddress,		// 0x10 Ok
	UsbMemSetAddress,		// 0x11	Ok
	UsbMemWrite,			// 0x12	Ok
	UsbMemRead,				// 0x13	Ok
	UsbMemClose,			// 0x14	Ok
	UsbMemSecErase,			// 0x15 Ok
	UsbMemBulkErase,		// 0x16	Ok
	UsbFunctNone,			// 0x17
	UsbFunctNone,			// 0x18
	UsbFunctNone,			// 0x19
	UsbMemSetBootAddress,	// 0x1A	Ok
	UsbFunctNone,			// 0x1B
	UsbFunctNone,			// 0x1C
	UsbFunctNone,			// 0x1D
	UsbMemFeel,				// 0x1E	Ok
	UsbMemStatus,			// 0x1F	Ok
	UsbFunctNone,			// 0x20
	UsbFunctNone,			// 0x21
	UsbFunctNone,			// 0x22
	UsbFunctNone,			// 0x23
	UsbFunctNone,			// 0x24
	UsbFunctNone,			// 0x25
	UsbFunctNone,			// 0x26
	UsbFunctNone,			// 0x27
	UsbFunctNone,			// 0x28
	UsbFunctNone,			// 0x29
	UsbFunctNone,			// 0x2A
	UsbFunctNone,			// 0x2B
	UsbFunctNone,			// 0x2C
	UsbFunctNone,			// 0x2D
	UsbFunctNone,			// 0x2E
	UsbFunctNone,			// 0x2F
	UsbFunctNone,			// 0x30
	UsbFunctNone,			// 0x31
	UsbFunctNone,			// 0x32
	UsbFunctNone,			// 0x33
	UsbFunctNone,			// 0x34
	UsbFunctNone,			// 0x35
	UsbFunctNone,			// 0x36
	UsbFunctNone,			// 0x37
	UsbFunctNone,			// 0x38
	UsbFunctNone,			// 0x39
	UsbFunctNone,			// 0x3A
	UsbFunctNone,			// 0x3B
	UsbFunctNone,			// 0x3C
	UsbFunctNone,			// 0x3D
	UsbFunctNone,			// 0x3E
	UsbFunctNone,			// 0x3F
	UsbFunctNone,			// 0x40
	UsbFunctNone,			// 0x41
	UsbFunctNone,			// 0x42
	UsbFunctNone,			// 0x43
	UsbFunctNone,			// 0x44
	UsbFunctNone,			// 0x45
	UsbFunctNone,			// 0x46
	UsbFunctNone,			// 0x47
	UsbFunctNone,			// 0x48
	UsbFunctNone,			// 0x49
	UsbFunctNone,			// 0x4A
	UsbFunctNone,			// 0x4B
	UsbFunctNone,			// 0x4C
	UsbFunctNone,			// 0x4D
	UsbFunctNone,			// 0x4E
	UsbFunctNone,			// 0x4F
	UsbFunctNone,			// 0x50
	UsbFunctNone,			// 0x51
	UsbFunctNone,			// 0x52
	UsbFunctNone,			// 0x53
	UsbFunctNone,			// 0x54
	UsbFunctNone,			// 0x55
	UsbFunctNone,			// 0x56
	UsbFunctNone,			// 0x57
	UsbFunctNone,			// 0x58
	UsbFunctNone,			// 0x59
	UsbFunctNone,			// 0x5A
	UsbFunctNone,			// 0x5B
	UsbFunctNone,			// 0x5C
	UsbFunctNone,			// 0x5D
	UsbFunctNone,			// 0x5E
	UsbFunctNone,			// 0x5F
	UsbFunctNone,			// 0x60
	UsbFunctNone,			// 0x61
	UsbFunctNone,			// 0x62
	UsbFunctNone,			// 0x63
	UsbFunctNone,			// 0x64
	UsbFunctNone,			// 0x65
	UsbFunctNone,			// 0x66
	UsbFunctNone,			// 0x67
	UsbFunctNone,			// 0x68
	UsbFunctNone,			// 0x69
	UsbFunctNone,			// 0x6A
	UsbFunctNone,			// 0x6B
	UsbFunctNone,			// 0x6C
	UsbFunctNone,			// 0x6D
	UsbFunctNone,			// 0x6E
	UsbFunctNone,			// 0x6F
	UsbFunctNone,			// 0x70
	UsbFunctNone,			// 0x71
	UsbFunctNone,			// 0x72
	UsbFunctNone,			// 0x73
	UsbFunctNone,			// 0x74
	UsbFunctNone,			// 0x75
	UsbFunctNone,			// 0x76
	UsbFunctNone,			// 0x77
	UsbFunctNone,			// 0x78
	UsbFunctNone,			// 0x79
	UsbFunctNone,			// 0x7A
	UsbFunctNone,			// 0x7B
	UsbFunctNone,			// 0x7C
	UsbFunctNone,			// 0x7D
	UsbFunctNone,			// 0x7E
	UsbFunctNone,			// 0x7F
	UsbFunctNone,			// 0x80
	UsbFunctNone,			// 0x81
	UsbFunctNone,			// 0x82
	UsbFunctNone,			// 0x83
	UsbFunctNone,			// 0x84
	UsbFunctNone,			// 0x85
	UsbFunctNone,			// 0x86
	UsbFunctNone,			// 0x87
	UsbFunctNone,			// 0x88
	UsbFunctNone,			// 0x89
	UsbFunctNone,			// 0x8A
	UsbFunctNone,			// 0x8B
	UsbFunctNone,			// 0x8C
	UsbFunctNone,			// 0x8D
	UsbFunctNone,			// 0x8E
	UsbFunctNone,			// 0x8F
	UsbFunctNone,			// 0x90
	UsbFunctNone,			// 0x91
	UsbFunctNone,			// 0x92
	UsbFunctNone,			// 0x93
	UsbFunctNone,			// 0x94
	UsbFunctNone,			// 0x95
	UsbFunctNone,			// 0x96
	UsbFunctNone,			// 0x97
	UsbFunctNone,			// 0x98
	UsbFunctNone,			// 0x99
	UsbFunctNone,			// 0x9A
	UsbFunctNone,			// 0x9B
	UsbFunctNone,			// 0x9C
	UsbFunctNone,			// 0x9D
	UsbFunctNone,			// 0x9E
	UsbFunctNone,			// 0x9F
	UsbGetAdc,				// 0xA0
	UsbFunctNone,			// 0xA1
	UsbFunctNone,			// 0xA2
	UsbFunctNone,			// 0xA3
	UsbGetGsmStatus,		// 0xA4
	UsbFunctNone,			// 0xA5
	UsbFunctNone,			// 0xA6
	UsbFunctNone,			// 0xA7
	UsbFunctNone,			// 0xA8
	UsbFunctNone,			// 0xA9
	UsbFunctNone,			// 0xAA
	UsbFunctNone,			// 0xAB
	UsbFunctNone,			// 0xAC
	UsbFunctNone,			// 0xAD
	UsbFunctNone,			// 0xAE
	UsbFunctNone,			// 0xAF
	UsbFunctNone,			// 0xB0
	UsbFunctNone,			// 0xB1
	UsbFunctNone,			// 0xB2
	UsbFunctNone,			// 0xB3
	UsbFunctNone,			// 0xB4
	UsbFunctNone,			// 0xB5
	UsbFunctNone,			// 0xB6
	UsbFunctNone,			// 0xB7
	UsbFunctNone,			// 0xB8
	UsbFunctNone,			// 0xB9
	UsbFunctNone,			// 0xBA
	UsbFunctNone,			// 0xBB
	UsbFunctNone,			// 0xBC
	UsbFunctNone,			// 0xBD
	UsbFunctNone,			// 0xBE
	UsbFunctNone,			// 0xBF
	UsbFunctNone,			// 0xC0
	UsbFunctNone,			// 0xC1
	UsbFunctNone,			// 0xC2
	UsbFunctNone,			// 0xC3
	UsbFunctNone,			// 0xC4
	UsbFunctNone,			// 0xC5
	UsbFunctNone,			// 0xC6
	UsbFunctNone,			// 0xC7
	UsbFunctNone,			// 0xC8
	UsbFunctNone,			// 0xC9
	UsbFunctNone,			// 0xCA
	UsbFunctNone,			// 0xCB
	UsbFunctNone,			// 0xCC
	UsbFunctNone,			// 0xCD
	UsbFunctNone,			// 0xCE
	UsbFunctNone,			// 0xCF
	UsbFunctNone,			// 0xD0
	UsbFunctNone,			// 0xD1
	UsbFunctNone,			// 0xD2
	UsbFunctNone,			// 0xD3
	UsbFunctNone,			// 0xD4
	UsbFunctNone,			// 0xD5
	UsbFunctNone,			// 0xD6
	UsbFunctNone,			// 0xD7
	UsbFunctNone,			// 0xD8
	UsbFunctNone,			// 0xD9
	UsbFunctNone,			// 0xDA
	UsbFunctNone,			// 0xDB
	UsbFunctNone,			// 0xDC
	UsbFunctNone,			// 0xDD
	UsbFunctNone,			// 0xDE
	UsbFunctNone,			// 0xDF
	UsbFunctEnableGsm,		// 0xE0     USB_COM_ENABLE_GSM
	UsbFunctEnableGps,		// 0xE1     USB_COM_ENABLE_GPS
	UsbFunctEnablePwm,		// 0xE2     USB_COM_ENABLE_PWM
	UsbFunctEnableWifi,		// 0xE3     USB_COM_ENABLE_WIFI
	UsbFunctNone,			// 0xE4
	UsbFunctNone,			// 0xE5
	UsbFunctNone,			// 0xE6
	UsbFunctNone,			// 0xE7
	UsbFunctNone,			// 0xE8
	UsbFunctNone,			// 0xE9
	UsbFunctEnableAdc,		// 0xEA     USB_COM_ENABLE_ADC
	UsbFunctNone,			// 0xEB
	UsbFunctNone,			// 0xEC
	UsbFunctNone,			// 0xED
	UsbFunctNone,			// 0xEE     USB_ERROR
	UsbFunctNone,			// 0xEF
	UsbFunctNone,			// 0xF0
	UsbFunctNone,			// 0xF1
	UsbFunctNone,			// 0xF2
	UsbFunctNone,			// 0xF3
	UsbFunctNone,			// 0xF4
	UsbFunctNone,			// 0xF5
	UsbFunctNone,			// 0xF6
	UsbFunctNone,			// 0xF7
	UsbFunctNone,			// 0xF8
	UsbFunctNone,			// 0xF9
	UsbFunctNone,			// 0xFA
	UsbFunctNone,			// 0xFB
	UsbDeviceText,			// 0xFC
	UsbDeviceInfo,			// 0xFD
	UsbDeviceStatus,		// 0xFE
	UsbCommandTest,			// 0xFF
};

/** FUNCTIONS ******************************************************/

void APP_DeviceCustomHIDInitialize()
{	//initialize the variable holding the handle for the last transmission
	USBInHandle = 0;
	SWITCH_OUT_BUF();

	//enable the HID endpoint
	USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

	//Re-arm the OUT endpoint for the next packet
	USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP, In.buf, USB_PACKET_SIZE);
}

#define USB_TIMEOUT_MS		20
inline void UsbMsTimer (void)
{
	static U8 i = USB_TIMEOUT_MS;
	if (!--i)
	{
		i = USB_TIMEOUT_MS;
//		UsbTimeout++;
	}
}

/*********************************************************************
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCustomHIDInitialize() and APP_DeviceCustomHIDStart() demos
*   respectively.
********************************************************************/
void APP_DeviceCustomHIDTasks()
{
	if(HIDRxHandleBusy(USBOutHandle) == false)
	{
		UsbFunct[In.UsbCommand]();													// execute function according to received command
		USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, In.buf, USB_PACKET_SIZE);	// init next receive
	}
}

void UsbWaitTxReady			(void)
{
//	while (HIDTxHandleBusy(USBInHandle));
	unsigned int i = 0xFFFF;
	while ((HIDTxHandleBusy(USBInHandle)) && i--);
}

void UsbAckError			(void)
{
//	SWITCH_OUT_BUF();
	Out = &Out1;
//	Out = (NextOutBuf)?	Out2:Out1;
	NextOutBuf ^= 1;
	OUT_COMMAND		= USB_ERROR;
	OUT_COM			= USB_ERROR;
	USB_SEND_PACKET();
}

void UsbAckOk			(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_OK;
	OUT_COM			= USB_OK;
	USB_SEND_PACKET();
}

void UsbFunctNone			(void)
{
	Usb_SendText("Uncnown USB command - not implemented");
	LED_HID = LED_STATUS_ERROR;
}

void UsbCommandReset		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_RESET;
	if (In.com == (0xFF - USB_COM_RESET))
	{
		LED_HID = LED_STATUS_ERROR;
		OUT_COM		= USB_OK;
		USB_SEND_PACKET();				// Send Ack
		USB_WAIT_TX_READY();			// & wait while packet sent

		__asm__ ("RESET");
		while(1);
	}
	else
		OUT_COM		= USB_ERROR;
	USB_SEND_PACKET();
}

void UsbCommandFlash		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND	= USB_COM_FLASH;
	if (In.com	== (0xFF - USB_COM_FLASH))
	{
		BootFlashInit();
		OUT_COM		= USB_OK;
	}
	else
		OUT_COM		= USB_ERROR;
	USB_SEND_PACKET();
}

void UsbCommandService		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_SERVICE;
	switch (In.com)
	{
	case 'f':	UsbCommandService_f();	break;
	case '1':	UsbCommandService_1();	break;
	case '2':	UsbCommandService_2();	break;
	case '3':	UsbCommandService_3();	break;
	case '4':	UsbCommandService_4();	break;
	case '5':	UsbCommandService_5();	break;
	case '6':	UsbCommandService_6();	break;
	case '7':	UsbCommandService_7();	break;
	case '8':	UsbCommandService_8();	break;
	case '9':	UsbCommandService_9();	break;
	default:	OUT_DATA_LEN	= 0;	break;
	}
	USB_SEND_PACKET();
}

//extern	UNI32	testVal;
//extern	U8		testBuf[20];
void UsbCommandService_f	(void)
{
/*	{	//	Boot_ReadReset();
		register unsigned int	offset	asm ("w6") = 0x000100;
		TBLPAG	= 0;
		resCom	= __builtin_tblrdh(offset);
		resAddr	= __builtin_tblrdl(offset);
	}
*/
	*(U16*)&OUT_DATA[0] = resCom;
	*(U16*)&OUT_DATA[2] = resAddr;
	*(U16*)&OUT_DATA[4] = 0x1234;       //testVal.u16h;	// 0x1234;
	*(U16*)&OUT_DATA[6] = 0x5678;       // testVal.u16l;	// 0x5678;
//	*(U16*)&OUT_DATA[8] = (U16)(&resCom);
//	*(U16*)&OUT_DATA[10] = (U16)(&resAddr);
//	memcpy(&OUT_DATA[8], testBuf, 20);
	OUT_DATA_LEN	= 62;
	BootFlashNew();
}

void UsbCommandService_1	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_On();
	#elif	(TEST == TEST_GPS)
		GPS_IE = !GPS_IE;
	#elif	(TEST == TEST_WIFI)
		Wifi_On();
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= (GSM_IE)?	'1':'0';
}

void UsbCommandService_2	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_PwrOn();
	#elif	(TEST == TEST_GPS)
		Gps_On();
	#elif	(TEST == TEST_WIFI)
		Wifi_Reset();
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'P';
}

void UsbCommandService_3	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_PwrKey();
	#elif	(TEST == TEST_WIFI)
		Wifi_BaudrateChange();
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'K';
}

void UsbCommandService_4	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_AT();
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'A';
}

void UsbCommandService_5	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_AtCommand((char*)In.Data);
	#elif	(TEST == TEST_GPS)
		Uart1_SendData((char*)In.Data, In.PackedDateLen);
	#elif	(TEST == TEST_WIFI)
		Wifi_SendCommand((char*)In.Data);
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'C';
}

void UsbCommandService_6	(void)
{
	#if		(TEST == TEST_GSM)
		Gsm_DTR();
	#endif
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'A';
}

void UsbCommandService_7	(void)
{
	OUT_DATA_LEN	= 2;
	#if		(TEST == TEST_GSM)
		*(U16*)&OUT_DATA[0] = Gsm_AUTO();
	#elif	(TEST == TEST_WIFI)
		*(U16*)&OUT_DATA[0] = Wifi_AUTO();
	#endif
}

void UsbCommandService_8	(void)
{
	Gsm_OnLine();
	OUT_DATA_LEN	= 1;
	OUT_DATA[0]		= 'O';
}

void UsbCommandService_9	(void)
{
	#ifdef	EXT_OUT2
		EXT_OUT2 = !EXT_OUT2;
		OUT_DATA_LEN	= 1;
		OUT_DATA[0]		= (EXT_OUT2)?	'1': '0';
	#else
		Usb_SendText("Command for this device not implemented");
		OUT_DATA_LEN	= 0;
	#endif
}

void UsbMemGetAddress		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_MEM_GET_ADDR;
	OUT_DATA_LEN	= 4;
	OUT_ADDRESS		= (In.READwrite)?	Mem_GetReadAddress(): Mem_GetWriteAddress();
	USB_SEND_PACKET();
}

void UsbMemSetAddress		(void)
{
//	UsbPrintRecBuf();
	if (In.READwrite)	Mem_SetReadAddress(In.Address);
	else				Mem_SetWriteAddress(In.Address);
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_MEM_SET_ADDR;
	OUT_COM		= USB_OK;
	USB_SEND_PACKET();
}

void UsbMemWrite			(void)
{
	register U8 i;
	for (i = 0; i < In.PackedDateLen; i++)
	{
		while (Mem_WriteByte(In.Data[i]) < 0);
	}

	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_MEM_WRITE;
	OUT_COM		= i;
	USB_SEND_PACKET();
}

void UsbMemRead				(void)
{
	register U16 size	= In.PageSize;
	Mem_SetReadAddress(In.Address);

	#if	(TEST == TEST_MEMORY)
		char tmp[64];
		sprintf(tmp, "Addr = 0x%X%04X; Size = %u ", (U16)(In.Address >> 16), (U16)(In.Address & 0xFFFF), size);
		Usb_SendText(tmp);
	#endif
	
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_READ;

//	OUT_COM				= MemDataToUart(In.Address, size);
//	if (OUT_COM)
//	{
//		USB_SEND_PACKET();
//		return;
//	}
	while (size)
	{
		register U8 i;
		OUT_DATA_LEN = (size > USB_PACKET_DATA_SIZE)?	USB_PACKET_DATA_SIZE: size;
		size -= OUT_DATA_LEN;

//		Usb_SendText("Read data one packet");
		for (i = 0; i < OUT_DATA_LEN; i++)
		{
			while (Mem_ReadByte(&OUT_DATA[i]) < 0);
		}

		USB_SEND_PACKET();
		SWITCH_OUT_BUF();
		OUT_COMMAND			= USB_COM_MEM_READ;
	}
	OUT_DATA_LEN = (U8)size;
	USB_SEND_PACKET();
}

void UsbMemClose			(void)
{
//	Mem_WriteEnd();
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_CLOSE;
	OUT_COM				= USB_OK;
	USB_SEND_PACKET();
}

void UsbMemSecErase			(void)
{
	Mem_PageErase();
//	Usb_SendText("Sector Erase command not implemented");
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_S_ERASE;
	OUT_COM				= USB_OK;
	USB_SEND_PACKET();
}

void UsbMemBulkErase		(void)
{
//	MemBulkErase();
	Usb_SendText("Bulk Erase command not implemented jet");
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_B_ERASE;
	OUT_COM				= USB_OK;
	USB_SEND_PACKET();
}

void UsbMemSetBootAddress	(void)
{
//	#ifdef		BOOT_LOADER
//		Out->PageSize	= MemSetAddressTo_0_chip();	// 16bit Boot memory chip Type
//	#else
	#ifdef GPS_IE			// if defined GPS interrupt
		GPS_IE = 0;			// switch it off to avoid memory write
	#endif
	FL_BOOT_PROCESS = 1;
		Mem_SetWriteAddress(0);
//		Out->PageSize	= 0;
//	#endif	//	BOOT_LOADER
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_BOOT_ADDR;
	Out->PageSize		= Mem_GetType();
	Out->Address		= Mem_GetWriteAddress();
	OUT_DATA_LEN		= 6;
	USB_SEND_PACKET();
}

void UsbMemFeel				(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_FEEL;
	OUT_COM				= USB_PACKET_DATA_SIZE;	// 62 bytes
	Out->Address		= Mem_GetWriteAddress();		// writed in memory bytes count
//	Out->PageSize		= BmpPageHi;			// 3000	at start
//	Out->BmpWithPix		= Get_BmpWithPix();		// Pix count in row	/0/0xFFFF
//	Out->BmpPage		= BmpPage;				// 0	at start
//	Out->CharRowSize	= GetCharRowWidth();	// byte count in row
//	Out->Percent_0_1	= Percent_10;			// calculated at start 0.1% of available memory (in bytes)
	USB_SEND_PACKET();
}

void UsbMemStatus			(void)
{
//	Usb_SendText("Status command not implemented");
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_MEM_STATUS;
	OUT_COM				= MemStatus();
	USB_SEND_PACKET();
}

void UsbGetAdc			(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_GET_ADC;

	OUT_DATA_LEN	= 10;					// 16bit x 5
	OUT_DATA_16[0]	= Adc_GetPowerState();
	OUT_DATA_16[1]	= Adc_GetPower();
	OUT_DATA_16[2]	= Adc_GetBattery();
	OUT_DATA_16[3]	= Adc_GetCoreV();
	OUT_DATA_16[4]	= Adc_GetDcDc();

	USB_SEND_PACKET();
}

void UsbGetGsmStatus	(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_GET_GSM_CSQ;
	OUT_DATA_LEN	= 0;
	OUT_COM			= Gsm_GetCsq();
	OUT_DATA_16[0]	= Gsm_GetState();
	OUT_DATA_16[1]	= Gsm_GetFlags();
	USB_SEND_PACKET();
}

void UsbFunctEnableGsm		(void)
{
	if (In.com < 2)
	{
		if (In.com)		Gsm_On();
		else			Gsm_Off();
	}

	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_TEXT;
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Gsm %s", (GSM_IE)?	"Enabled" : "Disabled");
	USB_SEND_PACKET();
}

void UsbFunctEnableGps		(void)
{
	if (In.com < 2)
		GPS_IE = (In.com)?	1 : 0;

	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_TEXT;
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Gps %s", (GPS_IE)?	"Enabled" : "Disabled");
	USB_SEND_PACKET();
}

void UsbFunctEnablePwm		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_TEXT;
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Command not implemented jet");
	USB_SEND_PACKET();
}

void UsbFunctEnableWifi		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_TEXT;
#ifdef	WIFI_EN
	if (In.com < 2)
	{
		if (In.com)		Wifi_On();
		else			Wifi_Off();
	}
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Wifi %s", (WIFI_EN)?	"Enabled" : "Disabled");
#else
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Device have not Wifi");
#endif
	USB_SEND_PACKET();
}

void UsbFunctEnableAdc		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND		= USB_COM_TEXT;
#if	(HARDWARE == HW_HOME)
	EXT_OUT2 = !EXT_OUT2;
	if (EXT_OUT2)
		OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Relay ENabled");
	else
		OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Relay DISabled");
#else
	OUT_DATA_LEN	= sprintf((char*)OUT_DATA, "Command not implemented jet");
#endif
	USB_SEND_PACKET();
}

void UsbCommandTest			(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND			= USB_COM_TEST;
	OUT_COM				= 0xFE;
	OUT_ADDRESS			= 0x12345678;
	OUT_READ_LEN		= 0x9ABC;
	USB_SEND_PACKET();

	SWITCH_OUT_BUF();
	memcpy(OUT_PACKET, In.buf, USB_PACKET_SIZE);
	USB_SEND_PACKET();
}

void Usb_SendText			(char* str)
{
	USB_WAIT_TX_READY();
	Text.UsbCommand	= USB_COM_TEXT;		// USB Command ID
	register U8 i;
	for (i = 0; i < 61; i++)			// String width \0 must not exceed 62
	{
		register U8 a	= str[i];
		if (!a)	break;					// test \0
		Text.Data[i]	= a;			// copy data
	}
	Text.Data[i]		= 0;			// \0
	Text.PackedDateLen	= i;			// save data size
	USB_WAIT_TX_READY();
	USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, Text.buf, USB_PACKET_SIZE);
}

void UsbDeviceText			(void)
{
	
}

void UsbDeviceInfo			(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND				= USB_COM_DEV_INFO;
	OUT_COM					= 1;
	sprintf((char*)OUT_DATA, "%s %s %s %s \r", DEVICE, VERSION, Build_Date, Build_Time);
	U8* ptr = OUT_DATA;
	while (*ptr++ != '\r')
		OUT_COM++;
	USB_SEND_PACKET();
}

//extern	U8		ManufacturerId, MemoryType, MemoryCapacity;
void UsbDeviceStatus		(void)
{
	SWITCH_OUT_BUF();
	OUT_COMMAND				= USB_COM_DEV_STATUS;
	DEV_STATUS* devStatus	= (DEV_STATUS*)&OUT_PACKET[0];
//	DEV_STATUS* devStatus = (DEV_STATUS*)&ToSendDataBuffer[0];
	devStatus->led			= (LED_HID == LED_STATUS_1_I)?	0:1;
	devStatus->pot			= 1;
	devStatus->Raddr		= Mem_GetReadAddress();
	devStatus->Waddr		= Mem_GetWriteAddress();
//	devStatus->MemId		= 1;
	devStatus->MemType		= Mem_GetType();
//	devStatus->MemCapacity	= 1;
	
	USB_SEND_PACKET();
}

void UsbPrintRecBuf			(void)
{
	char tmp[62];
	sprintf(tmp, "%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X",
			In.buf[0],In.buf[1],In.buf[2],In.buf[3],In.buf[4],In.buf[5],In.buf[6],In.buf[7],In.buf[8],In.buf[9]);
	Usb_SendText(tmp);
}

