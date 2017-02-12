#ifndef	_gsm_defines_h_
#define	_gsm_defines_h_

#include	"Typedef.h"
#include	"Gsm.h"
#include	"Uart2.h"

#define		GPRS_OK_TIMEOUT_S		360			// 6	min
#define		GPRS_RESTERT_TIMEOUT_S	600			// 10	min
#define		SMS_WAIT_TIMEOUT_10mS	50			// 0.5	sec
/*
+37129279661	Kaspars
+37126141841	Leo_bmwNew

start	Starta simbos STX 0x02
len		paketes garums ieskaitot start un len
crc		Kontrolsumma izrekinata sakot ar type un lidz pedejam paketes baitam (peec len)
type	paketes tips
id		iekartas tips (3 biti 0:7) un nummurs (13 biti 0:8191)
nr		paketes nummurs pec kartas pakešu identifikacijai
time	kompresets laiks


0 pakete - starta pakete
_____________________________________________________________
Laiks = 32 biti = sekundes 100 gadu intervalaa
pozicija	sekunžu skaits - šo skaitli reizina
un beigas visu saskaita
	sec			1				
	min			60				60 sekundes
	h			3600			60 minutes
	diena		86,400			24 stundas
	meen		2,678,400		31 diena
	gads		32,140,800		12 meneši
atpakal - dala sakot ar gadu, atlikumu dala talak, lidz tiek lidz sekundem

	sec			60				6b
	min			60				6b
	h			24				5b
	d			31				5b
	m			12				4
	g			100				7	(64 - 6b)

ggggggMM MMdddddh hhhhmmmm mmssssss
ggggggdd dddhhhhh MMMMmmmm mmssssss
_____________________________________________________________
AT+CIPSCONT
+CDNSORIP:1		CONNECT WITH IP ADDRESS (0) OR DOMAIN NAME SERVER (1)
+CIPSPRT:1		SET PROMPT OF ‘>’ WHEN SENDING DATA 0-no >, no Send Ok; 1-yes >, yes Send OK; 2-no >, no Send Ok
+CIPHEAD:0		ADD AN IP HEADER WHEN RECEIVING DATA
+CIPFLP:0		FIX LOCAL PORT (1 by default)when establishing a new connection (not fixed port speed up connection)
+CIPSRIP:0		SHOW WHERE RECEIVED DATA FROM (IP:Port)
+CIPCSGP:1		SET CSD (0) OR GPRS (1) FOR CONNECTION MODE, also - APN, User, Psw, (rate for CSD)
+CIPDPDP:1		SET WHETHER CHECK STATE OF GPRS NETWORK TIMING
Detect PDP Inerval:10Ms
Detect PDP Timer:3
App Tcpip Mode:0


	Old variant
		DebSendString("\r\nState - APN");
		GsmSendString("AT+CIPCSGP=1,\"internet.lmt.lv\";+CIPCCFG=3,2,256,1;+CIPATS=0;+CIPSCONT\r\n");
	AT+CIPCSGP=1,"internet.lmt.lv";+CIPCCFG=3,2,256,1
	AT+CIPATS=0;+CIPSCONT


New
	GsmUart_SendText("AT+CDNSORIP=1;+CIPSPRT=1;+CIPHEAD=0;+CIPFLP=0;+CIPSRIP=0;+CIPCSGP=1,\"");
	GsmUart_SendText(APN);
	GsmUart_SendText("\";+CIPCCFG=2,1,256,1;+CIPSCONT\r\n");

AT+CDNSORIP=1;+CIPSPRT=1;+CIPHEAD=0;+CIPFLP=0;+CIPSRIP=0;+CIPCSGP=1,"internet.lmt.lv";+CIPCCFG=3,2,256,1;+CIPSCONT
____________________________________________________________________________________
	For Transparent MODE
AT+CIPMODE=1;+CIPCCFG=3,2,256,1;+CIPATS=1;	3 retry, 400mS timeout, 256B packetsize
	else
AT+CIPMODE=0

AT+CREG=1		enable network registration unsolicited result code -> +CREG: <stat>
AT+CREG?		Network registration:
0- not registered, 1 home netw, 2 searching, 3 registration denied, 4 unknown, 5 roaming
____________________________________________________________________________________
DebSendString("\r\nState - DNS");
GsmSendString("AT+CDNSCFG=\"159.148.60.20\",\"195.2.96.2\";+CDNSORIP=1\r\n");
AT+CDNSCFG="159.148.60.20","195.2.96.2";+CDNSORIP=1;+CIPMODE=0

____________________________________________________________________________________
	DebSendString("\r\nState - Connect: ");
	GsmSendString("AT+CIPSTART=\"tcp\",\"peles.linkpc.net\",\"10123\"\r\n");
AT+CIPSTART="tcp","peles.linkpc.net","10123"

	OK
	CONNECT OK
	ato
	CONNECT
	 1:35:10
	 1:45
5659.85179|02416.40145
	GsmSendString("ATO\r\n");
ATO
____________________________________________________________________________________
	DebSendString("\r\nConnection Timeout - Close Connection");
	GsmSendString("AT+CIPCLOSE\r\n");
AT+CIPCLOSE
____________________________________________________________________________________
	DebSendString("\r\nShut Down Connection");
	GsmSendString("AT+CIPSHUT\r\n");
AT+CIPSHUT

____________________________________________________________________________________
//	Set GSM module to factory setings	Gsm_AT()
AT&F;&W

//	GsmState_AT:
AT
//	GsmState_Csq:
AT+CSQ
AT+CREG?		// check network registration (set/clear Flags.home)
//	GsmState_DataSend:
AT+CIPSEND=%u	SendDataLen,	'>' - data



//	GsmState_StartSetup:
AT+CDNSCFG="159.148.60.20","195.2.96.2";+CDNSORIP=1;+CIPMODE=0

//	GsmState_newModule:
ATE0;+CMEE=0;+IFC=2,2;+IPR=38400;&D1;&W
//	GsmState_SmsSetup:
AT+CLIP=1;&W;+COLP=1;&W;+COPS=0;&W;+CMGF=1;&W;+CNMI=2,1,0,0,0;&W;+CSAS=0;&W
//	GsmState_setAPN:
AT+CDNSORIP=1;+CIPSPRT=1;+CIPHEAD=0;+CIPFLP=0;+CIPSRIP=0;+CIPCSGP=1,"internet.lmt.lv";+CIPCCFG=3,2,256,1;+CIPSCONT
//	restart module


AT+CIPSTART="tcp","peles.linkpc.net","10123"


AT+CIPSTATUS
	OK
STATE: TCP CONNECTING

AT+CIPCLOSE
CLOSE OK
STATE: IP CLOSE
CONNECT FAIL

=========================================================================
				A-GPS download
AT+CDNSORIP=1;+CDNSCFG="159.148.60.20","195.2.96.2";+CIPSTART="tcp","agps.u-blox.com","46434"
	OK
	CONNECT
cmd=full;user=k256@inbox.lv;pwd=Cxcvir;latency=0.5;pacc=5000;lat=56.9974;lon=24.2732		(bergi - 24.2733	56.9972)
																								pacc=5000	- depending condition from last motion
	u-blox a-gps server (c) 1997-2008 u-blox AG\n												lat=56.9974	- from memory
	Content-Length: 2600\n																		lon=24.2732	- from memory
	Content-Type: application/ubx\n
	\n
	data.......

	CLOSED
=========================================================================
*/
typedef union
{
	U16 word;
	struct
	{
		U16 ackOk		:1;		// "OK"		received
		U16 ackErr		:1;		// "Error"	received
		U16 ackData		:1;		// data		received
		U16 rec			:1;		// serial data received
		U16 sms			:1;		// SMS mesaage receive
		U16 knownCaller	:1;
		U16 flag_6		:1;
		U16 flag_7		:1;
		U16 flag_8		:1;
		U16 setup		:1;
		U16 transp		:1;		// 1-DATA mode;		0-Command mode (In transparent mode)
		U16 connect		:1;		// Connected to server
		U16 home		:1;		// 1-Home network;	0-roaming or no network +CREG: 1
		U16 csq			:1;		// CSQ signal = 6:31
		U16 active		:1;		// "Call Ready" received
		U16 pwr			:1;		// Module Power ON/off
	};
	struct
	{
		U16	Ack			:5;
		U16 und_7		:3;
		U16 und_8		:2;
		U16	Status		:6;
	};
} GSM_FLAGS;

