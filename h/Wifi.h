#ifndef	_wifi_h_
#define	_wifi_h_

#include	"Typedef.h"

void Wifi_On		(void);
void Wifi_Off		(void);
U8	 Wifi_Connected	(void);
void Wifi_Reset		(void);
U16	 Wifi_AUTO		(void);
#if	(TEST == WIFI_TEST)
	void Wifi_Test				(void);
	void Wifi_SendCommand		(char* str);
	void Wifi_BaudrateChange	(void);
#endif

#endif	// _wifi_h_

