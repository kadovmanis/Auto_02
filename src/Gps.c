#include	"Gps.h"
#include	"GpsDefines.h"
#include	"Ports.h"
#include	"Tcp.h"
#include	"Uart1.h"
#include	<string.h>
#include	<stdlib.h>
//	8PG4-B7EM-CKUS
#define		SHORT_PACKETS		0
#if		(SHORT_PACKETS == 1)
	#define	PACKET_MIN_REMAIN	230
#else
//	#define	PACKET_MIN_REMAIN	4
	#define	PACKET_MIN_REMAIN	6
#endif

#if	(TEST == GPS_TEST)
  #include	"Debug.h"
  static char	tmp[80];
#else
  #define	DebugPrint(X)
  #define	DebugSprintf(stream,fmt,a...)
#endif

#define	UART1_TX		U1STAbits.UTXEN
#define	UART1_RX		U1MODEbits.UARTEN
						// ŠITAIS JAPĀRBAUDA!!!!!!!
/*
Lat		56.9975209 * 10000000 = 569975209
1 degree	= ~111.32km	= 11'132'000cm
(1 min		= 1855.33m	= 185'533cm)
0.0000001	= 1.1132cm

Lon		24.2734487 * 10000000 = 242734487
1 degree	= ~63.75km	= 6'375'000cm
(1 min		= 1062.5m	= 106'250cm)
0.0000001	= 0.6375cm

1.1132 * 32 * 256	= 9119.3344		83162259.89902336
0.6375 * 64 * 256	= 10444.8		109093847.04
									192256106.9390234	13865.64484396681

56.9298437	24.1790102
56.9220801	24.1672319
77636		117783
2426.125	1840.359375

131040		262080			12 bit
1458.7373m	1670.76m		2,217.96

1.1132*32	0.6375*64
35.6224cm	40.8cm			54.16cm 2D


leņkis
ātrums (nobrauktais attālums sekundē)
laiks
*/

#define	MAX_DIFF_LAT	269	// 134	//(150 / 1.1132)	// 150cm / 1.1132 = 134.7 
#define	MAX_DIFF_LON	470	// 235	//(150 / 0.6375)	// 150cm / 0.6375 = 235

//#define	MIN_LAT_CM_DIFF	300
//#define	MIN_LON_CM_DIFF	300
#define	MAX_DIFF_SEC	60
#define	MIN_DISTANCE	100		// cm/s (3.6km/h)
#define	MIN_SPEED		100		// cm/s (3.6km/h)
#define	MIN_ACCEL		100		// cm/s (3.6km/h)

#define	DEGREE_H360		0x0226	// 360.00000 = 0x0225 5100	(0x2250000 = 359.79264; 0x2260000 = 360.44800)
#define	DEGREE_H180		0x0113	// 180.00000 = 0x0112 A880	(0x1120000 = 179.56864; 0x1130000 = 180.22400)
#define	MIN_DEGREES		0x000C	// 7.86432'	 = 0x000C 0000

//#define	MAX_DIFF_LAT	((U32)(MIN_LAT_CM_DIFF * 10000) / 11132)
//#define	MAX_DIFF_LON	((U32)(MIN_LON_CM_DIFF * 10000) / 6375)

U16						MaxDiff_Lat = MAX_DIFF_LAT;
U16						MaxDiff_Lon = MAX_DIFF_LON;
U16						MaxDiff_Sec = MAX_DIFF_SEC;
static volatile GPS_FLAGS	GpsFlags;
static GPS_UBX_DATA		UbxDataReceived, UbxDataStart, UbxDataDiff;
static GPS_OFFSET_2		Offset;
//static GPS_UBX_DATA		UbxDataReceived, UbxDataSaved;
static			U8		GpsFix = 0xFF, GpsDataType = 0;
static			UNI32	latDiff, lonDiff, SpeedDiff;
static			U16		GpsSec = 0, DegDiff; //, DistanceCm = 0;
//static GPS_MSG*			pMsg;
#ifdef	UART1_GPS
  static U8				RxBuff[RX_BUFF_COUNT][RX_BUFF_SIZE];
#endif
static enum
{
	diff_null = 0,
	diff_8bit,
	diff_12bit,
	diff_16bit,
	diff_17bit,
	diff_32bit
} GpsDiff = diff_null;


//static volatile	U8			Section;
void Gps_On(void)
{
	GpsFlags.word	= 0;
	memset((char*)&UbxDataReceived,	0, sizeof(UbxDataReceived));
	memset((char*)&UbxDataStart,	0, sizeof(UbxDataStart));
	memset((char*)&UbxDataDiff,		0, sizeof(UbxDataDiff));
//	memset((char*)&UbxDataSaved, 0, sizeof(UbxDataSaved));

	Uart1_Init(BaudRate_9600);
	GPS_IF = 0;		// clear interrupt flag and
	GPS_IE = 1;		// enable GPS extend IRQ
	#if	(TEST == GPS_TEST)
		GPS_POWER = !GPS_POWER;
		if (GPS_POWER)
		{
			DebugPrint("Enable GPS Power, Uart1, GpsIrq");
			LED_GPS = LED_STATUS_1;
		}
		else
		{
			GPS_IE = 0;		// disable GPS extend IRQ
			DebugPrint("Disable GPS Power");
			LED_GPS = LED_STATUS_OFF;
		}
	#else
		GPS_POW_ON();
		LED_GPS	= LED_STATUS_1;
	#endif
}

// {Ubx_Class, UbxId,... 
// len = sizeof(payload) - 2 (len size)
void Gps_SendUbx(const U8* data, U16 len)
{
	register U8	ck_A, ck_B = 0;
	register UNI16	l;
	l.u16 = len;
	Uart1_SendByte(UBX_SYNC_CHAR_1);// 0xB5
	Uart1_SendByte(UBX_SYNC_CHAR_2);// 0x62
	ck_B  = *data++;
	ck_A  = *data++;
	Uart1_SendByte(ck_B);			// Ubx_Class
	Uart1_SendByte(ck_A);			// UbxId
	Uart1_SendByte(l.u8l);			// len low	byte
	Uart1_SendByte(l.u8h);			// len high	byte

	ck_A += ck_B;	ck_B += ck_A;	// checkSum of Ubx_Class & UbxId
	ck_A += l.u8l;	ck_B += ck_A;	// checkSum += len low	byte
	ck_A += l.u8h;	ck_B += ck_A;	// checkSum += len high	byte

	while(len)
	{
		register U8	a = *data++;
		Uart1_SendByte(a);
		ck_A += a;	ck_B += ck_A;	// checkSum += data	bytes
		len--;
	}
	Uart1_SendByte(ck_A);			// send checkSum A
	Uart1_SendByte(ck_B);			// send checkSum B
}

// **********************************************************************
//	Rx1 INTERRUPT
// ......................................................................
inline void	GpsSectionWaitIrq(U8 rxData);
inline void	GpsSectionUbxIdIrq(U8 rxData);
inline void	GpsSectionCrcIrq(U8 rxData);
inline void	GpsDataSaveIrq(U8 rxData);
inline U8	GpsHederSortIrq(void);

