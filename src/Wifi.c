#include	"WifiDefines.h"
#include	"Timers.h"
#include	"Tcp.h"

#ifdef UART4_WIFI

volatile	U16				TimeOut_10mS	= 0, WifiSec = 0, PackRecTimeout = 0;
static		BAUDRATE		WifiBaudrate	= BaudRate_115200;
static		WIFI_STATE		WifiState		= WifiState_Idle;
static		char			WifiBuf[512];
static		char*			RecWifiData 	= NULL;
static		TCP_PACKET		PacketIn[2];
//static		TCP_PACKET		PacketOut;
static		FLAGS			Flags;
static	WIFI_ACCESS_POINT	VisibleAPs[MAX_VISIBLE_AP];
static		U16				VisibleApCount;
static		TCP_PACKET*		PacketRec		= NULL;
static		TCP_PACKET*		OutPacket		= NULL;
static		UNI16			OutPacketLen;
static		U8				OkPacket[8];
static		UNI16			Rssi;

#if	(TEST == TEST_WIFI)
  #include		"Debug.h"
  static char	tmp[80];
#else
  extern void	Usb_SendText (char* str);
  #define		DebugPrint(X)
  #define		DebugSprintf(stream,fmt,a...)
#endif

inline	void Wifi_ResponseAction	(void);
inline	void Wifi_SaveWisibleSsid	(void);
inline	void Wifi_CheckWisibleSsid	(void);

void Wifi_On		(void)
{
	#if	(TEST == TEST_WIFI)
		if (WIFI_EN)
		{
			Wifi_Off();
			DebugPrint("Turn Wifi module Off");
			LED_WIFI = LED_STATUS_1;
			return;
		}
	#endif

	DebugPrint("Turn Wifi module On");
	Tcp_PreparePacket((TCP_PACKET*)OkPacket, TYPE_OK_WIFI);	// Prepare OK packet for use in interrupt
	WifiState = WifiState_PowerOn;
	WIFI_IF = 1;
	WIFI_IE = 1;
}

void Wifi_Off		(void)
{
	WIFI_EN		= 0;				// Wifi Power	Off
	WIFI_IE		= 0;				// Interrupt	Off
	WifiState	= WifiState_PowerOn;
	LED_WIFI	= LED_STATUS_1;
}

U8	 Wifi_Connected	(void)
{
	return (Flags.Status == WIFI_STATUS_CONNECTED);
}

void Wifi_Reset		(void)
{
	WIFI_RST = !WIFI_RST;
	#if	(TEST == TEST_WIFI)
		if (WIFI_RST)		DebugPrint("RUN Wifi module");
		else				DebugPrint("RESET Wifi module");
	#endif
}

U16 Wifi_AUTO		(void)
{
	FL_TEST_MANUAL = !FL_TEST_MANUAL;
	#if	(TEST == TEST_WIFI)
		if (FL_TEST_MANUAL)	DebugPrint("Wifi Manual Mode");
		else				DebugPrint("Wifi Automatic Mode");
	#endif
	return	Flags.word;
}

#if	(TEST == TEST_WIFI)
	void Wifi_Test		(void)
	{
		if (Uart4_StringReceived())
		{
			Uart4_GetString((P_U8)tmp);
			DebugPrint(tmp);
		}
	}

	void Wifi_SendCommand		(char* str)
	{
		Uart4_SendText(str);
		Uart4_SendEnter();
	}

	void Wifi_BaudrateChange	(void)
	{
		if (++WifiBaudrate >= BaudRate_none)
			WifiBaudrate = BaudRate_9600;
		switch (WifiBaudrate)
		{
			case BaudRate_9600:		DebugPrint("Wifi module Baud Rate changed to 9600");	break;
			case BaudRate_19200:	DebugPrint("Wifi module Baud Rate changed to 19200");	break;
			case BaudRate_38400:	DebugPrint("Wifi module Baud Rate changed to 38400");	break;
			case BaudRate_57600:	DebugPrint("Wifi module Baud Rate changed to 57600");	break;
			case BaudRate_115200:	DebugPrint("Wifi module Baud Rate changed to 115200");	break;
			case BaudRate_none:		DebugPrint("Wifi module Baud Rate Error");				break;
		}
		Uart4_SetBaudrate(WifiBaudrate);
	}
