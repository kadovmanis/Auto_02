#ifndef	_ports_h_
#define	_ports_h_

#include	"Typedef.h"
#include	"Timers.h"

#ifdef	ROM_DATA_ADDR
	typedef struct
	{
		FLAGS		flags;
		U16			DeviceId;
		char		BuildDate[20];
		char		BuildTime[12];
		char		Name[20];
		char		GsmSmsNumber[16];
		U8			MyIp[4];
		U8			GatewayIp[4];
		U8			SubnetMask[4];
		U8			MacAddress[6];
		U16			ResetCounter;
		U16			StackOwerReset;
		U8			StackUnderReset;
		U8			Watchdog;
		S16			CalibrePow;
		S16			CalibreBat;
		U16			data1;
		U16			data2;
	} FLASH_DATA_STRUCT;

	void	FlashDataInit	(void);
	void	FlashDataBurn	(void);

	extern	FLASH_DATA_STRUCT	DataFromFlash;
	#define	FLASH_FLAGS			DataFromFlash.flags.word
	#define	FLASH_FLAG_GSM		DataFromFlash.flags.flag_0
	#define	FLASH_FLAG_SMS		DataFromFlash.flags.flag_1
	#define	FLASH_FL_GSM_ACTIVE	DataFromFlash.flags.flag_2
	#define	ROM_DATA_CLEARED	DataFromFlash.flags.flag_15
#endif	// ROM_DATA


extern	volatile FLAGS		SysFlags;
#define	SYSTEM_FLAGS		SysFlags.word
#define	FL_GSM_SEC			SysFlags.flag_0		// Seconds 1Hz flag for GSM
#define	FL_TEST_MANUAL		SysFlags.flag_1		// Switch off gsm stateMachine (for debug)
#define	FL_GPRS_REC			SysFlags.flag_2		// GPRS packet START received
#define	FL_GPRS_OFFLINE		SysFlags.flag_3		// Offline run for tests
#define	FL_TCP_READY		SysFlags.flag_4		// At least 1 Tcp packet ready to send
#define	FL_WEB_ONLINE		SysFlags.flag_5		// While website online, send gps coordinates regularly
#define	FL_WIFI_REC			SysFlags.flag_6		// WIFI packet START received
#define	FL_WIFI_OFFLINE		SysFlags.flag_7		// WIFI packet START received
#define	FL_BOOT				SysFlags.flag_8		// Boot Process started
#define	FL_BOOT_TCP			SysFlags.flag_9		// Boot via TCP Process started
#define	FL_TCP_RESEND		SysFlags.flag_10	// Resend of lost Tcp Packet requested
#define	FL_POWER_CHANGES	SysFlags.flag_11	// ADC Power Changes
#define	FL_POWER_OFF    	SysFlags.flag_12	// Device Power Off sequence
#define	FL_BOOT_PROCESS		SysFlags.flag_13	// Boot Process started

enum ppsConfig
{
	pps_system = 0,
	pps_default
};
void InitPorts  (void);
void PPSconfig  (enum ppsConfig ppsSwitch);
void PowerOff   (void);
void Reset      (void);

// PERIPHERAL PIN SELECT OUTPUT SOURCES
#define IO_PORT		0		//	RPn tied to default port pin
#define IO_C1OUT	1		//	RPn tied to Comparator1 Output
#define IO_C2OUT	2		//	RPn tied to Comparator2 Output
#define IO_U1TX		3		//	RPn tied to UART1 Transmit
#define IO_U1RTS	4		//	RPn tied to UART1 Ready To Send
#define IO_U2TX		5		//	RPn tied to UART2 Transmit
#define IO_U2RTS	6		//	RPn tied to UART2 Ready To Send
#define IO_SDO1		7		//	RPn tied to SPI1 Data Output
#define IO_SCK1OUT	8		//	RPn tied to SPI1 Clock Output
#define IO_SS1OUT	9		//	RPn tied to SPI1 Slave Select Output
#define IO_SDO2		10		//	RPn tied to SPI2 Data Output
#define IO_SCK2OUT	11		//	RPn tied to SPI2 Clock Output
#define IO_SS2OUT	12		//	RPn tied to SPI2 Slave Select Output
#define IO_OC1		18		//	RPn tied to Output Compare 1
#define IO_OC2		19		//	RPn tied to Output Compare 2
#define IO_OC3		20		//	RPn tied to Output Compare 3
#define IO_OC4		21		//	RPn tied to Output Compare 4
#define IO_OC5		22		//	RPn tied to Output Compare 5
#define IO_OC6		23		//	RPn tied to Output Compare 6 
#define IO_OC7		24		//	RPn tied to Output Compare 7
#define IO_OC8		25		//	RPn tied to Output Compare 8
#define IO_U3TX		28		//	RPn tied to UART3 Transmit
#define IO_U3RTS	29		//	RPn tied to UART3 Ready To Send
#define IO_U4TX		30		//	RPn tied to UART4 Transmit
#define IO_U4RTS	31		//	RPn tied to UART4 Ready To Send
#define IO_SDO3		32		//	RPn tied to SPI3 Data Output
#define IO_SCK3OUT	33		//	RPn tied to SPI3 Clock Output
#define IO_SS3OUT	34		//	RPn tied to SPI3 Slave Select Output
#define IO_OC9		35		//	RPn tied to Output Compare 9
#define IO_C3OUT	36		//	RPn tied to Comparator3 Output

