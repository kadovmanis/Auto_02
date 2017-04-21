#ifndef		__DEFINE__
#define		__DEFINE__

//	Vecomamma	67394376	Màjas tàlrunis PAMATA
//	Audi		28637972	M2M 1
//	Bmw Leo		26141841	M2M 1
//	Blue_1		25747292	M2M 1
//	Blue_2		25767845	M2M 1

#ifndef __PIC24FJ128GB206__
	#define	__PIC24FJ128GB206__
#endif
#include <xc.h>

#define		HW_AUTO				1
#define		HW_SEE				2
#define		HW_HOME				3
#define		HARDWARE			HW_SEE
#define		VERSION				"02"
//#define		MAC_ADDRESS			{0xFF, 0xDD, 0xFF, 0xFF, 0xFF, 0xFF}
//#define		MY_IP				{192, 168, 2, 50}
//#define		SUBNET_MASK			{255, 255, 255, 0}
//#define		GATEWAY				{192, 168, 2, 1}

// _______________________________ Possible arguments ___________________________
#define		TEST_NO				0
#define		TEST_MEMORY			1
#define		TEST_MEM_WRITE		2
#define		TEST_PRINTER		3
#define		TEST_GSM			4
#define		TEST_GPS			5
#define		TEST_WIFI			6
#define		TEST_ADC			7
#define		TEST_W5100			10
#define		INPUT_NO			0
#define		INPUT_SEE			1

#define		ON_LINE				0
#define		OFF_LINE			1

#define		MP25P40				8		// 4Mbit	512kB
#define		MP25P32				64		// 32Mbit	4MB
#define		MP25P64				128		// 64Mbit	8MB		256(page) * 256(pages in sector) * 128(sectors)
#define		S25FL512S			1024	// 128Mbit	64MB	512(page) * 512(pages in sector) * 256(sectors)
#define		NO_PORT				0
#define		SPI_1				1
#define		SPI_2				2
#define		SPI_3				3
#define		UART_1				1
#define		UART_2				2
#define		UART_3				3
#define		LCD_NO				0
#define		LCD_GREEN			1
#define		LCD_BLUE			2
#define		LCD_RGB				3
#define		MICROCHIP_STRING	{'M','i','c','r','o','c','h','i','p',' ','T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'}
#define		MICROCHIP_SIZE		25
#define		BOOTLOADER_STRING	{'U','S','B',' ','H','I','D',' ','B','o','o','t','l','o','a','d','e','r'}
#define		BOOTLOADER_SIZE		18
#define		AUTO_STRING			{'_','_','A','u','t','o','_','_'}
#define		AUTO_SIZE			8
#define		SEE_STRING			{'_','_','S','e','e','_','_'}
#define		SEE_SIZE			7
#define		HOME_STRING			{'_','_','H','o','m','e','_','_'}
#define		HOME_SIZE			8

#define		APN					"internet.lmt.lv"
//#define	DNS_1				"159.148.60.20"
#define		DNS_1				"212.93.96.2"
//#define	DNS_2				"195.2.96.2"
#define		DNS_2				"212.93.96.4"

#define		SERVER				"peles.linkpc.net"
#define		PORT				"10123"

// _____________________________________ DEFINE _________________________________
#if		(HARDWARE == HW_HOME)
	#define		DEVICE						"Home"
	#define		DEVICE_ID					0x0001
	#define		LED_COUNT					4
//	#define		LCD							LCD_RGB
	#define		LCD							LCD_GREEN
//	#define		LCD_BLUE
//	#define		LCD_REVERSE
	#define		INPUT						INPUT_NO
	#define		ETH_W5100
	#define		BOOT_LOADER
	#define		BOOT_UART					UART_3			// Define			Uart3 used for bootloader
	#define		MEMORY1_PORT				SPI_2
	#define		USB_PORT
	#define		USB_DESCRIPTOR_PRODUCT		HOME_STRING
	#define		USB_DESCRIPTOR_PRODUCT_SIZE	HOME_SIZE
	#define		ROM_DATA_ADDR				0x000C00		// Non volatile data struct in programm ROM
	#define		ROM_DATA_SIZE				(_FLASH_PAGE*2)	// (_FLASH_PAGE*2)