#endif

// **********************************************************************
//	WIFI Interrupt (I2C3 Slave Event used for 16bit proc) 100Hz if enabled
// **********************************************************************
void WIFI_INTERRUPT(void)
{
	static	WIFI_STATE	lastState = WifiState_Off;
	static	U8			stateTimeout = 0, lastStatus;
	WIFI_IF = 0;

	if (lastStatus != Flags.Status)
	{
		lastStatus = Flags.Status;													// pow, com, ap, ip, conected
		if		(!Flags.pow)							LED_WIFI = LED_STATUS_1;	//  0    0    0    0    0
		else if (Flags.Status == WIFI_STATUS_POWER)		LED_WIFI = LED_STATUS_2;	//  1    0    0    0    0
		else if (Flags.Status == WIFI_STATUS_COM)		LED_WIFI = LED_STATUS_2;	//  1    1    0    0    0
		else if (Flags.Status == WIFI_STATUS_AP)		LED_WIFI = LED_STATUS_3;	//  1    1    1    0    0
		else if (Flags.Status == WIFI_STATUS_IP_OK)		LED_WIFI = LED_STATUS_3;	//  1    1    1    1    0
		else if (Flags.Status == WIFI_STATUS_CONNECTED)	LED_WIFI = LED_STATUS_SLOW;	//  1    1    1    1    1
	}

	if (PacketRec)
	{
//		DebugSprintf(tmp,"Tcp Packet - Type: %02X, ID: %04X, Len: %d", PacketRec->type, PacketRec->id, PacketRec->len);
//		DebugPrint(tmp);
		Tcp_PacketReceived(PacketRec);
		PacketRec = NULL;
	}
	if (PackRecTimeout)
	{
		if (!--PackRecTimeout)
		{
			FL_WIFI_REC	= 0;
			DebugPrint("Error: Tcp Packet Receive Timeout !!!");
		}
		return;
	}

	Wifi_ResponseAction();

	if (TimeOut_10mS)
	{
		TimeOut_10mS--;
		return;
	}

	if (lastState != WifiState)			// Wifi state changed
	{
		lastState = WifiState;
		stateTimeout = 0;
		DebugSprintf(tmp, "State nr: %d, Flags: 0x%04X", lastState, Flags.word);
		DebugPrint(tmp);
	}
	else if (!++stateTimeout)
	{
		#if	(TEST == TEST_WIFI)
			if (!FL_TEST_MANUAL)
		#endif
		{
			DebugSprintf(tmp, "State nr: %d, Flags: 0x%04X - TimeOut Error !!!", lastState, Flags.word);
			DebugPrint(tmp);
			WIFI_RST		= 0;
			FL_WIFI_OFFLINE	= 1;
			WifiState		= WifiState_PowerOn;// Restart Wifi module
			TimeOut_10mS	= 200;				// Timeout 2.0 sec
			return;
		}
		// TODO: Change Baudrates
	}
	switch (WifiState)
	{
	case WifiState_Idle:		{
		#if	(TEST == TEST_WIFI)
			if (FL_TEST_MANUAL)
			{
				stateTimeout = 0;				// debug mode
				return;
			}
		#endif
		if ((Flags.Status == WIFI_STATUS_IP_OK) && (Flags.ackOk))
			WifiState = WifiState_Connect;
		else if (!(WifiSec & AP_CHECK_SEC))		// every 31 sec
		{
//			FL_WIFI_OFFLINE = 0;
			Uart4_SendText("AT+CWJAP?\r\n");	//	Query Access Point Connection
			Flags.Ack = 0;
			WifiSec++;
		}
		else if (Flags.Status == WIFI_STATUS_CONNECTED)
		{
			FL_WIFI_OFFLINE = 0;
			if (WifiSec >= WIFI_OK_TIMEOUT_S)
			{
				OkPacket[6] = Rssi.u8l;
				OkPacket[7] = Rssi.u8h;
				Tcp_PreparePacket((TCP_PACKET*)OkPacket, TYPE_OK_WIFI);
				OutPacket = (TCP_PACKET*)OkPacket;
			}
			else if (FL_TCP_READY)
				OutPacket = Tcp_GetUnsent();
			
			if (!OutPacket)
				FL_TCP_READY = 0;
			else
			{
//				DebugSprintf(tmp, "Available buffers: %u", Tcp_AvailableOutBuff());
//				DebugPrint(tmp);
				OutPacketLen.u16	= 0;
				OutPacketLen.u8l	= OutPacket->len;
				OutPacketLen.u16  += TCP_PACKET_HEDER;			//		LEN_CHANGES
				WifiState = WifiState_SendPacket;
				WifiSec = 0;
				#if	(TEST == TEST_WIFI)
					switch (OutPacket->type)
					{
					case TYPE_OK_WIFI:	DebugPrint("Send OK_Wifi Packet");	break;
//					default:			DebugSprintf(tmp, "Send Wifi Packet: Type_%02X, Len_%03u, Nr_%04X",
//															OutPacket->type, OutPacket->len, OutPacket->nr);
//										DebugPrint(tmp);					break;
					}
				#endif
			}
		}
		stateTimeout = 0;						// temporary
		break;					}
	case WifiState_PowerOn:		{
		Flags.word	= 0;
		WIFI_RST	= 1;
		WIFI_EN		= !WIFI_EN;
		if (WIFI_EN)							// if not restart
		{
			Flags.pow	= 1;					// pow Flag on
			Uart4_Init(WifiBaudrate);
		}
		TimeOut_10mS = 150;						// Timeout 1.5 sec
		break;					}
	case WifiState_ComOn:		{
		if (!stateTimeout)
		{
			Uart4_SendText("ATE0\r\n");
			Flags.Ack = 0;
		}
		else if (Flags.ackOk)
			WifiState = WifiState_CwMode;
		break;				}
	case WifiState_CwMode:		{				// TODO: Restart module after mode change
		if (!stateTimeout)
		{
			Uart4_SendText("AT+CWMODE?\r\n");
			Flags.Ack = 0;
		}
		else if (Flags.ackOk)
		{
			if (Flags.setup)
				WifiState = WifiState_Setup;
			else
			{
				WifiSec = (AP_CHECK_SEC - 3);
				WifiState = WifiState_Idle;
			}
		}
		else if (Flags.ackData)
		{
			register S16 mode = atoi(RecWifiData);
			DebugSprintf(tmp, "Wifi MODE: %u, must be %u", mode, WIFI_MODE);
			DebugPrint(tmp);
			if (mode != WIFI_MODE)
				Flags.setup = 1;
			Flags.Ack = 0;
		}
		break;					}
	case WifiState_NoAp:		{
		FL_WIFI_OFFLINE = 1;
		Flags.Status = WIFI_STATUS_COM;			// pow=1, com=1, ap=0, ip=0, conected=0
		if (!Flags.ackOk)
			break;				}
	case WifiState_ListAp:		{
		Uart4_SendText("AT+CWLAP\r\n");
		TimeOut_10mS = 1000;					// Timeout 10 sec;
		Flags.Ack = 0;
		VisibleApCount = 0;
		WifiState = WifiState_ListApRec;
		break;					}
	case WifiState_ListApRec:	{
		if (Flags.ackOk)
		{
			Wifi_CheckWisibleSsid();
			WifiState = WifiState_Idle;
		}
		else if (Flags.ackErr)
		{
			DebugPrint("Wifi Access Point List Error");
			WifiState = WifiState_Idle;
		}
		else if (Flags.ackData)
		{
			Wifi_SaveWisibleSsid();
			Flags.Ack = 0;
		}
		break;					}
	case WifiState_CWJAP:		{
		register U8 i = 0, j = 0;
		FL_WIFI_OFFLINE = 0;
		while (RecWifiData[i])
		{
			if (RecWifiData[i++] != ',')	// search last ',' in string
				continue;
			j = i;							// point j to next char
		}
		if (RecWifiData[j])
			Rssi.s16 = atoi(&RecWifiData[j]);	// +CWJAP:"Mazais","a0:f3:c1:b5:af:00",3,-55
		
		WifiState = WifiState_Idle;
		break;					}
	case WifiState_SendPacket:	{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			sprintf(WifiBuf,"AT+CIPSEND=%d\r\n",OutPacketLen.u16);
			Uart4_SendText(WifiBuf);
		}
		else if (Flags.ackOk)
		{
			if ((Uart4_ByteReceived()) && (Uart4_GetByte() == '>'))
			{
//				while (Uart4_ByteReceived())
//					Uart4_GetByte();
				Uart4_SendData((char*)OutPacket->bytes, OutPacketLen.u16);
				TimeOut_10mS = 3000;			// Timeout 30 sec
//				WifiState = WifiState_Idle;
			}
		}
		break;					}
	case WifiState_SendOk:		{
		if (OutPacket != (TCP_PACKET*)OkPacket)
			OutPacket->start = 0;
		OutPacket = NULL;
		WifiState = WifiState_Idle;
		break;					}
	case WifiState_Setup:		{
		if (!stateTimeout)
		{
			sprintf(WifiBuf,"AT+CWMODE=%d\r\n", WIFI_MODE);
			Uart4_SendText(WifiBuf);
			Flags.Ack = 0;
			DebugSprintf(tmp, "Set CwMode to %u", WIFI_MODE);
			DebugPrint(tmp);
		}
		else if (Flags.ackOk)
		{
			DebugPrint("Ok - Reset module");
			WIFI_RST = 0;
			WifiState = WifiState_PowerOn;		// Restart Wifi module
			TimeOut_10mS = 20;					// Timeout 0.2 sec
		}
		break;					}
	case WifiState_Disconnect:	{
		Flags.Status &= WIFI_STATUS_COM;
		WifiState = WifiState_Idle;
		break;					}
	case WifiState_Connect:		{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			Uart4_SendText("AT+CIPSTART=\"TCP\",\"");
			Uart4_SendText(SERVER);
			Uart4_SendText("\",");
			Uart4_SendText(PORT);
			Uart4_SendEnter();
			DebugPrint("Wifi Connect Command");
		}
		else if (Flags.ackOk)
		{
			WifiState = WifiState_Idle;
			WifiSec	= WIFI_OK_TIMEOUT_S;
			DebugPrint("Wifi Connect Ok -> wait connection 90sec");
			TimeOut_10mS = 9000;			// Timeout 90 sec
		}
		else if (stateTimeout > 200)
		{
			DebugPrint("Wifi Connect Command Error -> tray after 10 sec");
			WifiState = WifiState_Idle;
			TimeOut_10mS = 1000;
		}
		break;					}
	case WifiState_Closed:		{
		Flags.conected	= 0;
		WifiSec = AP_CHECK_SEC - 1;
		WifiState = WifiState_Idle;
		break;					}
	default:					{
		DebugSprintf(tmp, "State nr: %d, Flags: 0x%04X - State Error !!!", WifiState, Flags.word);
		DebugPrint(tmp);
		WifiState = WifiState_Idle;
		break;					}
	}
}