// PERIPHERAL PIN SELECT OUTPUT REGISTERS
#define PPS_OUT_B0		_RP0R
#define PPS_OUT_B1		_RP1R
#define PPS_OUT_B2		_RP13R
#define PPS_OUT_B3
#define PPS_OUT_B4		_RP28R
#define PPS_OUT_B5		_RP18R
#define PPS_OUT_B6		_RP6R
#define PPS_OUT_B7		_RP7R
#define PPS_OUT_B8		_RP8R
#define PPS_OUT_B9		_RP9R
#define PPS_OUT_B10
#define PPS_OUT_B11
#define PPS_OUT_B12
#define PPS_OUT_B13
#define PPS_OUT_B14		_RP14R
#define PPS_OUT_B15		_RP29R

#define PPS_OUT_C12
#define PPS_OUT_C13
#define PPS_OUT_C14		// RPI37 - input only
#define PPS_OUT_C15

#define PPS_OUT_D0		_RP11R
#define PPS_OUT_D1		_RP24R
#define PPS_OUT_D2		_RP23R
#define PPS_OUT_D3		_RP22R
#define PPS_OUT_D4		_RP25R
#define PPS_OUT_D5		_RP20R
#define PPS_OUT_D6
#define PPS_OUT_D7
#define PPS_OUT_D8		_RP2R
#define PPS_OUT_D9		_RP4R
#define PPS_OUT_D10		_RP3R
#define PPS_OUT_D11		_RP12R

#define PPS_OUT_E0
#define PPS_OUT_E1
#define PPS_OUT_E2
#define PPS_OUT_E2
#define PPS_OUT_E3
#define PPS_OUT_E4
#define PPS_OUT_E5
#define PPS_OUT_E6
#define PPS_OUT_E7

#define PPS_OUT_F0
#define PPS_OUT_F1
#define PPS_OUT_F3		_RP16R
#define PPS_OUT_F4		_RP10R
#define PPS_OUT_F5		_RP17R
#define PPS_OUT_F7

#define PPS_OUT_G2
#define PPS_OUT_G3
#define PPS_OUT_G6		_RP21R
#define PPS_OUT_G7		_RP26R
#define PPS_OUT_G8		_RP19R
#define PPS_OUT_G9		_RP27R



// PERIPHERAL PIN SELECT INTPUT REGISTERS
#define	PPS_IN_VSS		0b11111		//	Periferal INput tied to Vss by default
#define PPS_IN_B0		0
#define PPS_IN_B1		1
#define PPS_IN_B2		13
#define PPS_IN_B3
#define PPS_IN_B4		28
#define PPS_IN_B5		18
#define PPS_IN_B6		6
#define PPS_IN_B7		7
#define PPS_IN_B8		8
#define PPS_IN_B9		9
#define PPS_IN_B10
#define PPS_IN_B11
#define PPS_IN_B12
#define PPS_IN_B13
#define PPS_IN_B14		14
#define PPS_IN_B15		29

#define PPS_IN_C12
#define PPS_IN_C13
#define PPS_IN_C14		37			// i
#define PPS_IN_C15

#define PPS_IN_D0		11
#define PPS_IN_D1		24
#define PPS_IN_D2		23
#define PPS_IN_D3		22
#define PPS_IN_D4		25
#define PPS_IN_D5		20
#define PPS_IN_D6
#define PPS_IN_D7
#define PPS_IN_D8		2
#define PPS_IN_D9		4
#define PPS_IN_D10		3
#define PPS_IN_D11		12

#define PPS_IN_E0
#define PPS_IN_E1
#define PPS_IN_E2
#define PPS_IN_E2
#define PPS_IN_E3
#define PPS_IN_E4
#define PPS_IN_E5
#define PPS_IN_E6
#define PPS_IN_E7

#define PPS_IN_F0
#define PPS_IN_F1
#define PPS_IN_F3		16
#define PPS_IN_F4		10
#define PPS_IN_F5		17
#define PPS_IN_F7

#define PPS_IN_G2
#define PPS_IN_G3
#define PPS_IN_G6		21
#define PPS_IN_G7		26
#define PPS_IN_G8		19
#define PPS_IN_G9		27

#if		(HARDWARE == HW_AUTO)
	#include	"Ports_Auto01.h"
#elif	(HARDWARE == HW_SEE)
	#include	"Ports_See01.h"
#elif	(HARDWARE == HW_HOME)
	#include	"Ports_Home.h"
#endif

