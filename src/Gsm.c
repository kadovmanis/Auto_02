#include		"Gsm.h"
#include		"GsmDefines.h"
#include		"Ports.h"
#include		"Tcp.h"
#include		<string.h>
#include		<stdio.h>
#include		<stdlib.h>

#if	(TEST == TEST_GSM)
  #include		"Debug.h"
  static char	tmp[80];
#else
  extern void	Usb_SendText (char* str);
  #define		DebugPrint(X)
  #define		DebugSprintf(stream,fmt,a...)
#endif

static	char		gsmBuff[256];
static	char		irqBuf[256];
static	char		callTxt[20], callNr[20];
static	char		OkPacket[16];
static	char*		recGsmData = NULL;
static	GSM_STATE	GsmState;
static	U16			TimeOut;			// 0.01 sec
static	U16			PackRecTimeout;	
static	GSM_FLAGS	Flags;
static	U8			Csq	= 0, LastCsq = 0;
//static	U8			IdleSeconds	= 0;
static	TCP_PACKET	PacketIn[2];
static	TCP_PACKET	PacketOut;
static	TCP_PACKET*	PacketRec	= 0;
static	char*		DataToSend;
static	U16			SendDataLen = 0, OkSeconds = 0, IdleSeconds = 0, CsqTime;

#ifndef GSM_POWER
	U8 GSM_POWER = 1;
#endif

// Public Functions ---------------------------------------------------------------------------------
void Gsm_On	(void)
{
	#if	(TEST == TEST_GSM)
		if (GSM_IE)
		{
			GSM_IE	= 0;	DebugPrint("Turn GSM Interrupt Off");
			LED_GSM	= LED_STATUS_OFF;
			return;
		}
	#endif
	GsmIrq_Off();
	TimeOut		= 1;
	GsmFlags	= 0;
	GsmState	= GsmState_PowerOn;
	Tcp_PreparePacket((TCP_PACKET*)OkPacket, TYPE_OK_GPRS);	// Prepare OK packet for use in interrupt
	GsmIrq_On();		DebugPrint("Turn GSM Interrupt On");
}

void Gsm_Off	(void)
{
	GSM_IE	= 0;
	GSM_RTS_OFF();
	GsmFlags	= 0;
	GsmState	= GsmState_Idle;
	LED_GSM		= LED_STATUS_ERROR;
	while (GSM_STATUS)
	{
		GSM_PK_ON();
	}
	GSM_DTR_OFF();
	LED_GSM		= LED_STATUS_OFF;
}

void Gsm_PwrOn	(void)
{
	GSM_POWER = !GSM_POWER;
	#if	(TEST == TEST_GSM)
		if (GSM_POWER)		DebugPrint("GSM Power Off");
		else				DebugPrint("GSM Power On");
	#endif
}

void Gsm_PwrKey	(void)
{
	GSM_P_KEY = !GSM_P_KEY;
	#if	(TEST == TEST_GSM)
		if (GSM_P_KEY)
		{
			GSM_RTS_ON();	DebugPrint("GSM Power key Off");
//			GSM_DTR_ON();
		}
		else				DebugPrint("GSM Power key On");
	#endif
}

void Gsm_AT		(void)
{
	DebugPrint("Set GSM module to factory setings (AT&F)");
	GsmUart_SendText("AT&F;&W\r\n");
//	GsmUart_SendText("ATE0;&W;+CMEE=0;&W;+IFC=2,2;&W;+IPR=38400;&W;&D0;&W\r\n");
//	GsmUart_SendText("AT\r\n");
}

void Gsm_AtCommand	(char* str)
{
	DebugPrint("Send to GSM:");
	DebugPrint(str);
	GsmUart_SendText(str);
	if (memcmp(str, "+++", 3) != 0)
		GsmUart_SendEnter();
}

void Gsm_DTR		(void)
{
	GSM_DTR = !GSM_DTR;
	#if	(TEST == TEST_GSM)
		if (GSM_DTR)		DebugPrint("GSM GSM_DTR signal High");
		else				DebugPrint("GSM GSM_DTR signal Low");
	#endif
}

U16 Gsm_AUTO		(void)
{
	FL_TEST_MANUAL = !FL_TEST_MANUAL;
	#if	(TEST == TEST_GSM)
		if (FL_TEST_MANUAL)	DebugPrint("GSM Manual Mode");
		else				DebugPrint("GSM Automatic Mode");
	#endif
	return	Flags.word;
}

void Gsm_OnLine		(void)
{
	FL_GPRS_OFFLINE = !FL_GPRS_OFFLINE;
	#if	(TEST == TEST_GSM)
		if (FL_GPRS_OFFLINE)	Usb_SendText("GSM Offline Mode");
		else					Usb_SendText("GSM Online  Mode");
	#endif
}