#define	FLAGS_OK	0b1001
#define	FLAGS_ERROR	0b1010
#define	STATUS_TCP_ACTIVE	0b111111
#define	STATUS_CONNECTED	0b111110
#define	STATUS_HOME			0b111100
#define	STATUS_NETWORK		0b111000
#define	STATUS_ACTIVE		0b110000
#define	STATUS_POWER		0b100000

//#define	Gsm_Interrupt		__attribute__((interrupt, no_auto_psv)) _SI2C1Interrupt
#define	GsmIrq_On()			GSM_IF = 0; GSM_IE = 1
#define	GsmIrq_Off()		GSM_IE = 0
#define	GsmUart_Init		Uart2_Init
#define	GsmUart_SetBaudrate	Uart2_SetBaudrate
#define	GsmByteReceived		Uart2_ByteReceived
#define	GsmStringReceived	Uart2_StringReceived
#define	GsmUart_GetByte		Uart2_GetByte
#define	GsmUart_SendByte	Uart2_SendByte
#define	GsmUart_GetString	Uart2_GetString
#define	GsmUart_SendData	Uart2_SendData
#define	GsmUart_SendText	Uart2_SendText
#define	GsmUart_SendEnter	Uart2_SendEnter
#define	GsmFlags			Flags.word

#define	QUOTE_CLIP_NUMBER	((0 * 2) + 1)				//	+CLIP: "+37129279661",145,"",,"Pele",0
#define	QUOTE_CLIP_CONTACT	((2 * 2) + 1)				//	+CLIP: "+37129279661",145,"",,"Pele",0
#define	QUOTE_SMS_NUMBER	((0 * 2) + 1)				//	+CMT: "+37129279661",145,"Pele","TIME"...
#define	QUOTE_SMS_CONTACT	((1 * 2) + 1)				//	+CMT: "+37129279661",145,"Pele","TIME"...
#define	QUOTE_SMS_TIME		((2 * 2) + 1)				//	+CMT: "+37129279661",145,"Pele","TIME"...
#define	QUOTE_SMS_R_STATUS	((0 * 2) + 1)				//	+CMGR: “REC READ”,“+8613918186089”,"Pele",“02/01/30,20:40:31+00”	<CR><LF> This is a test	<CR><LF> OK
#define	QUOTE_SMS_R_NUMBER	((1 * 2) + 1)				//	+CMGR: “REC READ”,“+8613918186089”,,“02/01/30,20:40:31+00”			<CR><LF> This is a test	<CR><LF> OK
#define	QUOTE_SMS_R_CONTACT	((2 * 2) + 1)				//	+CMGL: 1,”REC READ”,”+8613918186089”,"Pele", “02/01/30,20:40:31+00”		<CR><LF> This is a test	<CR><LF>
#define	QUOTE_SMS_R_TIME	((3 * 2) + 1)				//	+CMGL: 2,”REC UNREAD”,“+8613918186089”,,“02/01/30,20:40:31+00”<CR><LF> This is a test	<CR><LF> OK