inline void	Ubx_Ack			(GPS_MSG*		rxData);
inline void	Ubx_NavStatus	(NAV_STATUS*	rxData);
inline void	Ubx_NavTimeout	(NAV_TIMEUTC*	rxData);
inline void	Ubx_NavPosllh	(NAV_POSLLH*	rxData);
inline void	Ubx_NavVelned	(NAV_VELNED*	rxData);
inline void	Ubx_NavPvt		(NAV_PVT*		rxData);
inline void	Ubx_Uncnown		(GPS_MSG*		rxData);
inline void Ubx_DataAnalize	(void);
// **********************************************************************
#ifdef	UART1_GPS
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void )
{
	do										// at least one Receive Buffer Data Available
	{
		static U8*	pRx = RxBuff[0];
		static U16	ubxLen = 0, ubxIdx = 0;
		static U8	buffIdx = 0, ck_a = 0, ck_b = 0;

		register U8	rxData	= U1RXREG;		// read from uart1 Rx buffer
		_U1RXIF		= 0;					// Clear the Recieve Interrupt Flag

		if (GpsFlags.ubx_rec)				// UBX MSG Started
		{
			*pRx++ = rxData;
			if (++ubxIdx == 1)
			{
				if (rxData != UBX_SYNC_CHAR_2)
					GpsFlags.ubx_rec = 0;
			}
			else if (ubxIdx < ubxLen)
			{
				ck_a += rxData;				// Checksum
				ck_b += ck_a;
				if (ubxIdx == 6)			// Payload lenght received
					ubxLen = (*(U16*)&RxBuff[buffIdx][4] + ubxIdx);
			}
			else if (ubxIdx > ubxLen)		// Checksum test
			{
				if ((RxBuff[buffIdx][ubxLen]== ck_a) &&
					(rxData					== ck_b)	)
				{
					GPS_IF = 1;
					pMsg = (GPS_MSG*)RxBuff[buffIdx];
				}
			#if	(TEST == GPS_TEST)
				else if (GPS_IE)
				{
					DebugSprintf(tmp,"Cl:%02X, Id:%02X, Len:%2u, Idx:%2d, ck_a:%02X_%02X, ck_b:%02X_%02X",
								RxBuff[buffIdx][2], RxBuff[buffIdx][3], ubxLen, ubxIdx, RxBuff[buffIdx][ubxLen], ck_a, rxData, ck_b);
					DebugPrint(tmp);
				}
			#endif
				if (++buffIdx >= RX_BUFF_COUNT)
					buffIdx = 0;
				pRx = RxBuff[buffIdx];
				GpsFlags.ubx_rec = 0;
			}
		}
		else if (rxData == UBX_SYNC_CHAR_1)	// UBX MSG Start
		{
			GpsFlags.ubx_rec	= 1;
			pRx = RxBuff[buffIdx];
			*pRx++		= rxData;
			ubxIdx		= 0;
			ck_a = ck_b	= 0;
			ubxLen		= 0xFFFF;
		}
		else if (rxData == CR)				// End of text msg (NMEA, TXT etc)
		{
			*pRx = '\0';
			pMsg = (GPS_MSG*)RxBuff[buffIdx];
			if (++buffIdx >= RX_BUFF_COUNT)
				buffIdx = 0;
			pRx = RxBuff[buffIdx];
			GPS_IF = 1;
		}
		else if (rxData != LF)
			*pRx++	= rxData;
	} while (U1STAbits.URXDA);
	U1STAbits.OERR = 0;
}
#endif
// **********************************************************************
//	GPS_EXT Interrupt (PMP Interrupt used)
// **********************************************************************
void GPS_INTERRUPT	( void )
{
#ifndef	UART1_GPS
	GPS_IF	= 0;			// Clear Interrupt Flag
	char buff[256];

	Uart1_GetString((P_U8)buff);
	DebugPrint(buff);
#else
	GPS_IF	= 0;			// Clear Interrupt Flag
	if (pMsg->ubxSync16 == UBX_SYNC_INT)
	{
		if		(pMsg->ubxClass	  == UBX_CLASS_ACK)			Ubx_Ack			(				pMsg			);
		else if	(pMsg->ubxClassId == UBX_ID_NAV_STATUS)		Ubx_NavStatus	((NAV_STATUS*)	pMsg->ubxData	);
		else if	(pMsg->ubxClassId == UBX_ID_NAV_TIMEUTC)	Ubx_NavTimeout	((NAV_TIMEUTC*)	pMsg->ubxData	);
		else if	(pMsg->ubxClassId == UBX_ID_NAV_POSLLH)		Ubx_NavPosllh	((NAV_POSLLH*)	pMsg->ubxData	);
		else if (pMsg->ubxClassId == UBX_ID_NAV_VELNED)		Ubx_NavVelned	((NAV_VELNED*)	pMsg->ubxData	);
		else if	(pMsg->ubxClassId == UBX_ID_NAV_PVT)		Ubx_NavPvt		((NAV_PVT*)		pMsg->ubxData	);	// Not implemented in u-blox5
		else												Ubx_Uncnown		(				pMsg			);
		return;
	}
	else if	(pMsg->NMEA_MSG[0]			== 'G')
	{
		if		(pMsg->NMEA_MSG[1]		== 'S')
		{
			if		(pMsg->NMEA_MSG[2]	== 'A')
			{
//				Uart1_SendText("$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n");	// switch GSA protocol OFF
				SEND_UBX_STRING(CfgMsgRate_GSA);		// Set GSA		(nmea string - off)
				SEND_UBX_STRING(CfgMsgRate_TIMEUTC);	// Set TIMEUTC	(ubx         - 1Hz)
			}
			else if (pMsg->NMEA_MSG[2]	== 'V')
			{
//				Uart1_SendText("$PUBX,40,GSV,0,0,0,0,0,0*59\r\n");	// switch GSV protocol OFF
				SEND_UBX_STRING(CfgMsgRate_GSV);		// Set GSA		(nmea string - off)
				SEND_UBX_STRING(CfgMsgRate_POSLLH);		// Set POSLLH	(ubx         - 1Hz)
			}
		}
		else if	((pMsg->NMEA_MSG[1]		== 'G') &&
				 (pMsg->NMEA_MSG[2]		== 'A')	)
		{
//			Uart1_SendText("$PUBX,40,GGA,0,0,0,0,0,0*5A\r\n");	// switch GGA protocol OFF
			SEND_UBX_STRING(CfgMsgRate_GGA);		// Set GGA		(nmea string - off)
			SEND_UBX_STRING(CfgMsgRate_STATUS);		// Set STATUS	(ubx         - 1Hz)
//			SEND_UBX_STRING(CfgMsgRate_PVT);		// Set PVT		(ubx         - 1Hz) NOT supported on u-blox5 :(
		}
		else if	((pMsg->NMEA_MSG[1]		== 'L') &&
				 (pMsg->NMEA_MSG[2]		== 'L')	)
		{
//			Uart1_SendText("$$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n");	// switch GLL protocol OFF
			SEND_UBX_STRING(CfgMsgRate_GLL);		// Set GLL		(nmea string - off)
			SEND_UBX_STRING(CfgMsgRate_VELNED);		// Set VELNED	(ubx         - 1Hz)
		}
	}
	else if	((pMsg->NMEA_MSG[0] == 'Z') &&
			 (pMsg->NMEA_MSG[1] == 'D') &&
			 (pMsg->NMEA_MSG[2] == 'A')	)
				Uart1_SendText("$PUBX,40,ZDA,0,0,0,0,0,0*44\r\n");	// switch ZDA protocol OFF
	else if	((pMsg->NMEA_MSG[0] == 'V') &&
			 (pMsg->NMEA_MSG[1] == 'T') &&
			 (pMsg->NMEA_MSG[2] == 'G')	)
			 {
//				Uart1_SendText("$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n");	// switch VTG protocol OFF
				SEND_UBX_STRING(CfgMsgRate_VTG);	// Set VTG		(nmea string - off)
			 }
	else if	((pMsg->NMEA_MSG[0] == 'R') &&
			 (pMsg->NMEA_MSG[1] == 'M') &&
			 (pMsg->NMEA_MSG[2] == 'C')	)
			 {
//				Uart1_SendText("$PUBX,40,RMC,0,0,0,0,0,0*47\r\n");	// switch RMC protocol OFF
				SEND_UBX_STRING(CfgMsgRate_RMC);	// Set RMC		(nmea string - off)
			 }

	DebugPrint((char*)pMsg);
#endif
}

