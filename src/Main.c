#include		"Typedef.h"
#include		"Config.h"

#include		"Ports.h"
#include		"Timers.h"
#include		"Memory.h"
#include		"Gps.h"
#include		"Gsm.h"
#include		"Tcp.h"
#include		"Adc.h"
#include		"Input.h"
#include		"W5100.h"
#include		"system.h"
#include		"Wifi.h"
#include		"Lcd.h"

#include		<stdio.h>


extern void Usb_SendText (char* str);

int main (void)
{
	InitPorts();
	OSCconfig(Osc_32_MHz);
	PPSconfig(pps_system);

	#if	(LCD != LCD_NO)
		LcdInit();
	#endif
	#ifdef	ROM_DATA_ADDR
		FlashDataInit();
	#endif
	#if (LCD == LCD_RGB)
		LcdSetColor(GREY);
	#endif
	#if	(LCD > LCD_NO)
		LcdString(0, 48, DEVICE, font_ss16);
//		LcdString(5, 5, "En»elîtis", font_Seg24);
//		LcdString(2, 0, "Mazais", font_Seg24);
	#endif

	AdcInit();
	MemInit();
	#if (INPUT != INPUT_NO)
		InputInit();
	#endif
	#ifdef		USB_PORT
		Usb_Init();
	#endif
	#if	((TEST != TEST_GSM) && (TEST != TEST_WIFI))
//		Gsm_On();
	#endif
	#if	(TEST != TEST_GPS)
//		Gps_On();
	#endif
	#ifdef UART4_WIFI
		#if (TEST != TEST_WIFI)
			Wifi_On();
		#endif
	#endif

	#ifdef		USB_PORT
//		Usb_SendText("Start");
	#endif

	while(true)
	{
		#ifdef		USB_PORT
			Usb_RegularFunctions();
		#endif
		#if (LCD == LCD_RGB)
			LcdRgbTest();
		#endif
		#if	(TEST == TEST_WIFI)
			Wifi_Test();
		#endif

//		if (FL_BOOT_TCP)	{	Tcp_BootLoaderProcess();	continue;	}
		if (FL_TCP_RESEND)		Tcp_PacketResend();
        if (FL_POWER_OFF)       PowerOff();
//		if (FL_POWER_CHANGES)	Tcp_AdcPacket();

	#if	(TEST != TEST_NO)
		if (!FL_BOOT_PROCESS)
		{
			static U8 lastSec = 0;
			if (lastSec != SysTime.sec)
			{
				lastSec = SysTime.sec;
				char txt[48];
				sprintf(txt, "Rel %s ", (EXT_OUT2)? "On " : "Off");
				LcdString(0, 0, txt, font_7x5);
				Adc_GetAcVal(txt);
//				Adc_GetAllVal(txt);
				Usb_SendText(txt);
				#if ((LCD == LCD_GREEN) || (LCD == LCD_BLUE))
					Adc_GetAcVal(txt);	// 2 rows !!!
//					Usb_SendText(txt);
					LcdString(2, 0, txt, font_7x5);
					Adc_GetPowBat(txt);	// 2 rows !!!
//					Usb_SendText(txt);
					LcdString(4, 0, txt, font_7x5);
				#endif
			}
		}
	#endif
	#ifdef	ETH_W5100
		W51_Run();
		#if (TEST == TEST_W5100)
			W51_Test();
		#endif
	#endif
	}
	return 0;
}