inline	void	Gsm_ResponseAction	(void);
inline	void	Gsm_IdleState		(void);
inline	U8		QuotationPhoneNr	(U8 quotationNr);
inline	U8		QuotationText		(U8 quotationNr);
inline	void	GetSmsText			(char* ptr);
inline	void	SmsAnalize			(void);
inline	void	SmsCommand			(char* command, U8 len);
inline	void	SmsFunction_Reset	(char* ptr);
inline	void	SmsFunction_Tcp		(char* ptr);
inline	void	SmsFunction_Test	(char* ptr);
inline	void	SmsFunction_Help	(char* ptr);

//inline	void	GprsPacketReceived	(void);
//inline	void	TcpPacketReceived	(void);

typedef enum
{
	GsmState_Idle = 0,				//   0
	GsmState_AT,					//   1
	GsmState_Csq,					//   2
	GsmState_Creg,					//   3
	GsmState_Ato,					//   4

	GsmState_OKsend,				//   5
	GsmState_DataSend,				//   6
	GsmState_ChipStatus,			//   7
	GsmState_Connect,				//   8
	GsmState_Connected,				//   9
	GsmState_DisConnected,			//  10
	GsmState_SendOk,				//  11

	gsmState_off,					//  12
	GsmState_PowerOn,				//  13
	GsmState_PowerKeyOn,			//  14
	GsmState_PowerKeyOff,			//  15
	GsmState_waitRDY,				//  16
	GsmState_CPIN,					//  17
	GsmState_StartSetup,			//  18
	GsmState_callReady,				//  19
	gsmState_setDNS,				//  20
	GsmState_newModule,				//  21
	GsmState_ModuleSetup,			//  22
	GsmState_SmsSetup,				//  23
	GsmState_setAPN,				//  24

	GsmState_sms_received,			//  25
	GsmState_sms_check,				//  26
	GsmState_sms_read,				//  27

	GsmState_Reset,					//  28
	GsmState_TurnOff,				// 2
	GsmState_Restart,				// 3
	GsmState_Abort,					// 4
	GsmState_Init,					// 5
	GsmState_StartWait,				// 7
	GsmState_ModuleEnabled,			// 8
	GsmState_SimVerify,				// 9
	GsmState_waitCallReady,			// 10
	GsmState_IMEI,					// 11

	GsmState_NewModuleSetup,		// 12
	GsmState_CIPSCONT,				// 13	Save TCPIP Application Context

} GSM_STATE;