// Internal Functions -------------------------------------------------------------------------------
inline	void Gsm_ResponseAction	(void)
{
	while (GsmStringReceived())
	{
		if (Flags.sms)
			return GetSmsText(irqBuf);

		register char d = irqBuf[0];
		GsmUart_GetString((P_U8)irqBuf);
		if (!irqBuf[0])							// if received string is empty
		{	irqBuf[0] = d;	continue;	}		// restore previous value of buffer ("" = '\0')
		
		#if	(TEST == TEST_GSM)
			if (FL_TEST_MANUAL)
			{
				DebugPrint(irqBuf);
				return;
			}
		#endif

		Flags.Ack = 0b01000;					// sms=0, rec=1, ackData=0, ackErr=0, ackOk=0
		register U16 i;
		for(i = 0; i < (sizeof(GsmResponses) / sizeof(GSM_RESPONSE)); i++)
		{
			if(!(memcmp(irqBuf, GsmResponses[i].text, GsmResponses[i].size)))
			{
				if (GsmResponses[i].newState)
					GsmState = GsmResponses[i].newState;
				TimeOut = GsmResponses[i].newTimeout;
				GsmFlags |= GsmResponses[i].orFlags;
				recGsmData = irqBuf + GsmResponses[i].size;
				#ifdef	SHOW_AT_RESPONSE
					DebugPrint(irqBuf);
//					DebugPrint(recGsmData);
				#endif
				return;
			}
		}
		DebugPrint("Uncnown response:");
		DebugPrint(irqBuf);
		recGsmData = irqBuf;
		return;
	}
}