inline	void	WifiPacketReceive	(U8 data)
{
	static TCP_PACKET*	inPacket;
	static U8			currentPacket = 0, crc;
	static U16			idx = 0;
	static UNI16		recLen;

	if (!FL_WIFI_REC)
	{
		if (!Flags.conected)
			return;
		FL_WIFI_REC	= 1;				// Set flag to receive WIFI packet
		PackRecTimeout = TCP_PACKET_TIMEOUT;
		currentPacket ^= 1;				// change to ather packet buffer
		inPacket	= &PacketIn[currentPacket];	// Recive pointer to free buffer
		idx			= 0;				// buffer index = 0
		crc			= 0;				// clear crc
		recLen.u16  = 0;
	}
	inPacket->bytes[idx++] = data;		// save current byte
	if (idx > 3)						// if Start, Lenght, CRC bytes already received
	{
		crc ^= data;					// calculate crc
		if (idx >= recLen.u16)		// if packet len bytes received		LEN_CHANGES
		{
			FL_WIFI_REC		= 0;		// turn off tcp packet receive
			PackRecTimeout	= 0;
			if (crc == inPacket->crc)	// if crc ok
			{
				PacketRec	= inPacket;
				WIFI_IF		= 1;		// enable Wifi IRQ
			}
//			U8* ptr = inPacket->bytes;
//			DebugSprintf(gsmBuff,"%02X %02X %02X(%02X) %02X %02X %02X %02X %02X",
//						ptr[0], ptr[1], ptr[2], crc, ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
//			DebugPrint(gsmBuff);
		}
	}
	else if (idx == 2)											//		LEN_CHANGES
	{
		recLen.u8l  = inPacket->len;
		recLen.u16 += TCP_PACKET_HEDER;
	}
}