typedef struct
{
	char*		text;
	U8			size;
	GSM_STATE	newState;
	U16			newTimeout;
	U16			orFlags;
} GSM_RESPONSE;

const GSM_RESPONSE GsmResponses [] =
{	//Response text			text size					state (0-no switch)			Timeout		flags OR
	{"OK",					(sizeof("OK")				- 1),	0,						0,		0b0000000000000001	},	// bit Ok
	{"ERROR",				(sizeof("ERROR")			- 1),	0,						0,		0b0000000000000010	},	// bit Err
	{"+CSQ: ",				6,									0,						0,		0b0000000000000100	},	// bit data
	{"+CREG:",				(sizeof("+CREG:")			- 1),	GsmState_Creg,			0,		0b0000000000000100	},	// bit data	+CREG: 1,1 /  +CREG: 1
	{"CONNECT OK",			(sizeof("CONNECT OK")		- 1),	GsmState_Connected,		0,		0b0000100000000000	},	// bit connect
	{"CONNECT FAIL",		(sizeof("CONNECT FAIL")		- 1),	GsmState_DisConnected,	0,		0b0000000000000000	},	// bit 
	{"CONNECT",				(sizeof("CONNECT")			- 1),	GsmState_Connected,		0,		0b0000110000000000	},	// bit connect, transp
	{"ALREADY CONNECT",		(sizeof("ALREADY CONNECT")	- 1),	GsmState_Connected,		0,		0b0000110000000000	},	// bit connect, transp
	{"CLOSED",				(sizeof("CLOSED")			- 1),	GsmState_DisConnected,	0,		0b0000000000000100	},	// bit
	{"IP CLOSE",			(sizeof("IP CLOSE")			- 1),	GsmState_DisConnected,	0,		0b0000000000000000	},	// bit 
	{"SEND OK",				(sizeof("SEND OK")			- 1),	GsmState_SendOk,		0,		0b0000000000000000	},	// bit
//	{"TCP ERROR",			(sizeof("TCP ERROR")		- 1),	GsmState_PowerKeyOn,	0,		0b0000000000000000	},	// bit 
	{"RDY",					(sizeof("RDY") - 1),				GsmState_CPIN,			500,	0b1000000000000000	},	// bit Pwr
	{"+CFUN",				(sizeof("+CFUN")			- 1),	GsmState_CPIN,			500,	0b0000000000000100	},	// bit data
	{"+CPIN: READY",		(sizeof("+CPIN: READY")		- 1),	GsmState_StartSetup,	0,		0b0000000000000000	},	// bit 
	{"Call Ready",			(sizeof("Call Ready")		- 1),	0,						0,		0b0100000000000000	},	// bit active
	{"STATE: TCP CONNECTING",(sizeof("STATE: TCP CONNECTING")-1),	0,					9000,	0b0000000000000000	},	// bit active
	{"NORMAL POWER DOWN",	(sizeof("NORMAL POWER DOWN")-1),	GsmState_PowerOn,		0,		0b000000000000000	},	// bit active
	{"+CMTI: ",				(sizeof("+CMTI: ")			-1),	GsmState_sms_received,	0,		0b000000000000000	},
	{"+CMGR: ",				(sizeof("+CMGR:")			-1),	GsmState_sms_read,		0,		0b000000000000000	},
	{"+CMGL: ",				(sizeof("+CMGL:")			-1),	GsmState_sms_read,		0,		0b000000000000000	},
	{"RING",				(sizeof("RING")				-1),	GsmState_Reset,			0,		0b000000000000000	},	// temporary - reset system
//	{"",					0,									0,						0,		0b0000000000000000	},	// bit data
/*
	UNDER-VOLTAGE WARNNING
	{"+CPAS: ",				gsmState_CPAS,			0,		0						},
	{"+CLIP: ",				gsmState_CLIP,			0,		0						},
	{"+CMT: ",				gsmState_SMS,			0,		0						},
	{">",					gsmState_SMS_sendBody,	0,		0						},
	{"+CMGS:",				gsmState_off,			50,		RET_CODE_OK 			},
	{"NO CARRIER",			gsmState_HangUp,		0,		0						},
	{"BUSY",				gsmState_HangUp,		0,		RET_CODE_END			}
*/
};