// **********************************************************************
//	GSM Interrupt (I2C1 Slave Event used for 16bit proc) 100Hz if enabled
// **********************************************************************
//void Gsm_Interrupt(void)
void GSM_INTERRUPT(void)
{
	static	GSM_STATE	lastGsmState = GsmState_Idle;
	static	U8			stateTimeout = 0, lastStatus;	//, retry = 0, cpas;
	static	U16			ledStatus;
	static	char		recSmsNr[4] = "";
	GSM_IF = 0;

/*
	static U16			tcpTest = 0, tcpCnt = 0;
	if (FL_TCP_READY)
	{
		static U16 flags = 0;
		U16 print = 0;

		if (!tcpTest)
		{
			tcpTest	= 1;
			tcpCnt	= 0;
			print++;
		}
		if (flags != Flags.word)
		{
			flags = Flags.word;
			print++;
		}
		if (print)
		{
			char t[32];
			sprintf(t, "packet to send, flags:%04X, cnt: %u", flags, tcpCnt);
			Usb_SendText(t);
		}
		tcpCnt++;
	}
	else if (tcpTest)
	{
		tcpTest = 0;
		char t[32];
		sprintf(t, "packet    SENT, flags:%04X, cnt: %u", Flags.word, tcpCnt);
		Usb_SendText(t);
	}
*/
	if (lastStatus != Flags.Status)
	{
		lastStatus = Flags.Status;
		if		(lastStatus >= STATUS_CONNECTED)	ledStatus	= LED_STATUS_ON;
		else if	(lastStatus >= STATUS_NETWORK)		ledStatus	= LED_STATUS_3;
		else if	(lastStatus >= STATUS_ACTIVE)		ledStatus	= LED_STATUS_2;
		else if	(lastStatus >= STATUS_POWER)		ledStatus	= LED_STATUS_1;
		else										ledStatus	= LED_STATUS_ERROR;
		CsqTime = (Flags.home && Flags.csq)?	30 : 10;
	}

	if (FL_GSM_SEC)
	{
		FL_GSM_SEC = 0;
		IdleSeconds++;
		OkSeconds++;
	}
	LED_GSM		= ledStatus;

	if (PacketRec)
	{
		DebugSprintf(tmp,"Tcp Packet - Type: %02X, ID: %04X, Len: %d", PacketRec->type, PacketRec->id, PacketRec->len);
		DebugPrint(tmp);
		Tcp_PacketReceived(PacketRec);
		PacketRec = NULL;
	}

	if (PackRecTimeout)
	{
		if (!--PackRecTimeout)
		{
			FL_TCP_REC_GPRS	= 0;
			DebugPrint("Error: Tcp Packet Receive Timeout !!!");
			Tcp_SendText("Error: Tcp Packet Receive Timeout !!!");		// for debug
		}
		return;
	}

	Gsm_ResponseAction();
	
	if (TimeOut)
	{
		TimeOut--;
		return;
	}

	if (lastGsmState != GsmState)		// GSM state changed
	{
		lastGsmState = GsmState;
		stateTimeout = 0;
		DebugSprintf(gsmBuff, "State nr: %d, Flags: 0x%04X", lastGsmState, Flags.word);
		DebugPrint(gsmBuff);
//		sprintf(gsmBuff, "State nr: %d, Flags: 0x%04X", lastGsmState, Flags.word);
//		Tcp_SendText(gsmBuff);
	}
	else if (!++stateTimeout)
	{
		#if	(TEST == TEST_GSM)
			if (!FL_TEST_MANUAL)
		#endif
		{
			DebugSprintf(gsmBuff, "State nr: %d, Flags: 0x%04X - TimeOut Error !!!", lastGsmState, Flags.word);
			DebugPrint(gsmBuff);
//			sprintf(gsmBuff, "State nr: %d, Flags: 0x%04X - TimeOut Error !!!", lastGsmState, Flags.word);
//			Tcp_SendText(gsmBuff);
//			GsmState = GsmState_PowerOn;	// restart !!!
			GsmState = GsmState_PowerKeyOn;	// restart !!!
		}
	}

	switch (GsmState)
	{
	case GsmState_Idle:			{
		#if	(TEST == TEST_GSM)
			if (FL_TEST_MANUAL)			break;
		#endif
		if (*recSmsNr)							// SMS received
		{
			GsmUart_SendText("AT+CMGR=");		// read it
			GsmUart_SendText(recSmsNr);
			GsmUart_SendText(";+CMGD=");		// an delete it
			GsmUart_SendText(recSmsNr);
			GsmUart_SendEnter();		// AT+CMGR=1;+CMGD=1\r\n
			*recSmsNr = '\0';
			TimeOut = 200;						// wait 2 seconds
		}
		else if (IdleSeconds >= CsqTime)
		{
			IdleSeconds	= 0;
			GsmState	= GsmState_AT;
		}
		else if (Flags.Status == STATUS_TCP_ACTIVE)
		{
		#if	(TEST == TEST_GSM) || (!defined UART4_WIFI)
			if (FL_TCP_READY)
		#else
			if ((FL_TCP_READY) && (FL_WIFI_OFFLINE))
		#endif
			{
				register TCP_PACKET* packet = Tcp_GetUnsent();
				if (packet)
				{
//					register U16 len = TCP_PACKET_HEDER + packet->len;
					register UNI16 a = {.u16 = 0};
					a.u8l  = packet->len;
					a.u16 += TCP_PACKET_HEDER;					//		LEN_CHANGES
					DebugSprintf(tmp, "Send GPRS Packet: Type_%02X, Len_%03u, Nr_%04X",	packet->type, packet->len, packet->nr);
					DebugPrint(tmp);
					GsmUart_SendData((char*)packet->bytes, a.u16);
//					GsmUart_SendData((char*)packet->bytes, len);
					packet->start = 0;
//					TimeOut		= 2;				// Timeout 0.02 sec
					OkSeconds	= 0;
				}
				else
					FL_TCP_READY = 0;
			}
			else if (SendDataLen)
			{
				GsmUart_SendData(DataToSend, SendDataLen);
				DebugPrint("Gsm -> TCP Data send");
				SendDataLen = 0;
				*DataToSend	= 0;
				TimeOut		= 50;				// Timeout 0.5 sec
				OkSeconds	= 0;
			}
//			else if (OkSeconds >= GPRS_OK_TIMEOUT_S)	// TODO: jaskatàs, cik bie¾i viñ¹ ¹ito sþtïs...
			else if ((OkSeconds >= GPRS_OK_TIMEOUT_S) || (FL_POWER_CHANGES))
			{
				DebugPrint("Gsm -> TCP OK send");
				OkPacket[6] = Csq;	// packet nr as Csq value
//				OkPacket[7] = 0;	// packet nr as Csq value
				Tcp_PreparePacket((TCP_PACKET*)OkPacket, TYPE_OK_GPRS);
				GsmUart_SendData(OkPacket, (TCP_PACKET_HEDER + 4));			// TODO: feel OkPackets statuss (.nr)
//				GsmUart_SendText("Ok\r\n");
				TimeOut		= 2;				// Timeout 0.02 sec
				OkSeconds	= 0;
			}
		}
		else if (Flags.setup)
			GsmState = GsmState_newModule;
		else if (Flags.Status == STATUS_HOME)
		{
			#if (GPRS == OFF_LINE)
				if (!FL_GPRS_OFFLINE)
			#endif
			GsmState = GsmState_Connect;
		}
		else if (Flags.ackErr)
		{
			Flags.csq		= 0;
			Flags.home		= 0;
			Flags.connect	= 0;
			IdleSeconds		= CsqTime;
		}
//		else if (OkSeconds < GPRS_RESTERT_TIMEOUT_S)
			stateTimeout = 0;						// TODO ??????????????????????????
		break;					}
	case GsmState_AT:			{
		if (!stateTimeout)
//		if (!(stateTimeout & 0x3F))
		{
			Flags.Ack = 0;
			if (Flags.transp)
				GSM_DTR_ON();			// switch to command mode
			else
				GsmUart_SendText("AT\r\n");
//			DebugPrint("Gsm - send AT (idle)");
			break;
		}
		else if (Flags.ackOk)
		{
			GSM_DTR_OFF();				// end of command mode switch impulss
			Flags.transp = 0;
			GsmState = GsmState_Csq;
			lastGsmState = GsmState;	// continio imidiately
			stateTimeout = 0;
		}
		else
		{
			if (Flags.ackErr)
			{
				GSM_DTR_OFF();				// end of command mode switch impulss
				Flags.transp	= 0;
				Flags.connect	= 0;
				GsmState		= GsmState_ChipStatus;
			}
			break;
		}						}
	case GsmState_Csq:			{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CSQ\r\n");
		}
		else if (Flags.ackData)
		{
			Csq = atoi(recGsmData);
			Flags.Ack = 0;
			stateTimeout = 1;
		}
		else if (Flags.ackOk)
		{
			Flags.csq	= ((Csq >= 6) && (Csq <= 31))?	1 : 0;
			if		(Flags.connect)							// If TCP Connected
			{
				GsmState	= GsmState_Ato;
				TimeOut		= SMS_WAIT_TIMEOUT_10mS;		// Defined in GsmDefines.h (0.5 sec)
				Flags.connect = 0;
			}
			else if	((!Flags.home) && (Flags.csq))			// if not already at home network
			{
				GsmUart_SendText("AT+CREG?\r\n");			// check network registration
				TimeOut		= 100;							// Timeout 1.0 sec
				Flags.Ack = 0;
			}
			else
				GsmState	= GsmState_Idle;
//			GsmState	= GsmState_sms_check;
			if (LastCsq != Csq)
			{
				LastCsq = Csq;
//				OkPacket[6] = Csq;	// packet nr as Csq value
////			OkPacket[7] = 0;	// packet nr as Csq value
//				Tcp_PreparePacket((TCP_PACKET*)OkPacket, TYPE_OK_GPRS);
				DebugSprintf(gsmBuff, "CSQ = %u, Flags: 0x%04X", Csq, Flags.word);
				DebugPrint(gsmBuff);
			}
		}
		else if (Flags.ackErr)
		{
			Csq				= 0;
			Flags.csq		= 0;
			stateTimeout	= 0;
			GsmState = GsmState_Idle;
			Flags.Ack = 0;
		}
		break;					}
	case GsmState_Creg:			{
		if (recGsmData[2] == ',')
			Flags.home = (recGsmData[3] == '1')?	1 : 0;	// +CREG: 1,1	(after command AT+CREG?)
		else
			Flags.home = (recGsmData[1] == '1')?	1 : 0;	// +CREG: 1		(Unsolicited result code)

		GsmState = GsmState_Idle;
		break;					}
	case GsmState_Ato:			{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("ATO\r\n");					// return to Data Mode (transparent)
			DebugPrint("ATO command\r\n");
			TimeOut		= 200;								// Timeout 2.0 sec
		}
		break;					}
	case GsmState_DataSend:		{		// NOT used currently
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			sprintf(gsmBuff,"AT+CIPSEND=%u\r\n", SendDataLen);
			GsmUart_SendText(gsmBuff);
			DebugPrint(gsmBuff);
		}
		else if ((Flags.ackOk) || (Flags.ackErr))
		{
			GsmState = GsmState_Idle;
			DebugPrint("Gsm Data send Failure!!!");
		}
		else if ((GsmByteReceived()) && (GsmUart_GetByte() == '>'))
		{
			GsmUart_SendData(DataToSend, SendDataLen);
			GsmState = GsmState_Idle;
			SendDataLen = 0;
			TimeOut = 1000;
		}
		else if (stateTimeout > 250)
		{
			GsmState = GsmState_ChipStatus;
			DebugPrint("Gsm '>' not received!!!");
		}
		break;					}
	case GsmState_OKsend:		{		// NOT used currently
//		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("OK\r\n");
			DebugPrint("Gsm -> TCP OK send");
			GsmState = GsmState_Idle;
		}
