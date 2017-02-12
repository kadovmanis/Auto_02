#ifndef	_wifi_defines_h_
#define	_wifi_defines_h_

/***************************************	WIFI module Commands	**************************************************
AT									AT
AT+RST								Reset
AT+GMR								Firmware version
AT+CWLAP							List Access Points
AT+CWJAP?							Join Access Point				Query
AT+CWJAP="SSID","Password"											Ok
AT+CWJAP="Peles","mousefamily"
AT+CWQAP=?							Quit Access Point				Query
AT+CWQAP															Ok
AT+CIFSR							Get IP Address
AT+CWSAP?															Query
AT+CWSAP=<ssid>,<pwd>,<chl>, <ecn>	Set Parameters of Access Point	ssid, pwd chl = channel, ecn = encryption
AT+CWMODE?							WiFi Mode						Query
AT+CWMODE=<mode>													1: Client, 2: Access_point, 3: Both
AT+CIPSTART=?						Set up TCP or UDP connection	Query
AT+CIPSTART=[<id>]"type","addr",port								id: 0-4(if CIPMUX=1), type: TCP/UDP, addr: IP/dns, port: port
AT+CIPSTART="TCP","peles.linkpc.net",10123
AT+CIPMUX?							TCP/UDP Connections				Query
AT+CIPMUX=<mode>													0: Single, 1: Multiple
AT+CWLIF							Check join devices' IP			as Access_point
AT+CIPSTATUS						TCP/IP Connection Status
AT+CIPSEND=[<id>],length			Send TCP/IP data				(id if CIPMUX=1 only)
>.....
AT+CIPCLOSE[=<id>]					Close TCP / UDP connection		(id if CIPMUX=1 only)
AT+CIPSERVER=<mode>[,<port>]		Set as server					0: close, 1: open server mode
AT+CIPSTO?							Set the server timeout			Query
AT+CIPSTO=<time>													time: 0-28800 in seconds
	AT+CIOBAUD?						Baud Rate*						in V0.9.2.2
AT+CIFSR							Check IP address				AT+CIFSR 192.168.0.106<CR><LF>OK
AT+CIUPDATE							Firmware Upgrade (from Cloud)	1. +CIPUPDATE:1 found server
																	2. +CIPUPDATE:2 connect server
																	3. +CIPUPDATE:3 got edition
																	4. +CIPUPDATE:4 start update
	+IPD							Received data					+IPD,[<id>],len:<data> (id if CIPMUX=1 only)
																	+IPD,100:ddddddddd...
*********************************************************************************************************************/

#include	"Typedef.h"
#include	"Ports.h"
#include	"Uart4.h"
#include	"Wifi.h"
#include	"Tcp.h"
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>

#define		AP_CHECK_SEC			0x0000001F	// 31 sec
#define		WIFI_OK_TIMEOUT_S		120			// 2 min
#define		MAX_VISIBLE_AP			20
#define		WIFI_LED				LedStatus3
#define		WIFI_MODE_CLIENT		1
#define		WIFI_MODE_ACC_P			2
#define		WIFI_MODE_BOTH			3

#define		WIFI_MODE				WIFI_MODE_BOTH

#define		Ack						u8l
#define		Status					u8h
#define		ackOk					flag_0		// "OK"		received
#define		ackErr					flag_1		// "Error"	received
#define		ackData					flag_2		// data		received
#define		rec						flag_3		// serial data received
#define		setup					flag_10		// module should be setup
#define		conected				flag_11		// Wifi Connected to server
#define		ip						flag_12		// Wifi ip address getted
#define		ap						flag_13		// Wifi Connected to Access point
#define		com						flag_14		// Wifi module comunicate
#define		pow						flag_15		// Wifi Power enabled

#define		WIFI_STATUS_POWER		0b10000000	// Power enabled 
#define		WIFI_STATUS_COM			0b11000000	// Module comunicate
#define		WIFI_STATUS_AP			0b11100000	// Connected to Access point
#define		WIFI_STATUS_IP_OK		0b11110000	// IP address received
#define		WIFI_STATUS_CONNECTED	0b11111000	// Connected to Server

typedef enum
{
	WifiState_Undefined = 0,
	WifiState_Idle,
	WifiState_Off,
	WifiState_PowerOn,
	WifiState_ComOn,
	WifiState_CwMode,
	WifiState_NoAp,
	WifiState_ListAp,
	WifiState_ListApRec,
	WifiState_CWJAP,
	WifiState_SendPacket,
	WifiState_SendOk,
	WifiState_Setup,
	WifiState_,
	WifiState_Disconnect,
	WifiState_Connect,
	WifiState_Closed
} WIFI_STATE;

typedef struct
{
	char*		text;
	U8			size;
	WIFI_STATE	newState;
	U16			newTimeout;
	U16			orFlags;
} WIFI_RESPONSE;