inline void	Ubx_Uncnown		(GPS_MSG*		rxData)
{
	DebugSprintf(tmp, "Class:%02X,ID:%02X,Len:%u ", rxData->ubxClass, rxData->ubxID, rxData->ubxLen);
	DebugPrint(tmp);
}

inline void	Ubx_Ack			(GPS_MSG*		rxData)
{
//	if (rxData->ubxID)		// 0x01- ACK; 0x00- NAK
//		DebugSprintf(tmp, "ACK of %02X %02X : Ok", rxData->ubxData[0], rxData->ubxData[1]);
//	else
//		DebugSprintf(tmp, "ACK of %02X %02X : Error", rxData->ubxData[0], rxData->ubxData[1]);
	DebugSprintf(tmp, "ACK of %02X %02X : %s", rxData->ubxData[0], rxData->ubxData[1], (rxData->ubxID)?	"Ok":"Error");
	DebugPrint(tmp);
}

inline void	Ubx_NavStatus	(NAV_STATUS*	rxData)
{
	if (GpsFix != rxData->gpsFix)
	{
		GpsFix = rxData->gpsFix;
		GpsFlags.changes_gps = 1;
		switch (GpsFix)
		{
			case 0:		LED_GPS	= LED_STATUS_1;		break;	// 0x00 = no fix	GPSfix Type
			case 1:		LED_GPS	= LED_STATUS_2;		break;	// 0x01 = dead reckoning only
			case 2:		LED_GPS	= LED_STATUS_3;		break;	// 0x02 = 2D-fix
			case 3:		LED_GPS	= LED_STATUS_ON;	break;	// 0x03 = 3D-fix
			case 4:		LED_GPS	= LED_STATUS_2;		break;	// 0x04 = GPS + dead reckoning combined
			case 5:		LED_GPS	= LED_STATUS_2;		break;	// 0x05 = Time only fix
			default:	LED_GPS	= LED_STATUS_ERROR;	break;	// 0x06..0xff = reserved
		}
	}
//	DebugSprintf(tmp, "Fix:%d, FistFix: %ld sec, Reset: %ld", rxData->gpsFix, rxData->ttff, rxData->msss);
//	DebugPrint(tmp);
}

inline void	Ubx_NavTimeout	(NAV_TIMEUTC*	rxData)
{
	static	 S8	secLast = 0;
	if (GpsFix >= GPS_TIME_FIX)
	{
		memcpy((char*)&SysTime, (char*)&rxData->year, (sizeof(TIME_ST) - 1));
		SysTime.gpsFix = GpsFix;
	}
	else 
		SysTime.gpsFix = 0;
	memcpy((char*)&UbxDataReceived.year, (char*)&SysTime, sizeof(TIME_ST));

	register S8 diff = (SysTime.sec - secLast);
	secLast = SysTime.sec;
	register U16 max = (FL_WEB_ONLINE)? OnlineTimeoutSec : 60;
	if (diff < 0)		diff += 60;
	if ((GpsSec += diff) >= max)	GpsFlags.changes_gps = 1;
//	DebugSprintf(tmp, "UTC Time: %2u:%02u:%02u; date: %2u:%02u:%04u",
//				rxData->hour, rxData->min, rxData->sec, rxData->day, rxData->month, rxData->year);
//	DebugPrint(tmp);
	Ubx_DataAnalize();
}

inline void	Ubx_NavPosllh	(NAV_POSLLH*	rxData)
{
	memset(Offset.bytes, 0, sizeof(Offset));
	if (GpsFix == GPS_3D_FIX)
	{
		memcpy((char*)&UbxDataReceived.lon, (char*)&rxData->lon, 8);
		latDiff.s32 = UbxDataReceived.lat.s32 - UbxDataStart.lat.s32;
		lonDiff.s32 = UbxDataReceived.lon.s32 - UbxDataStart.lon.s32;
		if (latDiff.u8hh & 0x80){	latDiff.s32 = 0 - latDiff.s32;	Offset.latSign = 1;	}
//		if (latDiff.u8hh & 0x80){	latDiff.s32 *= -1;	Offset.latSign = 1;	}
//		if (latDiff.s32 < 0)	{	latDiff.u32 = abs(latDiff.s32);	Offset.latSign = 1;	}
		if (lonDiff.u8hh & 0x80){	lonDiff.s32 = 0 - lonDiff.s32;	Offset.lonSign = 1;	}
//		if (lonDiff.u8hh & 0x80){	lonDiff.s32 *= -1;	Offset.lonSign = 1;	}
//		if (lonDiff.s32 < 0)	{	lonDiff.u32 = abs(lonDiff.s32);	Offset.lonSign = 1;	}

//		0000 00FF	diff_8bit	TYPE_GPS_DATA
//		0000 0FFF	diff_12bit	TYPE_GPS_DATA_3
//		0000 FFFF	diff_16bit
//		0001 FFFF	diff_17bit
//		FFFF FFFF	diff_32bit
		if ((latDiff.u16h) || (lonDiff.u16h))
		{
			if ((latDiff.u32 > 0x1FFFF) || (lonDiff.u32 > 0x3FFFF))			// FFFF FFFF
			{
				GpsDiff = diff_32bit;
				GpsFlags.changes_32	= 1;
			}
			else															// 0001 FFFF
			{
				GpsDiff = diff_17bit;
				if (GpsDataType <= TYPE_GPS_DATA_3)		// TODO
					GpsFlags.changes_32	= 1;
			}
		}
		else if ((!latDiff.u8lh) && (!lonDiff.u8lh))						// 0000 00FF
			GpsDiff = diff_8bit;
		else
		{
			if ((latDiff.u16l > 0xFFF) || (lonDiff.u16l > 0xFFF))			// 0000 FFFF
			{
				GpsDiff = diff_16bit;
				if (GpsDataType <= TYPE_GPS_DATA_3)
					GpsFlags.changes_32	= 1;
			}
			else															// 0000 0FFF
			{
				GpsDiff = diff_12bit;
				if (GpsDataType <= TYPE_GPS_DATA_2)		// TODO
					GpsFlags.changes_32	= 1;
			}
		}
	}
	else if (latDiff.s32 != 0)
	{
		memset((char*)&UbxDataReceived.lon, 0, 8);
		latDiff.s32 = 0;
		lonDiff.s32 = 0;
	}
}