/*		else if ((Flags.ackOk) || (Flags.ackErr))
		{
			GsmState = GsmState_Idle;
			DebugPrint("Gsm Ok send Failure!!!");
		}
		else if ((GsmByteReceived()) && (GsmUart_GetByte() == '>'))
		{
			GsmUart_SendText("Ok\r\n");
			GsmUart_SendByte(CTRL_Z);
			GsmState = GsmState_Idle;
			TimeOut = 1000;
		}
		else if (stateTimeout > 250)
		{
			GsmState = GsmState_ChipStatus;
			DebugPrint("Gsm '>' not received!!!");
		}
*/
		break;					}
	case GsmState_ChipStatus:	{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CIPSTATUS\r\n");
			DebugPrint("Gsm -> AT+CIPSTATUS");
		}
		else if (stateTimeout > 250)
		{
			DebugPrint("Gsm +CIPSTATUS Timeout -> restart module");
			GsmState = GsmState_PowerOn;				// TODO: normal restart in future
		}
		break;					}
	case GsmState_Connect:		{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CIPSTART=\"tcp\",\"");
			GsmUart_SendText(SERVER);
			GsmUart_SendText("\",\"");
			GsmUart_SendText(PORT);
			GsmUart_SendText("\"\r\n");

//			DebugPrint("Gsm Connect Command");
			DebugSprintf(gsmBuff, "AT+CIPSTART=\"tcp\",\"%s\",\"%s\"\r\n", SERVER, PORT);