const WIFI_RESPONSE WifiResponses [] =
{	//Response text			text size					state (0-no switch)		Timeout	flags OR	pow, com, ap, ip, conected
	{"OK",					(sizeof("OK") - 1),					0,					0,		0b0000000000000001	},	// bit Ok
	{"ERROR",				(sizeof("ERROR") - 1),				0,					0,		0b0000000000000010	},	// bit Err
	{"FAIL",				(sizeof("FAIL") - 1),				0,					0,		0b0000000000000010	},	// bit Err
	{"No AP",				(sizeof("No AP") - 1),				WifiState_NoAp,		0,		0b0000000000000010	},	// bit Err
	{"ready",				(sizeof("ready") - 1),				WifiState_ComOn,	0,		0b1100000000000000	},	// bit pow, com
	{"WIFI CONNECTED",		(sizeof("WIFI CONNECTED") - 1),		0,					200,	0b1110000000000000	},	// bit pow, com, ap
	{"WIFI GOT IP",			(sizeof("WIFI GOT IP") - 1),		0,					0,		0b1111000000000000	},	// bit pow, com, ap, ip
	{"WIFI DISCONNECT",		(sizeof("WIFI DISCONNECT") - 1),	WifiState_Disconnect,0,		0b0000000000000000	},	// bit 
	{"CONNECT",				(sizeof("CONNECT") - 1),			0,					0,		0b1111100000000000	},	// bit pow, com, ap, ip, conected
	{"CLOSED",				(sizeof("CLOSED") - 1),				WifiState_Closed,	0,		0b0000000000000000	},	// bit 
	{"link is not valid",	(sizeof("link is not valid") - 1),	WifiState_Closed,	0,		0b0000000000000000	},	// bit 
	{"IP ERROR",			(sizeof("IP ERROR") - 1),			0,					0,		0b0000000000000000	},	// bit 
	{"STATUS:",				(sizeof("STATUS:") - 1),			0,					0,		0b0000000000000000	},	// bit 
	{"+CIFSR:APIP,\"",		(sizeof("+CIFSR:APIP,\"") - 1),		0,					0,		0b0000000000000000	},	// bit 
	{"+CIFSR:APMAC,\"",		(sizeof("+CIFSR:APMAC,\"") - 1),	0,					0,		0b0000000000000000	},	// bit 
	{"+CIFSR:STAIP,\"",		(sizeof("+CIFSR:STAIP,\"") - 1),	0,					0,		0b0000000000000000	},	// bit 
	{"+CIFSR:STAMAC,\"",	(sizeof("+CIFSR:STAMAC,\"") - 1),	0,					0,		0b0000000000000000	},	// bit 
	{"+CWJAP:\"",			(sizeof("+CWJAP:\"") - 1),			WifiState_CWJAP,	0,		0b0000000000000000	},	// bit 
	{"+IPD,",				(sizeof("+IPD,") - 1),				0,					0,		0b0000000000000000	},	// bit 
	{"Recv ",				(sizeof("Recv ") - 1),				0,					0,		0b0000000000000000	},	// bit 
	{"+CWLAP:",				(sizeof("+CWLAP:") - 1),			0,					0,		0b0000000000000100	},	// bit 
	{"+CWMODE:",			(sizeof("+CWMODE:") - 1),			0,					0,		0b0000000000000100	},	// bit data
	{"SEND OK",				(sizeof("SEND OK") - 1),			WifiState_SendOk,	0,		0b0000000000000100	}	// bit 
};

typedef struct
{
	char		ssid[60];
	char		psw[64];
	S16			rssi;
	U16			ecn;
} WIFI_ACCESS_POINT;

const WIFI_ACCESS_POINT WifiAccessPoints [] =
{	// SSID						PSW
	{"Peles",				"mousefamily",		0, 0},	// berģi
	{"Mazais",				"saulceriite7",		0, 0},	// jugla
	{"Skabarzi",			"",					0, 0},	// lauki
	{"Dovmani",				"noliktava",		0, 0},	// mežciems
	{"BMW E46",				"leounanda",		0, 0},	// leo auto
	{"Leo 3G S",			"leounanda",		0, 0},	// leo telefons
	{"Introskip",			"Enkei2014",		0, 0},	// leo darbs
	{"HUAWEY-Pekseni",		"8080808080",		0, 0},	// Pekseni
	{"Atnes alu",			"",					0, 0}	// leo raunas
};
/*
Debug:    +CWLAP:(3,"LMT-D069",-64,"88:ce:fa:ef:d0:69",1,-6)
 Debug:    +CWLAP:(4,"ALHN-9947",-80,"18:4a:6f:64:71:8c",1,-14)
 Debug:    +CWLAP:(4,"Stiiga",-77,"54:a6:19:9c:7f:5c",1,-39)
 Debug:    +CWLAP:(3,"TP-LINK_92CC44",-66,"14:cc:20:92:cc:44",3,-17)
 Debug:    +CWLAP:(3,"Mazais",-52,"a0:f3:c1:b5:af:00",3,-21)
 Debug:    +CWLAP:(3,"virus stop",-89,"b0:48:7a:97:9e:1a",4,-21)
 Debug:    +CWLAP:(4,"Jurgis.",-56,"10:fe:ed:cc:5d:2e",6,-46)
 Debug:    +CWLAP:(4,"Janis",-71,"64:70:02:c8:04:44",6,-29)
 Debug:    +CWLAP:(3,"JuglaP",-87,"00:19:5b:05:3b:b4",6,-21)
 Debug:    +CWLAP:(3,"Alpha_Juglas",-54,"c4:6e:1f:7a:d6:86",9,-24)
 Debug:    +CWLAP:(4,"unnamed_wifi",-81,"30:85:a9:37:90:70",11,-34)
 Debug:    +CWLAP:(4,"ALHN-F7D4",-84,"54:a6:19:ec:d8:6c",11,-57)

	ready
	ATE0
	OK
	WIFI DISCONNECT
	No AP
	OK

	WIFI CONNECTED
	WIFI GOT IP
	OK
    +CWJAP:"Mazais","a0:f3:c1:b5:af:00",3,-55
	OK
	__________________________
	ready
	ATE0
	OK
	WIFI CONNECTED
	No AP
	OK
	WIFI GOT IP
	+CWLAP:(4,"Stiiga",-82,"54:a6:19:9c:7f:5c",1,-37)


 Debug:    AT+CWMODE?
 Debug:    +CWMODE:2
 Debug:    OK

link is not valid
*/
#endif	// _wifi_defines_h_