inline void	Ubx_NavVelned	(NAV_VELNED*	rxData)
{
	if (GpsFix == GPS_3D_FIX)
	{
/*		UbxDataReceived.gSpeed.u32 += &rxData->gSpeed.u32;
		UbxDataReceived.heading.s32 += &rxData->heading.s32;

		memcpy((char*)&UbxDataReceived.gSpeed, (char*)&rxData->gSpeed, 8);	// copy NAV_VELNED gSpeed, heading
		
		MIN_DISTANCE
		
		
		if (UbxDataReceived.gSpeed.u16l >= MIN_SPEED)
			GpsFlags.changes_mot	= 1;


		DegDiff = DIFFERENCE(UbxDataReceived.heading.u16h, UbxDataStart.heading.u16h);
		if (DegDiff > DEGREE_H180)
			DegDiff = DEGREE_H360 - DegDiff;
		if (DegDiff > MIN_DEGREES)
			GpsFlags.changes_dir	= 1;

		SpeedDiff.u32	= abs(UbxDataReceived.gSpeed - UbxDataStart.gSpeed);
		if (SpeedDiff.s16h)
			GpsFlags.changes_32	= 1;
		else if (SpeedDiff.u16l > MIN_ACCEL)
			GpsFlags.changes_acc	= 1;

		DistanceCm = UbxDataReceived.gSpeed.
*/		
		if (UbxDataReceived.gSpeed.u32 >= MIN_SPEED)
		{
			GpsFlags.changes_mot	= 1;
			SpeedDiff.u32	= abs(UbxDataReceived.gSpeed.u32 - UbxDataStart.gSpeed.u32);
			if (SpeedDiff.s16h)
			{
				GpsFlags.changes_gps	= 1;
				GpsFlags.changes_32	= 1;
			}
			else if (SpeedDiff.u16l > MIN_ACCEL)
				GpsFlags.changes_gps	= 1;
			else
			{
				DegDiff = DIFFERENCE(UbxDataReceived.heading.u16h, UbxDataStart.heading.u16h);
				if (DegDiff > DEGREE_H180)
					DegDiff = DEGREE_H360 - DegDiff;
				if (DegDiff > MIN_DEGREES)
					GpsFlags.changes_gps	= 1;
			}
		}
		else
		{
			GpsFlags.changes_mot	= 0;
			SpeedDiff.u32	= 0;
			DegDiff			= 0;
		}
		memcpy((char*)&UbxDataStart.gSpeed, (char*)&UbxDataReceived.gSpeed, 8);
	}

//	DebugSprintf(tmp, "Speed: %ld cm/s; speedAcc: %ld; Heading: %ld, acc: %ld ",
//				rxData->gSpeed, rxData->sAcc, rxData->heading, rxData->cAcc);
//	DebugPrint(tmp);
}

inline void	Ubx_NavPvt		(NAV_PVT*		rxData)		// Not implemented in u-blox5
{
	DebugPrint("NavPvt");
}