//			GsmUart_SendText(gsmBuff);
			DebugPrint(gsmBuff);

		}
		else if (Flags.ackOk)
		{
			Flags.ackOk = 0;
//			GsmState = GsmState_Idle;
//			OkSeconds = 0;
			OkSeconds = GPRS_OK_TIMEOUT_S;				// Temporary
			DebugPrint("Connect command Ok -> wait connection 90sec");
			TimeOut = 9000;				// Timeout 90.0sec
		}
		else if (stateTimeout > 200)
		{
			DebugSprintf(gsmBuff, "AT+CFUN=1,1\r\n", SERVER, PORT);
			GsmUart_SendText(gsmBuff);
			DebugPrint(gsmBuff);
			GsmState = GsmState_PowerKeyOn;

//			GsmUart_SendText("AT+CIPCLOSE\r\n");
//			GsmState = GsmState_DisConnected;
//			DebugPrint("Connection Timeout !!!");
			TimeOut = 1000;				// Timeout 10.0sec
		}
		break;					}
	case GsmState_Connected:	{
		DebugPrint("Gsm GsmState_Connected...");
//		GsmState	= (Flags.transp)?	GsmState_Idle : GsmState_AT;
		GsmState		= GsmState_Idle;
		break;					}
	case GsmState_DisConnected:	{
		GSM_DTR_ON();
		DebugPrint("GPRS Connection Error -> tray to reconnect");
		Flags.csq		= 0;			// wait csq ok
		Flags.connect	= 0;			// Clear connect flag
		Flags.transp	= 0;
//		OkSeconds		= 0;
		GsmState		= GsmState_Idle;
		GSM_DTR_OFF();
		break;					}
	case GsmState_SendOk:		{		// NOT used currently
		OkSeconds		= 0;
		GsmState		= GsmState_Idle;
		break;					}

	case GsmState_PowerOn:		{
		GSM_POWER	= !GSM_POWER;			// 0 - On; 1 - Off
		ledStatus	= LED_STATUS_OFF;
		GSM_DTR_OFF();
		if (GSM_POWER)					// if already powered, restart
		{
			TimeOut = 2000;				// Timeout 20.0sec
			DebugPrint("Gsm Power Restart 2.0 sec");
			return;
		}
		DebugPrint("Gsm Power On 1.0 sec");
		GsmState++;
		TimeOut = 100;					// Timeout 1 sec
		break;					}
	case GsmState_PowerKeyOn:	{
		GSM_RTS_OFF();
		GSM_PK_ON();
		GsmUart_Init(BaudRate_38400);
		GsmFlags		= 0;
		FL_TCP_REC_GPRS	= 0;
		PackRecTimeout	= 0;
		ledStatus		= LED_STATUS_1;
		DebugPrint("Gsm Power Key On 1.0 sec");
		GsmState++;
		TimeOut = 150;					// Timeout 1.5 sec
		break;					}
	case GsmState_PowerKeyOff:	{
		if (GSM_STATUS)
		{
			GSM_PK_OFF();
			GSM_RTS_ON();
//			GSM_DTR_OFF();
			DebugPrint("Gsm Status Ok");
			GsmState++;
			TimeOut = 50;					// Timeout 0.1 sec
		}
		else if ((!GSM_P_KEY) && (stateTimeout > 200))		// Turn off Power key after 2 sec
		{
			GSM_PK_OFF();
			GSM_RTS_ON();
			DebugPrint("Gsm Power Key Off (2 sec timeout)");
		}
		else if (stateTimeout > 250)
			GsmState = GsmState_PowerOn;
		break;					}
	case GsmState_waitRDY:		{
		if (stateTimeout > 200)				// RDY not received
		{
			GsmState = GsmState_newModule;
			DebugPrint("Gsm RDY not received");
		}
		break;					}
	case GsmState_CPIN:			{
		if (stateTimeout > 200)					// +CPIN: READY not received
		{
//			GsmState = GsmState_Idle;			// temporary
			DebugPrint("Gsm CPIN Error");
			LED_GSM		= LED_STATUS_ERROR;		// Indicate ERROR
			GSM_POWER	= 1;					// Module Power Off
			GSM_IE		= 0;					// Gsm Irq Off
			GsmFlags	= 0;					// Gsm flags = 0
			return;
		}
		break;					}
	case GsmState_StartSetup:	{
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CDNSCFG=\"");
			GsmUart_SendText(DNS_1);
			GsmUart_SendText("\",\"");
			GsmUart_SendText(DNS_2);
			GsmUart_SendText("\";+CDNSORIP=1;+CIPMODE=1;+CREG=1\r\n");	// Domain On; Transparent On; Network registration On
			DebugPrint("Gsm set DNS");
		}
		else if (Flags.ackOk)
		{
			DebugPrint("Gsm DNS Ok");
			if (Flags.active)
				GsmState = GsmState_Idle;
			else
			{
				GsmState = GsmState_callReady;
				TimeOut = 3000;
			}
		}
		else if (stateTimeout > 250)
		{
			DebugPrint("Gsm DNS setup Error -> restart module");
			GsmState = GsmState_PowerOn;				// TODO: normal restart in future
		}
		break;					}
	case GsmState_callReady:	{
		if (Flags.active)
			GsmState = GsmState_Idle;
		else if (stateTimeout > 200)					// Call Ready not received
		{
			GsmState = GsmState_Idle;			// temporary
			DebugPrint("Gsm Call_Ready Error !!!");
		}
		break;					}
	case GsmState_newModule:	{
		if		(Flags.ackOk)
			GsmState++;
		else if	(!(stateTimeout & 0x3F))
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT\r\n");
			DebugPrint("Gsm - send AT");
		}
		else if (stateTimeout > 240)
		{
			static	BAUDRATE	InitBaudRate	= BaudRate_9600;
			#if	(TEST == TEST_GSM)
				switch (InitBaudRate)
				{
				case BaudRate_9600:		DebugPrint("Gsm BaudRate changed to 9600");		break;
				case BaudRate_19200:	DebugPrint("Gsm BaudRate changed to 19200");	break;
				case BaudRate_38400:	DebugPrint("Gsm BaudRate changed to 38400");	break;
				case BaudRate_57600:	DebugPrint("Gsm BaudRate changed to 57600");	break;
				case BaudRate_115200:	DebugPrint("Gsm BaudRate changed to 115200");	break;
				case BaudRate_none:		DebugPrint("Gsm no response from module !!!");  break;
				}
			#endif
			if (InitBaudRate == BaudRate_none)
			{										// All baudrates already tested
				LED_GSM		= LED_STATUS_ERROR;		// Indicate ERROR
				GSM_POWER	= 1;					// Module Power Off
//				GSM_IE		= 0;					// Gsm Irq Off
				GsmFlags	= 0;					// Gsm flags = 0
				InitBaudRate	= BaudRate_9600;
				GsmState	= GsmState_PowerOn;
				TimeOut		= 12000;					// Timeout 120.0sec
				return;
			}
			GsmUart_SetBaudrate(InitBaudRate++);
			stateTimeout = 0;
			Flags.Ack = 0;
			GsmUart_SendText("AT\r\n");
		}
		break;					}
	case GsmState_ModuleSetup:	{	// Set & STORE CURRENT PARAMETER TO USER DEFINED PROFILE
		if (!(stateTimeout & 0x3F))
		{
			Flags.Ack = 0;
			GsmUart_SendText("ATE0;+CMEE=0;+IFC=2,2;+IPR=38400;&D1;&W\r\n");
			DebugPrint("Gsm send module setup parameters");
		}
		else if (Flags.ackOk)
		{
			GsmState = GsmState_SmsSetup;
			DebugPrint("Gsm setup ok");
		}
		break;					}
	case GsmState_SmsSetup:	{	// Set & STORE CURRENT PARAMETER TO USER DEFINED PROFILE
		if (!(stateTimeout & 0x3F))
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CLIP=1;&W;+COLP=1;&W;+COPS=0;&W;+CMGF=1;&W;+CNMI=2,1,0,0,0;&W;+CSAS=0;&W\r\n");
			DebugPrint("Gsm send SMS setup parameters");
		}
		else if (Flags.ackOk)
		{
			GsmState = GsmState_setAPN;
			DebugPrint("Gsm SMS setup ok");
		}
		break;					}
	case GsmState_setAPN:		{	// Set TCPIP APPLICATION CONTEXT & SAVE in module non volatile memory
		if (!stateTimeout)
		{
			Flags.Ack = 0;
			GsmUart_SendText("AT+CDNSORIP=1;+CIPSPRT=1;+CIPHEAD=0;+CIPFLP=0;+CIPSRIP=0;+CIPCSGP=1,\"");
			GsmUart_SendText(APN);
			GsmUart_SendText("\";+CIPCCFG=3,2,256,1;+CIPSCONT\r\n");
			DebugPrint("Gsm GPRS parameters setup");
		}
		else if (Flags.ackOk)
		{
			GsmState = GsmState_PowerOn;				// TODO: normal restart in future
			DebugPrint("Gsm gprs setup ok -> restart module");
		}
		else if (stateTimeout > 200)
		{
			DebugPrint("Gsm gprs setup Error -> restart module");
			GsmState = GsmState_PowerOn;				// TODO: normal restart in future
		}
		break;					}
	case GsmState_sms_received:	{
		U8 i = 0;
//		Tcp_SendText(irqBuf);
		while((*recGsmData) && (*recGsmData++ != ','));								// +CMTI:”SM”,12
		while((IS_DIGIT(*recGsmData)) && (i < (sizeof(recSmsNr) - 1)))
			recSmsNr[i++] = *recGsmData++;
		recSmsNr[i] = '\0';
		sprintf(gsmBuff, "Sms Nr: %s received", recSmsNr);
		Tcp_SendText(gsmBuff);
//		DebugSprintf(gsmBuff, "Sms Nr: %s received", recSmsNr);
		DebugPrint(gsmBuff);
		GsmState = GsmState_Idle;
		break;					}
	case GsmState_sms_check:	{
		if (!stateTimeout)
		{
			GsmUart_SendText("AT+CMGL\r\n");
			Flags.Ack = 0;
		}
		else if (Flags.ackOk)
		{
			if		(Flags.connect)							// If TCP Connected
			{
				GsmUart_SendText("ATO\r\n");				// return to Data Mode (transparent)
				TimeOut		= 10;							// Timeout 0.1 sec
			}
			else if	((!Flags.home) && (Flags.csq))			// if not already at home network
			{
				GsmUart_SendText("AT+CREG?\r\n");			// check network registration
				TimeOut		= 50;							// Timeout 0.5 sec
			}
			GsmState	= GsmState_Idle;
		}
		break;					}
	case GsmState_sms_read:		{
		if (!stateTimeout)
		{
//			Tcp_SendText(irqBuf);
			Flags.Ack = 0b10000;		// sms=1, rec=0, ackData=0, ackErr=0, ackOk=0
			if((QuotationPhoneNr(QUOTE_SMS_R_NUMBER) >= QUOTE_SMS_R_TIME) &&			// sms have 3 Quotations (number, Sender NAME, time)
			   (QuotationText(QUOTE_SMS_R_CONTACT))						)
			{
				Flags.knownCaller = 1;
				DebugPrint(callTxt);
				DebugPrint(", (SIM number): ");										// 
			}
			else if (callNr[0])
				DebugPrint("unknown number: ");										// 
			else
				DebugPrint("unknown sms error ");
			DebugPrint(callNr);
			Tcp_SendText(callNr);
			TimeOut		= 100;								// Timeout 1.0 sec
		}
		else if (Flags.ackData)
			SmsAnalize();
		else if (Flags.ackOk || Flags.ackErr ||
				 (stateTimeout > 200))
			GsmState = GsmState_Idle;
		break;					}
	case GsmState_Reset:		{
		__asm__ ("RESET");
		while(1);		break;	}

	default:					{
		DebugSprintf(gsmBuff, "State nr: %d, Flags: 0x%04X - State Error !!!", GsmState, Flags.word);
		DebugPrint(gsmBuff);
		GsmState = GsmState_Idle;
		break;					}
	}
	if(GsmStringReceived())
		GSM_IF = 1;
}