#elif	(HARDWARE == HW_AUTO)
	#define		DEVICE						"Auto_01"
	#define		DEVICE_ID					0x0101
	#define		LED_COUNT					4
	#define		LCD							LCD_NO
	#define		INPUT						INPUT_NO
	#define		UART1_GPS
	#define		UART4_WIFI
	#define		WIFI_WAKE_UP_S				300				// check Wifi every 5min
	#define		GPRS						ON_LINE
	#define		BOOT_LOADER
	#define		BOOT_UART					UART_3			// Define			Uart3 used for bootloader
	#define		MEMORY1_PORT				SPI_1
//	#define		MEMORY						MP25P64
	#define		USB_PORT
	#define		USB_DESCRIPTOR_PRODUCT		AUTO_STRING
	#define		USB_DESCRIPTOR_PRODUCT_SIZE	AUTO_SIZE
	#define		ROM_DATA_ADDR				0x000C00		// Non volatile data struct in programm ROM
	#define		ROM_DATA_SIZE				(_FLASH_PAGE*2)	// (_FLASH_PAGE*2)
#elif	(HARDWARE == HW_SEE)
	#define		DEVICE						"See_01"
	#define		DEVICE_ID					0x1000
	#define		LED_COUNT					2
	#define		LCD							LCD_NO
	#define		INPUT						INPUT_SEE
	#define		GPRS						ON_LINE
	#define		BOOT_LOADER
	#define		BOOT_UART					UART_3			// Define			Uart3 used for bootloader
	#define		MEMORY1_PORT				SPI_1
//	#define		MEMORY						MP25P64
	#define		USB_PORT
	#define		USB_DESCRIPTOR_PRODUCT		SEE_STRING
	#define		USB_DESCRIPTOR_PRODUCT_SIZE	SEE_SIZE
	#define		ROM_DATA_ADDR				0x000C00		// Non volatile data struct in programm ROM
	#define		ROM_DATA_SIZE				(_FLASH_PAGE*2)	// (_FLASH_PAGE*2)
#endif

#define		TEST							TEST_NO 		// Send USB debug strings to test specific software modules
#define		DEVICE_NAME						DEVICE

#define		USB_DESCRIPTOR_MANUFACTURER			MICROCHIP_STRING
#define		USB_DESCRIPTOR_MANUFACTURER_SIZE	MICROCHIP_SIZE
#define		USB_VENDOR_ID						0x04D8
#define		USB_PRODUCT_ID						0x003F

// ______________________________ Specific HW Defines ___________________________
#include	"p24Fxxxx.h"				// C:\Program Files\Microchip\MPLAB C30\support\PIC24F\h
#define	LED_TIM			LedStatus1
#define	LED_USB			LedStatus1
#define	LED_GSM			LedStatus2
#define	LED_WIFI		LedStatus3
#define	LED_HID			LedStatus3		// for tests
#define	LED_GPS			LedStatus4

//#include	"p24Fxxxx.h"				// C:\Program Files\Microchip\MPLAB C30\support\PIC24F\h
#define	GPS_INTERRUPT	__attribute__((interrupt, no_auto_psv)) _PMPInterrupt
#define	GPS_IF			_PMPIF				// Parallel Master Port Interrupt used for
#define	GPS_IE			_PMPIE				// GPS_EXT Interrupt


#define	GSM_INTERRUPT	__attribute__((interrupt, no_auto_psv)) _SI2C1Interrupt
#define	GSM_IF			_SI2C1IF
#define	GSM_IE			_SI2C1IE
#define	WIFI_INTERRUPT	__attribute__((interrupt, no_auto_psv)) _SI2C2Interrupt
#define	WIFI_IF			_SI2C2IF
#define	WIFI_IE			_SI2C2IE

#endif	// __DEFINE__