inline	void Wifi_ResponseAction	(void)
{
	while (Uart4_StringReceived())
	{
		register char d = WifiBuf[0];
		Uart4_GetString((P_U8)WifiBuf);
		if (!WifiBuf[0])						// if received string is empty
		{	WifiBuf[0] = d;	continue;	}		// restore previous value of buffer ("" = '\0')

		#if	(TEST == TEST_WIFI)
			if (FL_TEST_MANUAL)
			{
				DebugPrint(WifiBuf);
				return;
			}
		#endif
		
		Flags.Ack = 0b1000;
		register U16 i;
		for(i = 0; i < (sizeof(WifiResponses) / sizeof(WIFI_RESPONSE)); i++)
		{
			if(!(memcmp(WifiBuf, WifiResponses[i].text, WifiResponses[i].size)))
			{
				if (WifiResponses[i].newState)
					WifiState = WifiResponses[i].newState;
				TimeOut_10mS = WifiResponses[i].newTimeout;
				Flags.word |= WifiResponses[i].orFlags;
				RecWifiData = WifiBuf + WifiResponses[i].size;
				#ifdef	SHOW_AT_RESPONSE
					DebugPrint(WifiBuf);
//					DebugPrint(RecWifiData);
				#endif
				return;
			}
		}
		DebugPrint(WifiBuf);
		RecWifiData = WifiBuf;
		return;
	}
}