typedef struct
{
	char*		text;
	U8			size;
	void		(*funct)(char* ptr);
} SMS_COMAND;

const SMS_COMAND SmsCommands [] =
{
	{"Reset",			(sizeof("Reset") - 1),		SmsFunction_Reset	},
	{"Tcp",				(sizeof("Tcp") - 1),		SmsFunction_Tcp		},
//	{"Call",			SmsFunction_Call		},
//	{"Sms",				SmsFunction_Sms			},
//	{"Name",			SmsFunction_Name		},
//	{"Number",			SmsFunction_Number		},
//	{"Interval",		SmsFunction_TimeOut		},
//	{"Layout",			SmsFunction_Layout		},
//	{"Data",			SmsFunction_Data		},
//	{"Service",			SmsFunction_Service		},
//	{"Psw",				SmsFunction_Password	},
//	{"?",				SmsFunction_Info		},
//	{"Dev:",			SmsTargetData_Device	},
	{"TEST_sms",		(sizeof("TEST_sms") - 1),	SmsFunction_Test	},
//	{"Time",			SmsTargetData_Time		},
//	{"P:",				SmsTargetData_Power		},
//	{"CSQ:",			SmsTargetData_Status	},
//	{"Lat",				SmsTargetData_Lat		},
//	{"Lon",				SmsTargetData_Lon		},
	{"?",				(sizeof("?") - 1),			SmsFunction_Help	}
};