inline	void	GprsPacketReceive	(U8 data)
{
	static TCP_PACKET*	inPacket;
	static U8			currentPacket = 0, crc;
	static U16			idx = 0;
	static UNI16		recLen;

	if (!FL_TCP_REC_GPRS)
	{
		FL_TCP_REC_GPRS	= 1;			// Set flag to receive GPRS packet
		PackRecTimeout = TCP_PACKET_TIMEOUT;
		currentPacket ^= 1;				// change to ather packet buffer
		inPacket	= &PacketIn[currentPacket];	// Recive pointer to free buffer
		idx			= 0;				// buffer index = 0
		crc			= 0;				// clear crc
		recLen.u16  = 0;
		PacketRec	= NULL;
	}
	inPacket->bytes[idx++] = data;		// save current byte
	if (idx > 3)						// if Start, Lenght, CRC bytes already received
	{
		crc ^= data;					// calculate crc
		if (idx >= recLen.u16)			// if packet len bytes received	LEN_CHANGES
		{
			FL_TCP_REC_GPRS	= 0;		// turn off gprs packet receive
			PackRecTimeout	= 0;
			if (crc == inPacket->crc)	// if crc ok
			{
				PacketRec	= inPacket;
				GSM_IF			= 1;	// enable GSM IRQ
			}
			else						// for debug
			{
				sprintf(gsmBuff,"Broken Tcp: type_%02Xh, len_%u, crc_%02Xh, rc_crc_%02Xh", inPacket->type, inPacket->len, inPacket->crc, crc);
				Tcp_SendText(gsmBuff);
				DebugPrint(gsmBuff);
			}
/*			U8* ptr = inPacket->bytes;
			DebugSprintf(gsmBuff,"%02X %02X %02X(%02X) %02X %02X %02X %02X %02X",
						ptr[0], ptr[1], ptr[2], crc, ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
			DebugPrint(gsmBuff);
*/
		}
	}
	else if (idx == 2)											//		LEN_CHANGES
	{
		recLen.u8l  = inPacket->len;
		recLen.u16 += TCP_PACKET_HEDER;
	}
}