// +CWLAP:(3,"Mazais",-52,"a0:f3:c1:b5:af:00",3,-21)
inline void Wifi_SaveWisibleSsid	(void)
{
	if (VisibleApCount >= MAX_VISIBLE_AP)		return;
	while ((*RecWifiData < '0') || (*RecWifiData > '9'))
	{
		if(!*RecWifiData++)						return;
	}
	VisibleAPs[VisibleApCount].ecn = atoi(RecWifiData);		// 3
	while (*RecWifiData != '"')								// "
	{
		if(!*RecWifiData++)						return;
	}
	RecWifiData++;											// M
	char* ptr = VisibleAPs[VisibleApCount].ssid;
	while (*RecWifiData != '"')
	{
		if(!*RecWifiData)						return;
		*ptr++ = *RecWifiData++;							// Mazais
	}
	*ptr = '\0';
	RecWifiData++;
	if (*RecWifiData++ != ',')					return;
	if(!*RecWifiData)							return;
	VisibleAPs[VisibleApCount].rssi = atoi(RecWifiData);	// -52

	VisibleApCount++;
}

inline void Wifi_CheckWisibleSsid	(void)
{
	if (!VisibleApCount)
		return;
	U16 i, j;
	for (i = 0; i < VisibleApCount; i++)
	{
//		DebugSprintf(tmp,"SSID: '%s', ecn: %d, rssi: %d", VisibleAPs[i].ssid, VisibleAPs[i].ecn, VisibleAPs[i].rssi);
//		DebugPrint(tmp);
//		DebugSprintf(tmp, "Visible AP: %d, Flags: 0x%04X", i, Flags.word);
//		DebugPrint(tmp);
		for (j = 0; j < (sizeof(WifiAccessPoints) / sizeof(WIFI_ACCESS_POINT)); j++)
		{
			if (!strcmp(VisibleAPs[i].ssid, WifiAccessPoints[j].ssid))
			{
				Uart4_SendText("AT+CWJAP=\"");
				Uart4_SendText(WifiAccessPoints[j].ssid);
				Uart4_SendText("\",\"");
				Uart4_SendText(WifiAccessPoints[j].psw);
				Uart4_SendText("\"\r\n");
				TimeOut_10mS = 1000;					// Timeout 10 sec;
				Rssi.s16 = VisibleAPs[VisibleApCount].rssi;
//				DebugSprintf(tmp, "Found AP: %d, Flags: 0x%04X", j, Flags.word);
//				DebugPrint(tmp);
				return;
			}
		}
	}
	// TODO Check free APs
//	VisibleApCount = 0;
	FL_WIFI_OFFLINE = 1;
}
#else
U8	 Wifi_Connected	(void)
{
    return 0;
}
#endif  //  UART4_WIFI