/*
AT+CLIP
AT+COLP
AT+COPS
AT+CMGF		SMS Message Format		0:	PDU  mode
									1:	text mode
AT+CNMI		SMS Message indications	2, 0,...	
									2, 2,...	
AT+CSAS		Save SMS Settings



#define _0_APN			"internet.lmt.lv"	// for debug
#define _0_USER			""					// for debug
#define _0_PSW			""					// for debug
#define _0_IP			"78.84.162.46"		// for debug
#define _0_PORT			"10123"				// for debug

#define _1_IP			"159.148.235.103"	// for debug
#define _1_PORT			"22220"				// for debug

#define _2_IP			"89.111.20.206"		// static IP of sns.lv
#define _2_PORT			"9005"				// 2nd port	 of sns.lv


#define _0_IP_PROX		0x67EB949F			// for debug	159.148.235.103 ->hex = 9F 94 EB 67
#define _0_PORT_PROX	22220				// for debug

#define DEFAULT_DNS1	"80.232.230.242"
#define DEFAULT_DNS2	"195.122.12.242"
#define DEFAULT_APN		_0_APN				// for debug
#define DEFAULT_USER	_0_USER				// for debug
#define DEFAULT_PSW		_0_PSW				// for debug
#define DEFAULT_IP		_0_IP				// for debug
#define DEFAULT_PORT	_0_PORT				// for debug
#define DEFAULT_PROX_IP	_0_IP_PROX			// for debug
#define DEFAULT_PROX_P	_0_PORT_PROX		// for debug

//#define GSM_SOCKET_COUNT		5
#define	CSQ_TIMEOUT_SEC			5
#define	MAX_SMS					16
#define	MAX_CMD					16

#define RECONNECT_NUMBER		3
#define RECONNECT_EVER			0xFF
#define TIMEOUT_MS_POWER_UP		300
#define TIMEOUT_MS_RESET		300
#define TIMEOUT_MS_POW_KEY		2000
#define TIMEOUT_MS_SYSSTART		5000
#define TIMEOUT_MS_CSQ			5000
#define TIMEOUT_1_SEC			1000
#define TIMEOUT_2_SEC			2000
#define TIMEOUT_5_SEC			5000
#define TIMEOUT_10_SEC			10000
#define TIMEOUT_MS_HANGUP		3000
#define TIMEOUT_MS_SMS_CMD		10000
#define TIMEOUT_MS_CONNECT		30000
#define TIMEOUT_MS_RECONNECT	20000
#define TIMEOUT_MS_CLOSE		10000
#define TIMEOUT_MS_READ			10000
#define TIMEOUT_MS_WRITE		10000
#define TIMEOUT_MS_AT_CMD		200
#define TIMEOUT_MS_AT_OK		10
#define TIMEOUT_MS_WATCHDOG		60000
#define TIMEOUT_MS_RECEIVING	3000


#define SMS_CMD_START			'#'
#define SMS_CMD_END				'#'
#define PARAMETER_SEPARATOR		';'
#define SMS_WRITE				0xFD
#define SMS_WRITE_INACTIVE		0xFE
#define SMS_WAIT_STORAGE_INDEX	0xFF
*/



/*
typedef enum
{
	gsmState_off = 0,
	gsmState_powerRestart,
	gsmState_powerOn,
	gsmState_powerKeyOn,
	gsmState_powerKeyOff,
	gsmState_waitRDY,
	gsmState_newModule,
	gsmState_ModuleSetup,
//	gsmState_setDNS,
	gsmState_setAPN,
	gsmState_RDY,
	gsmState_CPIN,
	gsmState_waitCallReady,
	gsmState_callReady,
	gsmState_IMEI,
	gsmState_AT,
	gsmState_setDNS,
	gsmState_CSQ,
	gsmState_connectTCPpeles,
	gsmState_waitTcpConnection,
	gsmState_ConnectionFail,
	gsmState_TcpConnected,
	gsmState_timeOut,
	gsmState_Closed,
	gsmState_wait,
	gsmState_switch2command,
	gsmState_CSQconnected,
	gsmState_ATO,
	gsmState_8,
	gsmState_9,
	gsmState_test,
	gsmState_sendCommand,
	gsmState_sendData,
	gsmState_SEND_OK,
	gsmState_switchOff,
	gsmState_switchOff_1,
	gsmState_switchOff_2,
	gsmState_switchOff_3,
	gsmState_PowerDown,
	gsmState_undefined
	
} GSM_STATE;
*/






typedef enum
{
	GsmCmd_none			= 0,
	GsmCmd_On,
	GsmCmd_Off,
	GsmCmd_Connect,
	GsmCmd_DisConnect,
	GsmCmd_Send,

	GsmCmd_undefined

//	GsmCmd_Connect		= GsmCmd_Save			+ GSM_SOCKET_COUNT,	// for all sockets
//	GsmCmd_Close		= GsmCmd_Connect		+ GSM_SOCKET_COUNT,	// for all sockets
//	GsmCmd_DataMustRead	= GsmCmd_Close			+ GSM_SOCKET_COUNT,	// for all sockets
//	GsmCmd_DataMustSend	= GsmCmd_DataMustRead	+ GSM_SOCKET_COUNT,	// for all sockets
//	GsmCmd_none			= GsmCmd_DataMustSend	+ GSM_SOCKET_COUNT,
//	GsmCmd_HangUp,
//	GsmCmd_SmsRead,
//	GsmCmd_SmsDelete,
//	GsmCmd_SendSMS,
//	GsmCmd_Call,
//	GsmCmd_ReNewSmsLimit,
} GSM_CMD;


