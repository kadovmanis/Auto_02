#include	"Ports.h"

char	__attribute__((space(auto_psv)))	Build_Date[] =	__DATE__;
char	__attribute__((space(auto_psv)))	Build_Time[] =	__TIME__;

#ifdef	ROM_DATA_ADDR	//_____________ Flash read/write functions ____________________________________
	#include	"libpic30.h"
	#include	<string.h>
	#include	<stdio.h>
	// Flash data runing copy in RAM
	FLASH_DATA_STRUCT DataFromFlash;
	// Flash data copy in ROM
	
	U16 __attribute__((space(prog),address(ROM_DATA_ADDR),aligned(ROM_DATA_SIZE),fillupper(0xFF))) FlashData[_FLASH_PAGE] = 
	{
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	};

	// Copy data from flash to ram structure
	void FlashDataInit(void)
	{
		_prog_addressT	pFlash;
		_init_prog_address(pFlash, FlashData);								// get address in program space
		_memcpy_p2d16(&DataFromFlash, pFlash, sizeof(FLASH_DATA_STRUCT));	// copy data from program space

		if ((memcmp(DataFromFlash.BuildTime, Build_Date, sizeof(Build_Date)) == 0)	&&
			(memcmp(DataFromFlash.BuildDate, Build_Time, sizeof(Build_Time)) == 0)		)
			return;

		if (ROM_DATA_CLEARED)												// Flash data not initialized jet
		{
			memset((char*)(&DataFromFlash), NULL, sizeof(FLASH_DATA_STRUCT));
			DataFromFlash.flags.word		=	0x0000;
			DataFromFlash.DeviceId			=	DEVICE_ID;
			#if (TEST != NO_TEST)
				sprintf(DataFromFlash.Name,		DEVICE_NAME);
			#endif
			sprintf(DataFromFlash.GsmSmsNumber,	"+37129279661");
		}
		memcpy(DataFromFlash.BuildTime, Build_Date, sizeof(Build_Date));
		memcpy(DataFromFlash.BuildDate, Build_Time, sizeof(Build_Time));
		FL_BOOT_TCP = 1;
		FlashDataBurn();
	}

	// Burn actual ram data to nonvolatile memory (flash)
	void FlashDataBurn(void)
	{
		_prog_addressT	pFlash;
		_init_prog_address(pFlash, FlashData);								// get address in program space
		_erase_flash(pFlash);												// erase a page
		_write_flash16(pFlash, (int*)(&DataFromFlash));						// write first row with 16-bit data
	}
#endif	// ROM_DATA___________________________________________________________________________

volatile FLAGS		SysFlags;

void InitPorts(void)
{
	InitPort_B();
	InitPort_C();
	InitPort_D();
	InitPort_E();
	InitPort_F();
	InitPort_G();
	SysFlags.word	= 0;
}


//-------------------------------------------------------------------------------------------------
//	Default PPS configuration after RESET
//-------------------------------------------------------------------------------------------------
void PPSconfig(enum ppsConfig ppsSwitch)
{
//	SistemState = ppsSwitch;
	// Unlock Registers
	__builtin_write_OSCCONL(OSCCONL & 0xbf);

	switch (ppsSwitch)
	{
	case pps_system:
		#if ((defined GPS_RX) && (defined GPS_TX))
			_U1RXR	= GPS_RX;		// RB9	< Rx_GPS
			GPS_TX	= IO_U1TX;		// RB8	< Tx_GPS
		#endif

		#if ((defined GSM_RX) && (defined GSM_TX))
			_U2RXR	= GSM_RX;		// RD2	< Rx_GSM
			GSM_TX	= IO_U2TX;		// RD1	< Tx_GSM
		#endif

//		_U3RXR		= SET_RX;		// RB6	< Rx_PGC
//		SET_TX		= IO_U3TX;		// RB7	< Tx_PGD
        SET_TX		= IO_PORT;		// RB7	< IO
        _U3RXR		= PPS_IN_VSS;	// RB6	< IO
		
		#ifdef UART4_WIFI
			_U4RXR	= WIFI_RX;		// RD2	< Rx_WIFI
			WIFI_TX	= IO_U4TX;		// RD1	< Tx_WIFI
		#endif

		#if     (LCD != LCD_NO)
			LCD_SCK	= IO_SCK1OUT;	// RD11	< SCK1 (LCD)
			LCD_SDA	= IO_SDO1;		// RD0	< SDO1 (LCD)
		#endif

		#if		(MEMORY1_PORT == SPI_1)
			_SDI1R	= MEM_SI;		// RB14	< SDI1 (MEM)
			MEM_SCK	= IO_SCK1OUT;	// RF4	< SCK1 (MEM)
			MEM_SO	= IO_SDO1;		// RB15	< SDO1 (MEM)
		#elif	(MEMORY1_PORT == SPI_2)
			_SDI2R	= MEM_SI;		// RB14	< SDI2 (MEM)
			MEM_SCK	= IO_SCK2OUT;	// RF4	< SCK2 (MEM)
			MEM_SO	= IO_SDO2;		// RB15	< SDO2 (MEM)
		#endif

		PWM_DC		= IO_OC1;		// G9	< PWM for DC_DC_Up
//		PWM			= IO_OC2;		// F5	< PWM for GSM test "sound"
		break;
	default:
		break;
	}

	// Lock Registers
	__builtin_write_OSCCONL(OSCCONL | 0x40);
}

extern void Gsm_Off	(void);
void PowerOff (void)
{
	GPS_POW_OFF();
	LedStatus1 = LedStatus2 = LedStatus3 = LED_STATUS_ERROR;
	Gsm_Off();
	FL_POWER_OFF = 0;
	__asm__ ("RESET");	while(1);
}

void Reset      (void)
{
    __asm__ ("RESET");	while(1);
}

