#ifndef		_debug_h_
#define		_debug_h_

#include	"Typedef.h"

#include	"Wifi.h"


//#if	(defined USB_PORT) || (defined UART4_WIFI)

//#ifdef	UART4_WIFI
//	extern void Tcp_SendText (char* str);
//
//#endif
#ifdef	USB_PORT
	#include	<stdio.h>
	extern void Usb_SendText (char* str);
  #ifdef	UART4_WIFI_
	#include	"Wifi.h"
	#include	"Tcp.h"
	#define		DebugPrint(X)				\
				{	if (Wifi_Connected())	\
						Tcp_SendText(X);	\
					Usb_SendText(X);		}
  #else
	#define	DebugPrint		Usb_SendText
  #endif
	#define	DebugSprintf	sprintf
#else
	#define	DebugPrint(X)
	#define	DebugSprintf(X)
#endif

#endif	//	_debug_h_