/*
const char* GsmResponseStrings [] =
{							// Resp_Empty
	"OK", 					// Resp_OK
	"ERROR", 				// Resp_ERROR
	"RDY",	 				// Resp_RDY
	"+CFUN: ",				// Resp_CFUN
	"+STC: ",				// Resp_STC
	"+CPIN: READY",			// Resp_CPIN
	"Call Ready", 			// Resp_Call_Ready
	"+CSQ: ", 				// Resp_CSQ
	"STATE: IP INITIAL",	// Resp_STATE_INI
	"STATE: IP STATUS",		// Resp_STATE_STATUS
	"STATE: IP CLOSE",		// Resp_STATE_CLOSE
	"STATE: TCP CONNECTING",// Resp_STATE_WAIT_C
	"STATE: PDP DEACT",		// Resp_STATE_PDP_D
	"+PDP: DEACT",			// Resp_PDP_DEACT
	"NORMAL POWER DOWN", 	// Resp_NORMAL_POWER_DOWN
	"CONNECT FAIL",			// Resp_CONNECT_FAIL
	"CONNECT OK",		 	// Resp_CONNECT
	">",				 	// Resp_SEND_Prefix
	"SEND OK",			 	// Resp_SEND_OK
	"CLOSED",			 	// Resp_CLOSED
	"CLOSE OK",			 	// Resp_CLOSE_OK
	"SHUT OK",			 	// Resp_SHUT_OK
	"AT",				 	// Resp_ECHO
	"$reset",			 	// Resp_RESET

	"RING",			 		// Resp_RING
	"+CLIP: ",			 	// Resp_CLIP		"+37129279661",145,"",,"",0
	"NO CARRIER",			// Resp_NO_CARRIER

	// 
							// 
	"" 						// Resp_Unknown
};
*/
typedef enum
{
	Resp_Empty = 0,

	Resp_OK,
	Resp_ERROR,
	Resp_RDY,
	Resp_CFUN,
	Resp_STC,
	Resp_CPIN_Ready,
	Resp_Call_Ready,
	Resp_CSQ,
//	Resp_IP,
	Resp_STATE_INI,
	Resp_STATE_STATUS,
	Resp_STATE_CLOSE,
	Resp_STATE_WAIT_C,
	Resp_STATE_PDP_D,
//	Resp_STATE,
	Resp_PDP_DEACT,
//	Resp_PDP,
	Resp_NORMAL_POWER_DOWN,
	Resp_CONNECT_FAIL,
	Resp_CONNECT,
	Resp_SEND_Prefix,
	Resp_SEND_OK,
	Resp_CLOSED,			// Closed by Server
	Resp_CLOSE_OK,			// Closed by Device
	Resp_SHUT_OK,
	Resp_ECHO,
	Resp_RESET,

	Resp_RING,
	Resp_CLIP,
	Resp_NO_CARRIER,
	
	Resp_Unknown
} RESPONSE;