S8 SendTcpPacket	(char* ptr, U16 len)
{
	if (SendDataLen)
		return	-1;
	DebugSprintf(tmp,"Send Tcp - Type: %02X, ID: %04X, Len: %u", ptr[3], *(U16*)&ptr[4], *(U8*)&ptr[1]);
	DebugPrint(tmp);
	DataToSend	= ptr;
	SendDataLen	= len + TCP_PACKET_HEDER;						//		LEN_CHANGES
	return 0;
}

U8 GprsConnected	(void)
{
	return Flags.connect;
}

void Gsm_Setup		(void)
{
	GsmUart_SendText("AT+IPR=9600;&W\r\n");
	__asm__ ("RESET");
	while(1);
//	Flags.setup = 1;
}

void TcpPacket	(U8 type)
{
	static U16 packetNr	= 0;
	U8	i;
	PacketOut.start		= STX;
	PacketOut.crc		= 0;
	PacketOut.type		= type;
	PacketOut.id		= 0x333;
	PacketOut.nr		= packetNr++;
	GetTcpTime((U32*)&PacketOut.data[0]);
//	PacketOut.time		= 0;

	PacketOut.len		= 12;
	for (i = 3; i < PacketOut.len; i++)
		PacketOut.crc ^= PacketOut.bytes[i];
	PacketOut.len		= 4;									//		LEN_CHANGES

	SendTcpPacket((char*)&PacketOut, PacketOut.len);
//	DataToSend	= (char*)PacketOut.bytes;
//	SendDataLen	= PacketOut.len;
}