inline void Ubx_DataAnalize	(void)
{
	static U8	tcpIdx = 0;
//	DebugSprintf(tmp, "\t\tFix:%d, Time: %2u:%02u:%02u; date: %2u:%02u:%04u",
//				 GpsFix, UbxDataReceived.hour, UbxDataReceived.min, UbxDataReceived.sec, UbxDataReceived.day, UbxDataReceived.month, UbxDataReceived.year);
//	DebugPrint(tmp);

	if ((GpsFlags.changes_gps) || (GpsFlags.changes_32)											||
//	if ((GpsFlags.changes)																		||	// changed motion direction, speed, etc
		((!GpsFlags.changes_mot) && ((latDiff.u16l > MaxDiff_Lat) || (lonDiff.u16l > MaxDiff_Lon)))	)	// changed location (no motion)
	{
		register U8 packetRemain;
		if ((GpsFlags.tcp_1) && (GpsFlags.changes_32))
		{
			Tcp_SendPacket(tcpIdx);
			GpsFlags.tcp_1 = 0;
			GpsFlags.changes_32 = 0;
//			DebugPrint("Send Previous Packet (%c%ld, %c%ld)");
			DebugSprintf(tmp, "Send Previous Packet (%c%ld, %c%ld)",
						 (Offset.latSign)? '-':'+', latDiff.u32, (Offset.lonSign)? '-':'+', lonDiff.u32);
			DebugPrint(tmp);
		}
		if (!GpsFlags.tcp_1)
		{
			U32	time;
/*			if		(GpsDiff <= diff_8bit)				GpsDataType = TYPE_GPS_DATA;
			else if	(GpsDiff <= diff_12bit)				GpsDataType = TYPE_GPS_DATA_3;
//			else if	(GpsDiff <= diff_16bit)				GpsDataType = TYPE_GPS_DATA_2;
//			else if	(GpsDiff <= diff_17bit)				GpsDataType = TYPE_GPS_DATA_2;
			else										GpsDataType = TYPE_GPS_DATA_3;
*/			GpsDataType = TYPE_GPS_DATA_3;
//			DebugPrint("Start New Tcp Packet");
			tcpIdx = Tcp_StartMemPacket(GpsDataType);
			GpsFlags.tcp_1 = 1;
			GetTcpTime(&time);
			Tcp_WriteMemPacket(tcpIdx, (U8*)&time, 4);
//			DebugSprintf(tmp, "\tTime    saved, packet remain: %3d bytes", tmpSize);	DebugPrint(tmp);
			packetRemain = Tcp_WriteMemPacket(tcpIdx, (U8*)&UbxDataReceived.lon, 8);
//			DebugSprintf(tmp, "\tLat/Lon saved, packet remain: %3d bytes", tmpSize);	DebugPrint(tmp);
			memcpy((char*)&UbxDataStart, (char*)&UbxDataReceived, sizeof(GPS_UBX_DATA));
			DebugSprintf(tmp, "S %2d sec, Fix:%d, Time: %2u:%02u:%02u; date: %2u:%02u:%04u",
						 GpsSec, GpsFix, UbxDataReceived.hour, UbxDataReceived.min, UbxDataReceived.sec, UbxDataReceived.day, UbxDataReceived.month, UbxDataReceived.year);
			DebugPrint(tmp);
			DebugSprintf(tmp, "\t Lat: %ld; Lon %ld ", UbxDataReceived.lat.s32, UbxDataReceived.lon.s32);
			DebugPrint(tmp);
		}
		else
		{
//			if (latDiff.u32 < (0x20000 - 0x10)) 	latDiff.u32 += 0x10;	// rounding (+= 16)
//			latDiff.u32 >>= 5;												// /= 32
			Offset.lat	= latDiff.u32;
//			latDiff.u32 <<= 5;												// *= 32

//			latDiff.u32 >>= 1;												// /= 32
//			latDiff.u16l &= 0xFFF0;
//			Offset.lat = latDiff.u8lh;
//			Offset.lon = latDiff.u8ll;
//			latDiff.u32 <<= 1;												// *= 32

//			if (lonDiff.u32 < (0x40000 - 0x20)) 	latDiff.u32 += 0x20;	// rounding
//			lonDiff.u32 >>= 6;												// /= 64
			Offset.lon	= lonDiff.u32;
//			Offset.lon |= lonDiff.u8lh;
//			Offset.lon2 = lonDiff.u8ll;
//			lonDiff.u32 <<= 6;												// *= 64 
			Offset.sec = GpsSec;
			packetRemain = Tcp_WriteMemPacket(tcpIdx, Offset.bytes, sizeof(GPS_OFFSET_2));
			DebugSprintf(tmp, "  %2d sec,%c%c%02Xx%02Xx%02X, rem: %3db",
						 Offset.sec, (Offset.latSign)? 'S':'_', (Offset.lonSign)? 'S':'_',
						 Offset.bytes[0], Offset.bytes[1], Offset.bytes[2], packetRemain);		DebugPrint(tmp);
			DebugSprintf(tmp, "\t Dif:%c%u,%c%u; Speed:%ldcm/s;Deg:%ld ",
						 (Offset.latSign)? '-':'+', latDiff.u16l, (Offset.lonSign)? '-':'+', lonDiff.u16l,
						 UbxDataReceived.gSpeed.s32, UbxDataReceived.heading.u32);				DebugPrint(tmp);

			if (!Offset.latSign)	UbxDataStart.lat.s32 += latDiff.s32;
			else					UbxDataStart.lat.s32 -= latDiff.s32;
			if (!Offset.lonSign)	UbxDataStart.lon.s32 += lonDiff.s32;
			else					UbxDataStart.lon.s32 -= lonDiff.s32;
//			memcpy((char*)&UbxDataStart, (char*)&UbxDataReceived, sizeof(GPS_UBX_DATA));
		}
//		if (Tcp_WriteMemPacket(tcpIdx, Offset.bytes, sizeof(Offset)) < sizeof(Offset));
//		if ((GpsSec >= OnlineTimeoutSec)	||
		if (((FL_WEB_ONLINE) && (GpsSec >= OnlineTimeoutSec))	||
			(packetRemain < PACKET_MIN_REMAIN)						)
		{
			Tcp_SendPacket(tcpIdx);
			GpsFlags.tcp_1 = 0;
			DebugPrint("Tcp Packet compleete");
		}
		GpsSec			= 0;
		latDiff.u32		= 0;
		lonDiff.u32		= 0;
		GpsFlags.changes_gps	= 0;
		GpsDiff			= diff_null;
	}
}
/*
inline void Ubx_DataAnalize	(void)
{
	static U8	tcpIdx = 0;
//	DebugSprintf(tmp, "\t\tFix:%d, Time: %2u:%02u:%02u; date: %2u:%02u:%04u",
//				 GpsFix, UbxDataReceived.hour, UbxDataReceived.min, UbxDataReceived.sec, UbxDataReceived.day, UbxDataReceived.month, UbxDataReceived.year);
//	DebugPrint(tmp);

	if ((GpsFlags.changes_gps)																		||	// changed motion direction, speed, etc
		((!GpsFlags.changes_mot) && ((latDiff.u16l > MaxDiff_Lat) || (lonDiff.u16l > MaxDiff_Lon)))	)	// changed location (no motion)
	{
		register U8 packetRemain;
		if ((GpsFlags.tcp_1) && (GpsFlags.changes_32))
		{
			Tcp_SendPacket(tcpIdx);
			GpsFlags.tcp_1 = 0;
			GpsFlags.changes_32 = 0;
//			DebugPrint("Send Previous Packet (%c%ld, %c%ld)");
			DebugSprintf(tmp, "Send Previous Packet (%c%ld, %c%ld)",
						 (Offset.latSign)? '-':'+', latDiff.u32, (Offset.lonSign)? '-':'+', lonDiff.u32);
			DebugPrint(tmp);
		}
		if (!GpsFlags.tcp_1)
		{
			U32	time;
//			DebugPrint("Start New Tcp Packet");
			tcpIdx = Tcp_StartMemPacket(TYPE_GPS_DATA);
			GpsFlags.tcp_1 = 1;
			GetTcpTime(&time);
			Tcp_WriteMemPacket(tcpIdx, (U8*)&time, 4);
//			DebugSprintf(tmp, "\tTime    saved, packet remain: %3d bytes", tmpSize);	DebugPrint(tmp);
			packetRemain = Tcp_WriteMemPacket(tcpIdx, (U8*)&UbxDataReceived.lon, 8);
//			DebugSprintf(tmp, "\tLat/Lon saved, packet remain: %3d bytes", tmpSize);	DebugPrint(tmp);
			memcpy((char*)&UbxDataStart, (char*)&UbxDataReceived, sizeof(GPS_UBX_DATA));
			DebugSprintf(tmp, "S %2d sec, Fix:%d, Time: %2u:%02u:%02u; date: %2u:%02u:%04u",
						 GpsSec, GpsFix, UbxDataReceived.hour, UbxDataReceived.min, UbxDataReceived.sec, UbxDataReceived.day, UbxDataReceived.month, UbxDataReceived.year);
			DebugPrint(tmp);
			DebugSprintf(tmp, "\t Lat: %ld; Lon %ld ", UbxDataReceived.lat.s32, UbxDataReceived.lon.s32);
			DebugPrint(tmp);
		}
		else
		{
			if (latDiff.u16l < (0x2000 - 0x10)) 	latDiff.u16l += 0x10;	// rounding
			latDiff.u16l >>= 5;												// /= 32
			Offset.lat = latDiff.u8ll;
			latDiff.u16l <<= 5;												// *= 32
			if (lonDiff.u16l < (0x4000 - 0x20)) 	latDiff.u16l += 0x20;	// rounding
			lonDiff.u16l >>= 6;												// /= 64
			Offset.lon = lonDiff.u8ll;
			lonDiff.u16l <<= 6;												// *= 64 
			Offset.sec = GpsSec;
			packetRemain = Tcp_WriteMemPacket(tcpIdx, Offset.bytes, sizeof(GPS_OFFSET));
			DebugSprintf(tmp, "  %2d sec,%c%c%02Xx%02Xx%02X, rem: %3db",
						 Offset.sec, (Offset.latSign)? 'S':'_', (Offset.lonSign)? 'S':'_',
						 Offset.bytes[0], Offset.bytes[1], Offset.bytes[2], packetRemain);		DebugPrint(tmp);
			DebugSprintf(tmp, "\t Dif:%c%u,%c%u; Speed:%ldcm/s;Deg:%ld ",
						 (Offset.latSign)? '-':'+', latDiff.u16l, (Offset.lonSign)? '-':'+', lonDiff.u16l,
						 UbxDataReceived.gSpeed, UbxDataReceived.heading.u32);					DebugPrint(tmp);
			if (!Offset.latSign)	UbxDataStart.lat.s32 += latDiff.s32;
			else					UbxDataStart.lat.s32 -= latDiff.s32;
			if (!Offset.lonSign)	UbxDataStart.lon.s32 += lonDiff.s32;
			else					UbxDataStart.lon.s32 -= lonDiff.s32;
//			memcpy((char*)&UbxDataStart, (char*)&UbxDataReceived, sizeof(GPS_UBX_DATA));
		}
//		if (Tcp_WriteMemPacket(tcpIdx, Offset.bytes, sizeof(Offset)) < sizeof(Offset));
//		if ((GpsSec >= OnlineTimeoutSec)	||
		if (((FL_WEB_ONLINE) && (GpsSec >= OnlineTimeoutSec))	||
			(packetRemain < PACKET_MIN_REMAIN)						)
		{
			Tcp_SendPacket(tcpIdx);
			GpsFlags.tcp_1 = 0;
			DebugPrint("Tcp Packet compleete");
		}
		GpsSec			= 0;
		latDiff.u32		= 0;
		lonDiff.u32		= 0;
		GpsFlags.changes_gps	= 0;
	}
}

*		TARGET_DATA		Targets[TARGET_NUMBER];

static	volatile U8		TxBuff[256];
static	volatile U8		TxWrite, TxSent;


const SECTION NmeaSections[] = 
{	// $GPRMC,161241.00,A,5659.84653,N,02416.39666,E,0.028,,200212,,,D*73 ________________
	{START_EMPTY,		0},				// 									0	SECTION_WAIT
	{START_HEDER,		LEN_HEDER},		// 									1	SECTION_HEDER
	{START_TIME,		LEN_TIME},		// 									2	GPRMC_START
	{START_STATUS,		LEN_STATUS},	// 									3
	{START_LATITUDE,	LEN_LATITUDE},	// 									4
	{START_N_S,			LEN_N_S},		// 									5
	{START_LONGITUDE,	LEN_LONGITUDE},	// 									6
	{START_E_W,			LEN_E_W},		// 									7
	{START_KNOTS,		0},				// 									8
	{START_DEGREES,		0},				// 									9
	{START_DATE,		LEN_DATE},		// 									10
	{START_EMPTY,		0},				// 									11
	{START_EMPTY,		0},				// 									12
	{START_EMPTY,		0},				// 									13
	{START_EMPTY,		0},				// 									14
	// $GPVTG, 309.62, T,,M,0.13,N,0.2,K*6E ______________________________________________
	{START_DEGREES,		LEN_DEGREES},	// 									15	GPVTG_START
	{START_EMPTY,		0},				// 									16
	{START_EMPTY,		0},				// 									17
	{START_EMPTY,		0},				// 									18
	{START_KNOTS,		LEN_KNOTS},		// 									19
	{START_EMPTY,		0},				// 									20
	{START_KM,			LEN_KM},		// 									21
	{START_EMPTY,		0},				// 									22
	{START_EMPTY,		0},				// 									23
	// $GPGGA,161241.00,5659.84653,N,02416.39666,E,2,05,2.00,20.9,M,22.7,M,,*6B ___________
	{START_EMPTY,		0},				// Time when Fix taken (UTC)		24	GPGGA_START
	{START_EMPTY,		0},				// Latitude
	{START_EMPTY,		0},				// N/S
	{START_EMPTY,		0},				// Longitude
	{START_EMPTY,		0},				// E/W
	{START_EMPTY,		0},				// Fix quality:	0 = invalid
										//				1 = GPS fix (SPS)
										//				2 = DGPS fix
										//				3 = PPS fix
										//				4 = Real Time Kinematic
										//				5 = Float RTK
										//				6 = estimated (dead reckoning) (2.3 feature)
										//				7 = Manual input mode
										//				8 = Simulation mode
	{START_SAT_TRACK,	LEN_SAT_TRACK},	// Number of satellites being tracked
	{START_EMPTY,		0},				// Horizontal dilution of position
	{START_ALTITUDE,	LEN_ALTITUDE},	// Altitude, Meters, above mean sea level
	{START_EMPTY,		0},				// (M) Meters, above mean sea level
	{START_EMPTY,		0},				// Height of geoid (mean sea level) above WGS84 ellipsoid
	{START_EMPTY,		0},				// (M) Meters, above WGS84 ellipsoid
	{START_EMPTY,		0},				// (empty field) time in seconds since last DGPS update
	{START_EMPTY,		0},				// (empty field) DGPS station ID number
	{START_EMPTY,		0},				// the checksum data, always begins with *
};

static	volatile U8			Section, sectionLen, NmeaCharNr, controlSum, controlSumReceived;
static	volatile GPS_DATA_RECEIVED	NmeaData;
static	LAT_LON_VALUE		CurrentLatVal;
static	LAT_LON_VALUE		CurrentLonVal;
static	volatile FLAGS		GpsFlags;
static	volatile FLAGS		GpsSigns;
				GPS_DATA	GpsNmeaData;
				U16			GpsDirection;

#define	GGA_STRING			GpsFlags.flag_0
#define	GSA_STRING			GpsFlags.flag_1
#define	GSV_STRING			GpsFlags.flag_2
#define	GLL_STRING			GpsFlags.flag_3
#define	RMC_STRING			GpsFlags.flag_4
#define	VTG_STRING			GpsFlags.flag_5

#define	GPS_STATUS			GpsFlags.flag_8
#define	GPS_N_S				GpsFlags.flag_9
#define	GPS_E_W				GpsFlags.flag_10
#define	GPS_DATA_OK			GpsFlags.flag_11

#define	LAT_LON_SIGNS		GpsSigns.word
#define	LAT_SIGN			GpsSigns.flag_0
#define	LON_SIGN			GpsSigns.flag_1
#define	LAT_HI_SIGN			GpsSigns.flag_2

#define	GpsLed				LedStatus3

#define	GPS_STATUS_OFF		0b0000000000000000
#define	GPS_STATUS_ON		0b1111111111111111
#define	GPS_STATUS_1		0b1000000000000000
#define	GPS_STATUS_2		0b1001000000000000
#define	GPS_STATUS_3		0b1001001000000000
#define	GPS_STATUS_4		0b1001001001000000
#define	GPS_STATUS_ERROR	0b1010101010101010


// **********************************************************************
//	Tx1 INTERRUPT
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt( void )
{
	_U1TXIF		= 0;				// Clear the Transmit Interrupt Flag
	if (TxWrite	!= TxSent)			//	if remain bytes in buffer
		U1TXREG	= TxBuff[TxSent++];	//	Transmit next byte
	else
		_U1TXIE		= 0;			// Disable Transmit Interrupt
}

// **********************************************************************
//	Rx1 INTERRUPT
// ......................................................................
inline void	GpsSectionWaitIrq(U8 rxData);
inline void	GpsSectionUbxIdIrq(U8 rxData);
inline void	GpsSectionCrcIrq(U8 rxData);
inline void	GpsDataSaveIrq(U8 rxData);
inline U8	GpsHederSortIrq(void);
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void )
{
	while (U1STAbits.URXDA)			// at least one Receive Buffer Data Available
	{
		U8	rxData	= U1RXREG;		// read from uart1 Rx buffer
		_U1RXIF		= 0;			// Clear the Recieve Interrupt Flag

		if (SETUP_GPS)
			Uart3SendByte(rxData);

		if (Section == SECTION_WAIT)
			GpsSectionWaitIrq(rxData);
		else if (Section == SECTION_UBX_ID)
			GpsSectionUbxIdIrq(rxData);
		else if (rxData == '*')		// Start of string ControlSum receive
		{
			NmeaData.bytes[NmeaCharNr] = 0;
			Section = SECTION_CRC;
		}
		else if (Section == SECTION_CRC)
			GpsSectionCrcIrq(rxData);
		else
			GpsDataSaveIrq(rxData);
	}
}

inline void GpsSectionWaitIrq(U8 rxData)
{
	if (rxData == '$')					// Start of NMEA string
	{
		Section		= SECTION_HEDER;
		NmeaCharNr	= START_HEDER;
		sectionLen	= LEN_HEDER;
		controlSum	= controlSumReceived = 0;
	}
	else if (rxData == UBX_SYNC_CHAR_1)	// UBX Sync Char 1
	{
		Section		= SECTION_UBX_ID;
		NmeaCharNr	= SECTION_UBX_ID;
//			sectionLen	= LEN_HEDER;
		controlSum	= controlSumReceived = 0;
	}
}

inline void	GpsSectionUbxIdIrq(U8 rxData)
{
	if (NmeaCharNr == SECTION_UBX_ID)
	{
		if (rxData == UBX_SYNC_CHAR_2)		// UBX Sync Char 2
			NmeaCharNr	= START_HEDER;
		else
			Section = SECTION_WAIT;
	}
	else if (NmeaCharNr	< (START_HEDER + UBX_HEDER_SIZE))
	{
		NmeaData.bytes[NmeaCharNr++] = rxData;
		controlSum += rxData;
		controlSumReceived += controlSum;
	}
	else
	{	//TODO
		Section = SECTION_WAIT;
	}
}

inline void	GpsSectionCrcIrq(U8 rxData)
{
	if (rxData == LF)					// check ControlSum
	{
		Section	= sectionLen = NmeaCharNr = SECTION_WAIT;

		if (controlSum != controlSumReceived)
		{
			NmeaData.gps.status[0] = '!';	// wrong checksum
			GpsLed = GPS_STATUS_ERROR;
			GpsFlags.word = 0;
		}
		else if (GGA_STRING && VTG_STRING && RMC_STRING)
		{
			GGA_STRING = 0;
			VTG_STRING = 0;
			RMC_STRING = 0;
*			Gsm_SendData((char*)(NmeaData.gps.Time.str));
			LcdString(0, 0, (char*)(NmeaData.gps.Time.str));
			LcdString(1, 0, (char*)(NmeaData.gps.Date.str));
			LcdString(2, 0, (char*)(NmeaData.gps.status));
			LcdString(2, 12, (char*)(NmeaData.gps.e_w));
			LcdString(2, 24, (char*)(NmeaData.gps.n_s));
			LcdString(3, 0, (char*)(NmeaData.gps.satTrack));
//			LcdString(4, 0, (char*)(NmeaData.gps.degrees));
			LcdString(4, 0, (char*)(NmeaData.gps.altitude));
			LcdString(5, 0, (char*)(NmeaData.gps.latitude));
			LcdString(6, 0, (char*)(NmeaData.gps.longitude));
			LcdString(7, 0, (char*)(NmeaData.gps.km));
*
//			LcdData(0, 0, (char*)(NmeaData.gps.Time.str), 6);
//			LcdData(1, 0, (char*)(NmeaData.gps.Date.str), 6);
//			LcdData(2, 0, (char*)(NmeaData.gps.status), 1);
//			LcdData(2, 12, (char*)(NmeaData.gps.e_w), 1);
//			LcdData(2, 24, (char*)(NmeaData.gps.n_s), 1);
//			LcdData(3, 0, (char*)(NmeaData.gps.satTrack), 2);
//			LcdData(4, 0, (char*)(NmeaData.gps.degrees), 7);
//			LcdData(5, 0, (char*)(NmeaData.gps.latitude), 10);
//			LcdData(6, 0, (char*)(NmeaData.gps.longitude), 11);
//			LcdData(7, 0, (char*)(NmeaData.gps.km), 7);
			if (NmeaData.gps.status[0] == 'A')
			{
				GPS_DATA_OK = 1;
				GpsLed = GPS_STATUS_ON;
				memcpy((char*)GpsNmeaData.bytes, (const void*)NmeaData.gps.bytes, LEN_GPS);
				GPS_IF = 1;		// turn On interrupt for new GPS data processing
//				GPS_NEW_DATA = 1;
			}
			else
			{
				GpsLed                  = GPS_STATUS_ERROR;
				GpsNmeaData.status[0]	= NmeaData.gps.status[0];
				GpsDirection			= 0xFFFF;
			}
		}
	}
	else if (rxData != CR)				// receive ControlSum
	{
		controlSumReceived <<= 4;
		controlSumReceived += (rxData <= '9')? (rxData - '0'):(rxData - ('A' - 0x0A));
	}
}

inline void	GpsDataSaveIrq(U8 rxData)
{
	controlSum ^= rxData;				// calculate ControlSum
	if (rxData == ',')
	{
		NmeaData.bytes[NmeaCharNr] = 0;
		if (Section == SECTION_HEDER)
			Section = GpsHederSortIrq();
		else if (Section)
			Section++;


		NmeaCharNr	= NmeaSections[Section].start;
		sectionLen	= NmeaSections[Section].len;
	}
	else if (sectionLen)
	{
		NmeaData.bytes[NmeaCharNr++] = rxData;
		sectionLen--;
//				if ((Section == SECTION_STATUS) &&
//					(NmeaData.gps.Date.moonOnes[0] > '0') )
//					// Copy GPS Clock to System Clock
//					memcpy(DateTime.str, (const void*)NmeaData.gps.Date.str, (LEN_DATE + LEN_TIME - 1));
	}
}

inline U8 GpsHederSortIrq(void)
{
	if		((NmeaData.heder[2] == 'V') &&
			 (NmeaData.heder[3] == 'T') &&
			 (NmeaData.heder[4] == 'G') )
	{
		VTG_STRING = 1;
		return SECTION_GPVTG;
	}
	else if ((NmeaData.heder[2] == 'G') &&
			 (NmeaData.heder[3] == 'G') &&
			 (NmeaData.heder[4] == 'A') )
	{
		GGA_STRING = 1;
		return SECTION_GPGGA;
//		Uart1SendText("$PUBX,40,GGA,0,0,0,0,0,0*5A\r\n");	// switch GGA protocol OFF
	}
	else if ((NmeaData.heder[2] == 'R') &&
			 (NmeaData.heder[3] == 'M') &&
			 (NmeaData.heder[4] == 'C') )
	{
		RMC_STRING = 1;
		return SECTION_GPRMC;
	}
	else if ((NmeaData.heder[2] == 'G') &&
			 (NmeaData.heder[3] == 'S') &&
			 (NmeaData.heder[4] == 'A') )
		Uart1SendText("$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n");	// switch GSA protocol OFF
	else if ((NmeaData.heder[2] == 'G') &&
			 (NmeaData.heder[3] == 'S') &&
			 (NmeaData.heder[4] == 'V') )
		Uart1SendText("$PUBX,40,GSV,0,0,0,0,0,0*59\r\n");	// switch GSV protocol OFF
	else if ((NmeaData.heder[2] == 'G') &&
			 (NmeaData.heder[3] == 'L') &&
			 (NmeaData.heder[4] == 'L') )
		Uart1SendText("$$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n");	// switch GLL protocol OFF
	else if ((NmeaData.heder[2] == 'Z') &&
			 (NmeaData.heder[3] == 'D') &&
			 (NmeaData.heder[4] == 'A') )
		Uart1SendText("$PUBX,40,ZDA,0,0,0,0,0,0*44\r\n");	// switch ZDA protocol OFF

	return SECTION_WAIT;
}


// Lat	 56|59.84787 degrees = 0:90
// Lon	024|16.39522 degrees = 0:180
//	   degr|minutes
LAT_LON_VALUE GpsValue (char* str)
{
	LAT_LON_VALUE val;
	U16 i = 0;
	memset(val.bytes, 0, sizeof(LAT_LON_VALUE));
	
	while (str[i] != '.')
	{
		if(!str[i++])
			return val;
	}
	i -= 2;
	while (i)
	{
		val.degrees *= 10;
		val.degrees += (*str - '0');
		str++;
		i--;
	}
	i = 5;
	while (i)
	{
		if (*str != '.')
		{
			val.minutes *= 10;
			val.minutes += (*str - '0');
			i--;
		}
		str++;
	}
	if (*str > '4')
		val.minutes++;

	return val;
}

inline U16 GpsDirectionValue (char* str)
{	// 0.028
	if (!*str)
		return 0xFFFF;

	U16	ret = 0;
	while(*str)
	{
		if (*str == '.')
		{
			str++;
			ret += (*str++ - '0');
			break;
		}
		else
		{
			ret += (*str++ - '0');
			ret *= 10;
		}
	}
	if (*str > '4')
		ret++;
    
	return ret;

}

LAT_LON_VALUE Difference (LAT_LON_VALUE* home, LAT_LON_VALUE* target)
{
	LAT_LON_VALUE diff;

	if(target->degrees >= home->degrees)
	{
		diff.degrees =	target->degrees	- home->degrees;
		diff.sign = 0;
	}
	else
	{
		diff.degrees =	home->degrees - target->degrees;
		diff.sign = 1;
	}

	if(!diff.sign)
	{
		if(target->minutes >= home->minutes)
			diff.minutes =	target->minutes	- home->minutes;
		else
		{
			diff.minutes = home->minutes - target->minutes;
			if (diff.degrees)
			{
				diff.degrees--;
				diff.minutes = GPS_VAL_MINUTES_IN_DEGREE - diff.minutes;
			}
			else
				diff.sign = 1;
		}
	}
	else
	{
		if (target->minutes <= home->minutes)
			diff.minutes = home->minutes - target->minutes;
		else
		{
			diff.degrees--;
			diff.minutes = GPS_VAL_MINUTES_IN_DEGREE - (target->minutes - home->minutes);
		}
	}




*
	if(target->minutes < home->minutes)
	{
		diff.minutes = GPS_VAL_MINUTES_IN_DEGREE - (home->minutes - target->minutes);
		if (diff.degrees > 0)
			diff.degrees--;
		if(!diff.degrees)
			diff.sign = 1;
	}
	else
		diff.minutes =	target->minutes	- home->minutes;
*
*
10.3 - 12.4 = -2.1
10.3 - 11.4 = -1.1
10.3 - 10.4 = -0.1
10.3 - 9.4  =  0.9
 7 - 8
17 - 8 = 9
10 - 7 = 3
8 - 7 = -1

10 - (8 - 7) = 1
*

	return diff;
}

LAT_LON_VALUE Hypotenuse(LAT_LON_VALUE* lat, LAT_LON_VALUE* lon)
{
	U8				shift = 0;
	U32				a, b, c;
	LAT_LON_VALUE	rc;

	rc.km		= 0;
	rc.meters	= 0;
	
	if (!lat->km && !lon->km && !lat->meters && !lon->meters)
		return rc;


	if((lat->km < 45) && (lon->km < 45))			// 12.345 km
	{
		a = (lat->km * 1000) + lat->meters; 
		b = (lon->km * 1000) + lon->meters;
	}
	else if((lat->km < 450) && (lon->km < 450))		// 123.45 km
	{
		shift = 1;
		a = (lat->km * 100) + (lat->meters / 10); 
		b = (lon->km * 100) + (lon->meters / 10);
		if ((lat->meters % 10) > 4)		a++;
		if ((lon->meters % 10) > 4)		b++;
	}
	else if((lat->km < 4500) && (lon->km < 4500))	// 1234.5 km
	{
		shift = 2;
		a = (lat->km * 10) + (lat->meters / 100); 
		b = (lon->km * 10) + (lon->meters / 100);
		if ((lat->meters % 100) > 44)		a++;
		if ((lon->meters % 100) > 44)		b++;
	}
	else //  lat->km => 4500 || lon->km >= 4500		// 12345 km
	{
		shift = 3;
		a = ((lat->meters % 1000) > 444)?	lat->km : (lat->km + 1); 
		b = ((lon->meters % 1000) > 444)?	lon->km : (lon->km + 1); 
	}

	if (LAT_HI_SIGN)	c = a;				// multiplier = biggest of cathetus		(a > b)
	else				c = b;				//										(a < b)
		

	a = (a * a) + (b * b);						// sum of cathetus quadratic
	while ((c * c) < a)		c++;				// square root
	c--;										// calculation

	if		(shift == 0)
	{
		rc.km		= c / 1000;
		rc.meters	= c % 1000;
	}
	else if (shift == 1)
	{
		rc.km		=  c / 100;
		rc.meters	= (c % 100) * 10;
	}
	else if (shift == 2)
	{
		rc.km		=  c / 10;
		rc.meters	= (c % 10) * 100;
	}
	else // (shift == 3)
		rc.km		= (U16)c;	// rc.meters = 0;

	return rc;
}

// **********************************************************************
//	GPS_EXT Interrupt (I2C1 Slave Event used)
// **********************************************************************
void GPS_INTERRUPT	( void )
{
	GPS_IF	= 0;			// Clear Interrupt Flag

	CurrentLatVal	= GpsValue(GpsNmeaData.latitude);
	CurrentLonVal	= GpsValue(GpsNmeaData.longitude);
    GpsDirection	= GpsDirectionValue(GpsNmeaData.degrees);
	U8 targetNr;
	for(targetNr = 0; targetNr < TARGET_NUMBER; targetNr++)
	{
		if (!Targets[targetNr].active)	continue;

//		Targets[targetNr].active = 0;														// deactivate current target to avoid incorrect data
		LAT_LON_VALUE	latDist	= Difference(&CurrentLatVal, &Targets[targetNr].latVal);
		LAT_LON_VALUE	lonDist	= Difference(&CurrentLonVal, &Targets[targetNr].lonVal);
		LAT_LON_SIGNS	= 0;
		LAT_SIGN		= latDist.sign;
		LON_SIGN		= lonDist.sign;

		U32	a32 = latDist.minutes;
		U32	b32 = lonDist.minutes;
		if (latDist.degrees)		a32 += (U32)(latDist.degrees * GPS_VAL_MINUTES_IN_DEGREE);
		if (lonDist.degrees)		b32 += (U32)(lonDist.degrees * GPS_VAL_MINUTES_IN_DEGREE);
		U16 c16;
		if (a32 > b32)
		{
			c16 = (U16)((b32 * 450) / a32);						// max degrees variation = 45'
			LAT_HI_SIGN = 1;
		}
		else
			c16 = (U16)((a32 * 450) / b32);

		switch (LAT_LON_SIGNS)			// Lat < Lon,	LonNeg, LatNeg
		{
		case 0:	Targets[targetNr].degrees =	( 900	-	c16);	break;		// Lat < Lon; +Lon; +Lat
		case 1:	Targets[targetNr].degrees =	( 900	+	c16);	break;		// Lat < Lon; +Lon; -Lat
		case 2:	Targets[targetNr].degrees =	(2700	+	c16);	break;		// Lat < Lon; -Lon; +Lat
		case 3:	Targets[targetNr].degrees =	(2700	-	c16);	break;		// Lat < Lon; -Lon; -Lat
		case 4:	Targets[targetNr].degrees =				c16;	break;		// Lat > Lon; +Lon; +Lat
		case 5:	Targets[targetNr].degrees =	(1800	-	c16);	break;		// Lat > Lon; +Lon; -Lat
		case 6:	Targets[targetNr].degrees =	(3600	-	c16);	break;		// Lat > Lon; -Lon; +Lat
		case 7:	Targets[targetNr].degrees =	(1800	+	c16);	break;		// Lat > Lon; -Lon; -Lat
		}

	//	{111, 32, 186},		// Lat: 1degree = ~111.32km		1min = 1855.33m
	//	{63, 75, 106}		// Lon: 1degree = ~63.75km		1min = 1062.5m
		a32							= ((U32)(latDist.minutes) * 18553) / 10000;		// 1min = 1855.33m (we have 1min x 10000)
		c16							= latDist.degrees * 320;
		latDist.km					= ((c16 / 1000) + (latDist.degrees * 111));
		latDist.meters				=  (c16 % 1000);
		latDist.km					+= (U16)(a32 / 1000);
		latDist.meters				+= (U16)(a32 % 1000);

		a32							= ((U32)(lonDist.minutes) * 10625) / 10000;
		c16							= lonDist.degrees * 750;
		lonDist.km					= ((c16 / 1000) + (lonDist.degrees * 63));
		lonDist.meters				=  (c16 % 1000);
		lonDist.km					+= (U16)(a32 / 1000);
		lonDist.meters				+= (U16)(a32 % 1000);

		Targets[targetNr].distance	= Hypotenuse(&latDist, &lonDist);

//		Targets[targetNr].active = 1;														// activate current target with correct data
	}
}
*/