/*
typedef enum
{
	SmsCmd_empty = 0,

	SmsCmd_Connect,
	SmsCmd_ReConnect,
	SmsCmd_Close,
	SmsCmd_Save,
	SmsCmd_Ip,
	SmsCmd_Port,
	SmsCmd_Apn,
	SmsCmd_User,
	SmsCmd_Psw,
	SmsCmd_Restart,
	SmsCmd_Abort,
	SmsCmd_Test,

	SmsCmd_Unknown
} SMS_CMD;

typedef struct
{
	SMS_CMD		code;
	const char* smsCmdStr;
} SmsCmd;

const SmsCmd SmsCmdStrings [] =
{
	{SmsCmd_Connect,				"Connect"},
	{SmsCmd_Connect,				"connect"},
	{SmsCmd_Connect,				"CONNECT"},
	{SmsCmd_ReConnect,				"Reconnect"},
	{SmsCmd_ReConnect,				"reconnect"},
	{SmsCmd_ReConnect,				"RECONNECT"},
	{SmsCmd_Close,					"Close"},
	{SmsCmd_Close,					"close"},
	{SmsCmd_Close,					"CLOSE"},
	{SmsCmd_Save,					"Save"},
	{SmsCmd_Save,					"save"},
	{SmsCmd_Save,					"SAVE"},
	{SmsCmd_Ip,						"Ip"},
	{SmsCmd_Ip,						"ip"},
	{SmsCmd_Ip,						"IP"},
	{SmsCmd_Port,					"Port"},
	{SmsCmd_Port,					"port"},
	{SmsCmd_Port,					"PORT"},
	{SmsCmd_Apn,					"Apn"},
	{SmsCmd_Apn,					"apn"},
	{SmsCmd_Apn,					"APN"},
	{SmsCmd_User,					"User"},
	{SmsCmd_User,					"user"},
	{SmsCmd_User,					"USER"},
	{SmsCmd_Psw,					"Password"},
	{SmsCmd_Psw,					"password"},
	{SmsCmd_Psw,					"PASSWORD"},
	{SmsCmd_Restart,				"Restart"},
	{SmsCmd_Restart,				"RESTART"},
	{SmsCmd_Restart,				"restart"},
	{SmsCmd_Abort,					"Abort"},
	{SmsCmd_Abort,					"ABORT"},
	{SmsCmd_Abort,					"abort"},
	{SmsCmd_Test,					"Test"},

	{SmsCmd_Unknown,				""}
};
*/
/*
typedef union
{
	U16	all;
	struct
	{
		U16	moduleConnnected	: 1;
		U16	simCardReady		: 1;
		U16	networkAvailable	: 1;
		U16	GprsProfileReady	: 1;
		U16	disableModule		: 1;
		U16	smsText				: 1;
		U16	gprsRead			: 1;
		U16	gprsWrite			: 1;
	};
} GSM_STATUS;

typedef union
{
	U8	all;
	struct
	{
		U8	allocated	: 1;
		U8	configured	: 1;
		U8	connected	: 1;
		U8	unused		: 5;
	};
} SOCKET_STATUS;

typedef struct
{
	SOCKET_STATUS	status;
	uint8	connectionProfile;
	uint8	reconnect;
	uint32	ip;
	uint16	port;
	Queue*	m_recQueue;
	Queue*	m_sendQueue;

//	uint16 dataInReceiveBuffer;
//	uint16 retryCount;
//	uint32 delayStartTicks;
	uint32 activityTimeoutMs;
	bool autoReconnect;
} MG232_CONTROL;
*/
#endif	// _gsm_defines_h_

/*
PowerKey On
	^SYSSTART
PowerKey Off
ATE0
	ATE0
	OK
AT+CLIP=1;+CMGF=1;+CNMI=2,1,0,0,0;+CPIN?
	+CPIN: READY
	OK
AT+CHUP
	OK
------------------------------------------------------------------------
								IDLE
------------------------------------------------------------------------
AT+CSQ
	+CSQ: 28,99
	OK
------------------------------------------------------------------------
								SAVE
------------------------------------------------------------------------
		0 Con profile clear
AT^SICS=0,conType,none
	OK

		0 Con profile conType
AT^SICS=0,conType,GPRS0
	OK

		0 Con profile apn
AT^SICS=0,apn,"internet.lmt.lv"
	OK

		0 Con profile user
AT^SICS=0,user,"USER"
	OK

		0 Con profile password
AT^SICS=0,passwd,"PASSWORD"
	OK

		0 Service profile clear
AT^SISS=0,srvType,none
	OK

		0 Service profile srvType
AT^SISS=0,srvType,Socket
	OK

		0 Service profile conId
AT^SISS=0,conId,0
	OK

		0 Service profile address
AT^SISS=0,address,"socktcp://159.148.235.103:22220"
	OK
------------------------------------------------------------------------
								CMD
------------------------------------------------------------------------










------------------------------------------------------------------------
AT+CGDCONT=1,“IP”,“INTERNET.LMT.LV”
	OK
at+cgact=1,1;+cgact?
	+CGACT: 1,1
	OK
(at+cgatt?)
*/