// extract Phone number from string by quotationNr
// return number of Quotations
inline U8 QuotationPhoneNr (U8 quotationNr)
{
	U8		i = 0, j = 0;
	char*	ptr	= recGsmData;
	while(*ptr)
	{
		if		(*ptr == '"')		i++;
		else if	(i == quotationNr)	callNr[j++] = *ptr;
		ptr++;
	}
	callNr[j] = 0;
	return i;
}

// extract "text" from string by quotationNr
// return number chars within Quotations
inline U8 QuotationText (U8 quotationNr)
{
	U8		i = 0, j = 0;
	char*	ptr	= recGsmData;
	while(*ptr)
	{
		if (*ptr == '"')
			if (++i > quotationNr)	break;
		if	(i == quotationNr)		callTxt[j++] = *ptr;
		ptr++;
	}

	if (j > 1)
	{
		callTxt[j++] = '"';
		callTxt[j] = 0;
		return (j - 2);
	}
	callTxt[0] = 0;
	return NULL;
}

inline void	GetSmsText (char* ptr)
{
	while (true)
	{
		U8 data = GsmUart_GetByte();
		if		(data == CR)		break;
		else if	(data == LF)		*ptr++ = '>';
		else						*ptr++ = data;
	}
	*ptr = '\0';
	Flags.Ack = 0b01100;		// sms=0, rec=1, ackData=1, ackErr=0, ackOk=0
	TimeOut	= 0;
}

inline void	SmsAnalize (void)
{
	char* ptr = irqBuf;
//	Tcp_SendText(irqBuf);

	while(*ptr)
	{
		if (*ptr == '>')
		{
			register U8 i = 0;
			ptr++;
			while (ptr[i] && (ptr[i] != '>'))	i++;
			SmsCommand(ptr, i);
 			ptr	= &ptr[i];
		}
		else
			ptr++;
	}
	Flags.ackData = 0;
}
//___________________________SMS__SMS__SMS__SMS__SMS__SMS__SMS___________________________
inline void	SmsCommand (char* command, U8 len)
{
	if(*command)
	{
		U8 i;
		for(i = 0; i < (sizeof(SmsCommands) / sizeof(SMS_COMAND)); i++)
		{
			if(!(memcmp(command, SmsCommands[i].text, SmsCommands[i].size)))
				return SmsCommands[i].funct(&command[SmsCommands[i].size]);
		}
	}
}

inline void SmsFunction_Reset	(char* ptr)
{
	if (!Flags.knownCaller)
	{
		__asm__ ("RESET");
		while(1);
	}
}

inline void	SmsFunction_Tcp		(char* ptr)
{
	register U16 data = atoi(ptr);
	sprintf(gsmBuff,"Sms: TCP nr %u", data);
	Tcp_SendText(gsmBuff);

//	memset(PacketOut.bytes,0, sizeof(PacketOut.bytes));
//	PacketOut.start		= STX;
//	PacketOut.crc		= 0;
	PacketOut.type		= data;
//	PacketOut.id		= 0x333;
//	PacketOut.nr		= packetNr++;
//	GetTcpTime((U32*)&PacketOut.data[0]);
	switch (data)
	{
	case TYPE_ONLINE:
		PacketOut.dat_16[0]	= 60;		// OnlineTimer
		PacketOut.dat_16[1]	= 3;		// OnlineTimeoutSec
//		*(U16*)&PacketRec->data[0] = 60;
//		*(U16*)&PacketRec->data[2] = 3;
		break;
	}

	Tcp_PacketReceived(&PacketOut);
}

inline void SmsFunction_Test	(char* ptr)
{
	Tcp_SendText(ptr);
}

inline void SmsFunction_Help	(char* ptr)
{
	Tcp_SendText(ptr);
}

U8	Gsm_GetCsq		(void)
{
	return Csq;
}

U16	Gsm_GetFlags	(void)
{
	return Flags.word;
}

U16	Gsm_GetState	(void)
{
	return (U16)GsmState;
}