//------------------------------------------------------------------------------------
#define LED1_ON()		LED1			= 1
#define LED1_TGL()		LED1			= !LED1
#define LED1_OFF()		LED1			= 0
#define LED2_ON()		LED2			= 1
#define LED2_TGL()		LED2			= !LED2
#define LED2_OFF()		LED2			= 0
#define LED3_ON()		LED3			= 1
#define LED3_TGL()		LED3			= !LED3
#define LED3_OFF()		LED3			= 0
#define LED4_ON()		LED4			= 1
#define LED4_TGL()		LED4			= !LED4
#define LED4_OFF()		LED4			= 0

#define GPS_POW_ON()	GPS_POWER		= 1
#define GPS_POW_OFF()	GPS_POWER		= 0
#define GSM_PK_ON()		GSM_P_KEY		= 0
#define GSM_PK_OFF()	GSM_P_KEY		= 1
#define GSM_DTR_ON()	GSM_DTR			= 1
#define GSM_DTR_OFF()	GSM_DTR			= 0
#define GSM_RTS_ON()	GSM_RTS			= 0
#define GSM_RTS_OFF()	GSM_RTS			= 1


#ifdef		BOOT_LOADER	// __________________________________________________
	// BootLoader Ports initialisation	*************************************
	#define	BOOT_LOADER_PORT_INIT()		InitPort_B();						\
										InitPort_F()
	// BootLoader Led	*****************************************************
	#define	LED_BOOT					LED1

	// BootLoader Uart defines	*********************************************
	#if		(BOOT_UART == UART_1)
	#elif	(BOOT_UART == UART_2)
	#elif	(BOOT_UART == UART_3)
		#define	BOOT_BAUDRATE_H			(((32000000 /2 / 4) / 115200))
		#define	UART_RX_EN				U3MODEbits.UARTEN
		#define	UART_BRGH				U3MODEbits.BRGH
		#define	UART_TX_EN				U3STAbits.UTXEN
		#define	UART_TX_FULL			U3STAbits.UTXBF
		#define	UART_RX_EMPTY			!U3STAbits.URXDA
		#define	UART_TX_PROGR			!U3STAbits.TRMT
		#define	UART_RX_REC				U3STAbits.URXDA
		#define	UART_TX_DATA			U3TXREG
		#define	UART_RX_DATA			U3RXREG
		#define	BOOT_LOADER_PPS_UART()	_U3RXR		= SET_RX;				\
										SET_TX		= IO_U3TX
		#define	BOOT_LOADER_INIT_UART()	U3MODE		= 0;					\
										U3STA		= 0;					\
										UART_BRGH	= 1;					\
										U3BRG		= BOOT_BAUDRATE_H;		\
										_U3TXIF		= 0;					\
										_U3RXIF		= 0;					\
										_U3TXIE		= 0;					\
										_U3RXIE		= 0;					\
										UART_RX_EN	= 1;					\
										UART_TX_EN	= 1
	#else
		#define	BOOT_LOADER_PPS_UART()
		#define	BOOT_LOADER_INIT_UART()
	#endif

	// BootLoader Spi Memory defines	*************************************
	#define	BOOT_MEM_CS_ON()		{register U16 i = 10; while (--i)MEM_CS = 0;}
	#define	BOOT_MEM_CS_OFF()		{register U16 i = 10; while (--i)MEM_CS = 0; MEM_CS = 1;}
	#define	BOOT_MEM_CS_IS_ON()		(!MEM_CS)
	#if		(MEMORY1_PORT == SPI_1)
		#define	BOOT_LOADER_PPS_MEM_SPI()	_SDI1R	= MEM_SI;				\
											MEM_SCK	= IO_SCK1OUT;			\
											MEM_SO	= IO_SDO1
		#define	BOOT_SPI_BUF			SPI1BUF
		#define	BOOT_SPI_RX_FIFO_EMPTY	SPI1STATbits.SRXMPT
		#define	BOOT_SPI_SHIFT_EMPTY	SPI1STATbits.SRMPT
		#define	BOOT_LOADER_INIT_MEM()	SPI1STAT	= 0b0000000000010100;	\
										SPI1CON1	= 0b0000000001111111;	\
										SPI1CON2	= 0b0000000000000001;	\
										_SPI1IF		= 0;					\
										_SPI1IE		= 0;					\
										SPI1STATbits.SPIEN	= 1
	#elif	(MEMORY1_PORT == SPI_2)
		#define	BOOT_LOADER_PPS_MEM_SPI()	_SDI2R	= MEM_SI;				\
											MEM_SCK	= IO_SCK2OUT;			\
											MEM_SO	= IO_SDO2
		#define	BOOT_SPI_BUF			SPI2BUF
		#define	BOOT_SPI_RX_FIFO_EMPTY	SPI2STATbits.SRXMPT
		#define	BOOT_SPI_SHIFT_EMPTY	SPI2STATbits.SRMPT
		#define	BOOT_LOADER_INIT_MEM()	SPI2STAT	= 0b0000000000010100;	\
										SPI2CON1	= 0b0000000001111111;	\
										SPI2CON2	= 0b0000000000000001;	\
										_SPI2IF		= 0;					\
										_SPI2IE		= 0;					\
										SPI2STATbits.SPIEN	= 1
	#endif
#endif	// BOOT_LOADER	_____________________________________________________

#endif