/*
GPS_DATA GpsDelta(char* str, U8 lat_lon)
{
	U8 i = 0;
	U16	km, m, a;
	GPS_DATA	data;

	data.sign = 0;
	data.degrees = 0;
	data.minutes = 0;

	while (str[i] != '.')
		i++;
	i -= 2;

	while (i)
	{
		data.degrees *= 10;
		data.degrees += (*str - '0');
		str++;
		i--;
	}

	if (data.degrees < Home[lat_lon].degrees)
	{
		data.sign = 1;
		data.degrees = Home[lat_lon].degrees - data.degrees;
	}
	else
		data.degrees = data.degrees - Home[lat_lon].degrees;
	
	i = 4;
	while (i)
	{
		if (*str != '.')
		{
			data.minutes *= 10;
			data.minutes += (*str - '0');
			i--;
		}
		str++;
	}
	if (*str > '4')
		data.minutes++;
	
	if (!data.sign)
	{
		if (data.minutes >= Home[lat_lon].minutes)
			data.minutes = data.minutes - Home[lat_lon].minutes;
		else
		{
			if (data.degrees)
			{
				data.degrees--;
				data.minutes = (data.minutes + 6000) - Home[lat_lon].minutes;
			}
			else
			{
				data.sign = 1;
				data.minutes = Home[lat_lon].minutes - data.minutes;
			}
		}
	}
	else
	{
		if (data.minutes <= Home[lat_lon].minutes)
			data.minutes = Home[lat_lon].minutes - data.minutes;
		else
		{
			data.degrees--;
			data.minutes = (Home[lat_lon].minutes + 6000) - data.minutes;
		}
	}
	DegreesToKm(&data, lat_lon);

	return data;
}
*/
