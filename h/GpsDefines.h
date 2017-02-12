#ifndef	_gpsDefines_h_
#define	_gpsDefines_h_

#include	"Typedef.h"
#include	"Timers.h"

/*
#define	FL_UBX_REC				GpsFlags.flag_0
#define	FL_GPS_CHANGES			GpsFlags.flag_1
#define	FL_CHANGES_32			GpsFlags.flag_2
#define	FL_TCP_1				GpsFlags.flag_3
#define	FL_GPS_NEW				GpsFlags.flag_4

#define	FL_GPS_ACCEL			GpsFlags.flag_5
#define	FL_GPS_MOTION			GpsFlags.flag_6
#define	FL_GPS_DIRECTION		GpsFlags.flag_7
*/
#define	RX_BUFF_SIZE			256
#define	RX_BUFF_COUNT			5
#define	MEASURE_RATE_HZ			1
#define	GPS_TIME_FIX			2
#define	GPS_3D_FIX				3

#define	UBX_SYNC_CHAR_1			0xB5
#define	UBX_SYNC_CHAR_2			0x62
#define	UBX_SYNC_INT			0x62B5
#define	SEND_UBX_STRING(X)		Gps_SendUbx(X, (sizeof(X) - 2));
void	 Gps_SendUbx			(const U8* data, U16 len);

#define	UBX_CLASS_NAV			0x01	// Navigation Results: Position, Speed, Time, Acc, Heading, DOP, SVs used
#define	UBX_CLASS_RXM			0x02	// Receiver Manager Messages: Satellite Status, RTC Status
#define	UBX_CLASS_INF			0x04	// Information Messages: Printf-Style Messages, with IDs such as Error, Warning, Notice
#define	UBX_CLASS_ACK			0x05	// Ack/Nack Messages: as replies to CFG Input Messages
#define	UBX_CLASS_CFG			0x06	// Configuration Input Messages: Set Dynamic Model, Set DOP Mask, Set Baud Rate, etc.
#define	UBX_CLASS_MON			0x0A	// Monitoring Messages: Comunication Status, CPU Load, Stack Usage, Task Status
#define	UBX_CLASS_AID			0x0B	// AssistNow Aiding Messages: Ephemeris, Almanac, other A-GPS data input
#define	UBX_CLASS_TIM			0x0D	// Timing Messages: Time Pulse Output, Timemark Results
#define	UBX_CLASS_LOG			0x21	// Logging

#define	NMEA_CLASS				0xF0	// NMEA Standard Messages

#define	NMEA_ID_DTM				0x0AF0	// Datum Reference
#define	NMEA_ID_GBS				0x09F0	// GNSS Satellite Fault Detection
#define	NMEA_ID_GGA				0x00F0	// Global positioning system fix data
#define	NMEA_ID_GLL				0x01F0	// Latitude and longitude, with time of position fix and status
#define	NMEA_ID_GLQ				0x43F0	// Poll a standard message (if the current Talker ID is GL)
#define	NMEA_ID_GNQ				0x42F0	// Poll a standard message (if the current Talker ID is GN)
#define	NMEA_ID_GNS				0x0DF0	// GNSS fix data
#define	NMEA_ID_GPQ				0x40F0	// Poll a standard message (if the current Talker ID is GP)
#define	NMEA_ID_GRS				0x06F0	// GNSS Range Residuals
#define	NMEA_ID_GSA				0x02F0	// GNSS DOP and Active Satellites
#define	NMEA_ID_GST				0x07F0	// GNSS Pseudo Range Error Statistics
#define	NMEA_ID_GSV				0x03F0	// GNSS Satellites in View
#define	NMEA_ID_RMC				0x04F0	// Recommended Minimum data
#define	NMEA_ID_TXT				0x41F0	// Text Transmission
#define	NMEA_ID_VTG				0x05F0	// Course over ground and Ground speed
#define	NMEA_ID_ZDA				0x08F0	// Time and Date
//--------------------------------------------------------------------------------------------------------------------------------
#define	UBX_ID_NAV_AOPSTATUS	0x6001	//   Periodic/Polled		AssistNow Autonomous Status
#define	UBX_ID_NAV_CLOCK		0x2201	// 5 Periodic/Polled		Clock Solution
#define	UBX_ID_NAV_DGPS			0x3101	//   Periodic/Polled		DGPS Data Used for NAV
#define	UBX_ID_NAV_DOP			0x0401	// 5 Periodic/Polled		Dilution of precision
#define	UBX_ID_NAV_POSECEF		0x0101	// 5 Periodic/Polled		Position Solution in ECEF
#define	UBX_ID_NAV_POSLLH		0x0201	// 5 Periodic/Polled		Geodetic Position Solution
#define	UBX_ID_NAV_PVT			0x0701	//   Periodic/Polled		Navigation Position Velocity Time Solution
#define	UBX_ID_NAV_SBAS			0x3201	// 5 Periodic/Polled		SBAS Status Data
#define	UBX_ID_NAV_SOL			0x0601	// 5 Periodic/Polled		Navigation Solution Information
#define	UBX_ID_NAV_STATUS		0x0301	// 5 Periodic/Polled		Receiver Navigation Status
#define	UBX_ID_NAV_SVINFO		0x3001	// 5 Periodic/Polled		Space Vehicle Information
#define	UBX_ID_NAV_TIMEGPS		0x2001	// 5 Periodic/Polled		GPS Time Solution
#define	UBX_ID_NAV_TIMEUTC		0x2101	// 5 Periodic/Polled		UTC Time Solution
#define	UBX_ID_NAV_VELECEF		0x1101	// 5 Periodic/Polled		Velocity Solution in ECEF
#define	UBX_ID_NAV_VELNED		0x1201	// 5 Periodic/Polled		Velocity Solution in NED
#define	UBX_ID_RXM_RAW			0x1002	//   Periodic/Polled		Raw Measurement Data
#define	UBX_ID_RXM_SVSI			0x2002	//   Periodic/Polled		SV Status Info
#define	UBX_ID_CFG_PRT			0x0006	// Set/Poll					Set/Polls the configuration of the All/One/Used I/O Port
#define	UBX_ID_CFG_MSG			0x0106	// Set/Poll					Set/Poll a message Rate (configuration)
#define	UBX_ID_CFG_RATE			0x0806	// Set/Poll					Set/Poll Navigation/Measurement Rate Settings
#define	UBX_ID_CFG_NAVX5		0x2306	// Set/Get/Poll				Navigation Engine Expert Settings

typedef union
{
	U16 word;
	struct
	{
		U16 changes_32	:1;		// FL_CHANGES_32
		U16 changes_gps	:1;		// FL_GPS_CHANGES
		U16 changes_acc	:1;		// FL_GPS_ACCEL
		U16 changes_mot	:1;		// FL_GPS_MOTION
		U16 changes_dir	:1;		// FL_GPS_DIRECTION
		U16 flag_5		:1;
		U16 flag_6		:1;
		U16 flag_7		:1;
		U16 ubx_rec		:1;		// FL_UBX_REC
		U16 tcp_1		:1;		// FL_TCP_1
		U16 flag_10		:1;		// 
		U16 flag_11		:1;		// 
		U16 flag_12		:1;		// 
		U16 flag_13		:1;		// 
		U16 flag_14		:1;		// 
		U16 flag_15		:1;		// 
	};
	struct
	{
		U16	changes		:2;		// FL_CHANGES_32 + FL_GPS_CHANGES
		U16 und_7		:6;
		U16 und_8		:8;
	};
} GPS_FLAGS;

typedef union	tagGPS_MSG
{
	U8	bytes[512];
	struct
	{
		union
		{
			struct
			{
				U8	ubxSync1;
				U8	ubxSync2;
			};
			U16		ubxSync16;
		};
		union
		{
			struct
			{
				U8	ubxClass;
				U8	ubxID;
			};
			U16		ubxClassId;
		};
		U16			ubxLen;
		U8			ubxData[(512 - 6)];
	};
	struct
	{
		U8			NMEA_START;
		U8			NMEA_ID[2];
		U8			NMEA_MSG[3];
		U8			nmeaData[(512 - 6)];
	};
} GPS_MSG;
/*
#include	"Adc.h"

#define	TARGET_NUMBER		4
#define	TARGET_MAX_TIMEOUT	300		// 300sec = 5min
#define	GPS_BAUDRATE		38400	// 0: 9600, 1: 19200, 2: 38400, 3: 115200
#define	UBX_BUFF_ADDRESS	0x2000	// Address in Large memmory (for C & asm use)
#define	UBX_BUFF_SIZE		0x1000
#define	UBX_DATA_MIN_SIZE	512		// 
#define	UBX_LAST_POINTER	((UBX_BUFF_ADDRESS + UBX_BUFF_SIZE) - UBX_DATA_MIN_SIZE)

#define	MAX_DEGREE			3600				//	360' x10
#define	GPS_VAL_MINUTES_IN_DEGREE	60000
#define	METERS_IN_KM		1000
#define	LAT_DEGREE_KM		111
#define	LAT_DEGREE_M		320
#define	LAT_MINUTE_M_0		18553
#define	LON_DEGREE_KM		63
#define	LON_DEGREE_M		750
#define	LON_MINUTE_M_0		10625


typedef enum
{
	module_ready	= 0,
	module_setup_ubx,
	module_setup_nmea,

	module_setup_baudRate	// must be last
} MODULE_SETUP;

#define	LEN_TIME		9	+ 1	// hhmmss.00\0
#define	LEN_DATE		6	+ 1	// ddmmyy
#define	LEN_STATUS		1	+ 1	// A=data valid or V=data not valid
#define	LEN_LATITUDE	10	+ 1	// ddmm.mmmm
#define	LEN_N_S			1	+ 1	// N=north or S=south
#define	LEN_LONGITUDE	11	+ 1	// dddmm.mmmm
#define	LEN_E_W			1	+ 1	// E=east or W=west
#define	LEN_DEGREES		7	+ 1	// degrees
#define	LEN_KM			7	+ 1	// Km
#define	LEN_KNOTS		7	+ 1	// Knots
#define	LEN_ALTITUDE	5	+ 1	// Altitude
#define	LEN_SAT_TRACK	2	+ 1	// Satelites Tracking
#define	LEN_GPS			LEN_TIME + LEN_DATE + LEN_STATUS + LEN_LATITUDE + LEN_N_S + LEN_LONGITUDE + LEN_E_W + LEN_DEGREES + LEN_KM + LEN_KNOTS + LEN_ALTITUDE + LEN_SAT_TRACK

#define	LEN_EMPTY		1
#define	LEN_HEDER		5 + 1	// GPRMC / GPVTG
#define	LEN_NMEA		LEN_EMPTY + LEN_HEDER + LEN_GPS


#define	SECTION_WAIT	0
#define	SECTION_HEDER	1
#define	SECTION_GPRMC	2
#define	SECTION_STATUS	3
#define	SECTION_GPVTG	15
#define	SECTION_GPGGA	24
#define	SECTION_UBX_PL	0xFD
#define	SECTION_UBX_ID	0xFE
#define	SECTION_CRC		0xFF
#define	UBX_HEDER_SIZE	4

// GPS Data buffer structure Defined
#define	START_EMPTY		0
#define	START_HEDER		START_EMPTY		+ LEN_EMPTY
#define	START_DATE		START_HEDER		+ LEN_HEDER
#define	START_TIME		START_DATE		+ LEN_DATE
#define	START_STATUS	START_TIME		+ LEN_TIME
#define	START_LATITUDE	START_STATUS	+ LEN_STATUS
#define	START_N_S		START_LATITUDE	+ LEN_LATITUDE
#define	START_LONGITUDE	START_N_S		+ LEN_N_S
#define	START_E_W		START_LONGITUDE	+ LEN_LONGITUDE
#define	START_DEGREES	START_E_W		+ LEN_E_W
#define	START_KM		START_DEGREES	+ LEN_DEGREES
#define	START_KNOTS		START_KM		+ LEN_KM
#define	START_ALTITUDE	START_KNOTS		+ LEN_KNOTS
#define	START_SAT_TRACK	START_ALTITUDE	+ LEN_ALTITUDE

typedef struct
{
	U8	start;
	U8	len;
} SECTION;

typedef union
{
	char str[LEN_DATE];
	struct
	{
		char	dayTens[1];
		char	dayOnes[1];
		char	moonTens[1];
		char	moonOnes[1];
		char	yearTens[1];
		char	yearOnes[1];
	};
} DATE;

typedef union
{
	char str[LEN_TIME];
	struct
	{
		U8	hourTens;
		U8	hourOnes;
		U8	minTens;
		U8	minOnes;
		U8	secTens;
		U8	secOnes;
		U8	point;
		U8	sec_100Tens;
		U8	sec_100Ones;
		U8	null;
	};
} TIME;

typedef union
{
	char str[LEN_DATE + LEN_TIME];
	struct
	{
		DATE	date;
		TIME	time;
	};
} DATE_TIME;

typedef union
{
	U8	bytes[LEN_GPS];
	struct
	{
		DATE	Date;
		TIME	Time;
		char	status	[LEN_STATUS];
		char	latitude[LEN_LATITUDE];
		char	n_s		[LEN_N_S];
		char	longitude[LEN_LONGITUDE];
		char	e_w		[LEN_E_W];
		char	degrees	[LEN_DEGREES];
		char	km		[LEN_KM];
		char	knots	[LEN_KNOTS];
		char	altitude[LEN_ALTITUDE];
		char	satTrack[LEN_SAT_TRACK];
	};
} GPS_DATA;

typedef union
{
	U8	bytes[LEN_NMEA];
	struct
	{
		char		empty	[LEN_EMPTY];
		char		heder	[LEN_HEDER];
		GPS_DATA	gps;
	};
} GPS_DATA_RECEIVED;

typedef union			// Lat	 56|59.84787 degrees = 0:90
{						// Lon	024|16.39522 degrees = 0:180
	U8	bytes[4];		//	   degr|minutes
//	U16	words[2];		//			65535
	struct
	{
		U8	sign;
		U8	degrees;
		U16	minutes;
	};
	struct
	{
		U16	km;
		U16	meters;
	};
} LAT_LON_VALUE;

#define	NAME_LEN	20
typedef struct
{
	char			number[NAME_LEN];
	char			name[NAME_LEN];
	U8				active;
	U8				csq;
	U16				lastReceived;
	BATTERY_LEVEL	AdcBattery;
	BATTERY_LEVEL	AdcPower;
	GPS_DATA		gps;
	LAT_LON_VALUE	latVal;
	LAT_LON_VALUE	lonVal;
	LAT_LON_VALUE	distance;
	U16				degrees;
} TARGET_DATA;
*
RU8 KmConvert[2][3] =
{
	{111, 32, 186},		// Lat: 1degree = ~111.32km		1min = 1855.33m
	{63, 75, 106}		// Lon: 1degree = ~63.75km		1min = 1062.5m
0.0000001	0.000.011.132cm
0.0000001	0.000.00.006.3.75

};
*
#define	UBX_SYNC_CHAR_1			0xB5
#define	UBX_SYNC_CHAR_2			0x62

#define	UBX_ID_RXM_ALM			0x3002	// Poll Request			Poll GPS Constellation Almanac Data
//		UBX_ID_RXM_ALM			0x3002	// Poll Request			Poll GPS Constellation Almanac Data for a SV
//		UBX_ID_RXM_ALM			0x3002	// Poll Answer/Periodic	PS Aiding Almanac Input/Output Message
#define	UBX_ID_RXM_EPH			0x3102	// Poll Request			Poll GPS Constellation Ephemeris Data
//		UBX_ID_RXM_EPH			0x3102	// Poll Request			Poll GPS Constellation Ephemeris Data for a SV
//		UBX_ID_RXM_EPH			0x3102	// Poll Answer/Periodic	GPS Aiding Ephemeris Input/Output Message
#define	UBX_ID_RXM_PMREQ		0x4102	// Command				Requests a Power Management task
#define	UBX_ID_RXM_SFRB			0x1102	// Periodic				Subframe Buffer

#define	UBX_ID_ACK_ACK			0x0105	// Output Message Acknowledged
#define	UBX_ID_ACK_NAK			0x0005	// Output Message Not-Acknowledged

#define	UBX_ID_CFG_ANT			0x1306	// Poll					Request Poll Antenna Control Settings
//		UBX_ID_CFG_ANT			0x1306	// Input/Output			Antenna Control Settings
#define	UBX_ID_CFG_CFG			0x0906	// Command				Clear, Save and Load configurations
#define	UBX_ID_CFG_DAT			0x0606	// Poll					Request Poll Datum Setting
//		UBX_ID_CFG_DAT			0x0606	// Input				Set User-defined Datum
//		UBX_ID_CFG_DAT			0x0606	// Output				The currently defined Datum
#define	UBX_ID_CFG_GNSS			0x3E06	// Poll					Request Polls the configuration of the GNSS system configuration
//		UBX_ID_CFG_GNSS			0x3E06	// sOutput				GNSS system configuration
#define	UBX_ID_CFG_INF			0x0206	// Poll					Request Poll INF message configuration for one protocol
//		UBX_ID_CFG_INF			0x0206	// Input/Output			Information message configuration
#define	UBX_ID_CFG_ITFM			0x3906	// Poll Request			Polls the Jamming/Interference Monitor configuration.
//		UBX_ID_CFG_ITFM			0x3906	// Command				Jamming/Interference Monitor configuration.
#define	UBX_ID_CFG_LOGFILTER	0x4706	// Poll Request			Poll Data Logger filter Configuration
//		UBX_ID_CFG_LOGFILTER	0x4706	// Input/Output			Data Logger Configuration
#define	UBX_ID_CFG_NAV5			0x2406	// Poll Request			Poll Navigation Engine Settings
//		UBX_ID_CFG_NAV5			0x2406	// Input/Output			Navigation Engine Settings
#define	UBX_ID_CFG_NMEA			0x1706	// Poll Request			Poll the NMEA protocol configuration
//		UBX_ID_CFG_NMEA			0x1706	// Input/Output			NMEA protocol configuration (deprecated)
//		UBX_ID_CFG_NMEA			0x1706	// Input/Output			NMEA protocol configuration
#define	UBX_ID_CFG_NVS			0x2206	// Command				Clear, Save and Load non-volatile storage data
#define	UBX_ID_CFG_PM2			0x3B06	// Poll Request			Poll extended Power Management configuration
//		UBX_ID_CFG_PM2			0x3B06	// Input/Output			Extended Power Management configuration
#define	UBX_ID_CFG_RINV			0x3406	// Poll Request			Poll contents of Remote Inventory
//		UBX_ID_CFG_RINV			0x3406	// Input/Output			Contents of Remote Inventory
#define	UBX_ID_CFG_RST			0x0406	// Command				Reset Receiver / Clear Backup Data Structures
#define	UBX_ID_CFG_RXM			0x1106	// Poll Request			Poll RXM configuration
//		UBX_ID_CFG_RXM			0x1106	// Input/Output			RXM configuration
#define	UBX_ID_CFG_SBAS			0x1606	// Poll Request			Poll contents of SBAS Configuration
//		UBX_ID_CFG_SBAS			0x1606	// Input/Output			SBAS Configuration
#define	UBX_ID_CFG_TP5			0x3106	// Poll Request			Poll Time Pulse Parameters
//		UBX_ID_CFG_TP5			0x3106	// Poll Request			Poll Time Pulse Parameters
//		UBX_ID_CFG_TP5			0x3106	// Input/Output			Time Pulse Parameters
#define	UBX_ID_CFG_USB			0x1B06	// Poll Request			Poll a USB configuration
//		UBX_ID_CFG_USB			0x1B06	// Input/Output			USB Configuration

#define	UBX_ID_AID_ALM			0x300B	// Poll					Request Poll GPS Aiding Almanac Data
//		UBX_ID_AID_ALM			0x300B	// Poll 				Request Poll GPS Aiding Almanac Data for a SV
//		UBX_ID_AID_ALM			0x300B	// Input/Output			GPS Aiding Almanac Input/Output Message
#define	UBX_ID_AID_ALPSRV		0x320B	// Output				ALP client requests AlmanacPlus data from server
//		UBX_ID_AID_ALPSRV		0x320B	// Input				ALP server sends AlmanacPlus data to client
//		UBX_ID_AID_ALPSRV		0x320B	// Output				ALP client sends AlmanacPlus data to server.
#define	UBX_ID_AID_ALP			0x500B	// Input				ALP file data transfer to the receiver
//		UBX_ID_AID_ALP			0x500B	// Input				Mark end of data transfer
//		UBX_ID_AID_ALP			0x500B	// Output				Acknowledges a data transfer
//		UBX_ID_AID_ALP			0x500B	// Output				Indicate problems with a data transfer
//		UBX_ID_AID_ALP			0x500B	// Periodic/Polled Poll	the AlmanacPlus status
#define	UBX_ID_AID_AOP			0x330B	// Poll request Poll	AssistNow Autonomous data
//		UBX_ID_AID_AOP			0x330B	// Poll request Poll	AssistNow Autonomous data for one satellite
//		UBX_ID_AID_AOP			0x330B	// Input/Output			AssistNow Autonomous data
#define	UBX_ID_AID_DATA			0x100B	// Poll Request			Polls all GPS Initial Aiding Data
#define	UBX_ID_AID_EPH			0x310B	// Poll Request			Poll GPS Aiding Ephemeris Data
//		UBX_ID_AID_EPH			0x310B	// Poll Request			Poll GPS Aiding Ephemeris Data for a SV
//		UBX_ID_AID_EPH			0x310B	// Input/Output			GPS Aiding Ephemeris Input/Output Message
#define	UBX_ID_AID_HUI			0x020B	// Poll Request			Poll GPS Health, UTC and ionosphere parameters
//		UBX_ID_AID_HUI			0x020B	// Input/Output			GPS Health, UTC and ionosphere parameters
#define	UBX_ID_AID_INI			0x010B	// Poll Request			Poll GPS Initial Aiding Data
//		UBX_ID_AID_INI			0x010B	// Input/Output			Aiding position, time, frequency, clock drift
#define	UBX_ID_AID_REQ			0x000B	// Virtual Sends a poll	(AID-DATA) for all GPS Aiding Data

#define	U16(X)					Lo(X), Hi(X)
#define	U32(X)					(U8)((X>>0) & 0x00FF), (U8)((X>>8) & 0x00FF), (U8)((X>>16) & 0x00FF), (U8)((X>>24) & 0x00FF)

#define	SEND_UBX_STRING(X)		GpsSendUbx(X, (sizeof(X) - 2));

*
#define	UBX_ID_
#define	UBX_ID_
#define	UBX_ID_
#define	UBX_ID_
#define	UBX_ID_
#define	UBX_ID_
#define	UBX_ID_
*

typedef union
{
	U8	bytes[6];
	U16	words[3];
	struct
	{
		U16		classId;
		U16		len;
		P_U8	ptr;
	};
} UBX_HEDER;

typedef struct		// NAV-SVINFO (0x01 0x30)
{
	U32		iTOW;		// ms	GPS time of week of the navigation epoch.
	U8		numCh;		//		Number of channels
	U8		globalFlags;//		Bitmask (see graphic below)
	U16		reserved2;	//		Reserved
} NAV_SVINFO_HEDER;

typedef struct		// NAV-SVINFO (0x01 0x30)
{
	U8		chn;		//		Channel number, 255 for SVs not assigned to a channel
	U8		svid;		//		Satellite ID, see Satellite numbering for assignment
	U8		flags;		//		Bitmask (see graphic below)
	U8		quality;	//		Bitfield (see graphic below)
	U8		cno;		// dBHz	Carrier to Noise Ratio (Signal Strength)
	U8		elev;		// deg	Elevation in integer degrees
	S16		azim;		// deg	Azimuth in integer degrees
	U32		prRes;		// cm	Pseudo range residual in centimetres
} NAV_SVINFO_DATA;

typedef struct		// CFG-NAVX5 (0x06 0x23)
{
	U16		version;	// 			Message version (0 for this version)
	FLAGS	mask1;		// 			First Parameters Bitmask. Only the flagged parameters will be applied, unused bits must be set to 0. (see graphic below)
	U32		reserved0;	// 			Always set to zero
	U8		reserved1;	// 			Always set to zero
	U8		reserved2;	// 			Always set to zero
	U8		minSVs;		// #SVs		Minimum number of satellites for navigation
	U8		maxSVs;		// #SVs		Maximum number of satellites for navigation
	U8		minCNO;		// #dBHz	Minimum satellite signal level for navigation
	U8		reserved5;	// 			Always set to zero
	U8		iniFix3D;	// 			Initial Fix must be 3D flag (0=false/1=true)
	U8		reserved6;	// 			Always set to zero
	U8		reserved7;	// 			Always set to zero
	U8		reserved8;	// 			Always set to zero
	U16		wknRollover;// 			GPS week rollover number; GPS week numbers will be set correctly from this week up to 1024 weeks after this week. Setting this to 0 reverts to firmware default.
	U32		reserved9;	// 			Always set to zero
	U8		reserved10;	// 			Always set to zero
	U8		reserved11;	// 			Always set to zero
	U8		usePPP;		// 			Only supported on certain product variants use Precise Point Positioning flag (0=false/1=true)
	U8		aopCfg;		// 			AssistNow Autonomous configuration (see graphic below)
	U8		reserved12;	// 			Always set to zero
	U8		reserved13;	// 			Always set to zero
	U16		aopOrbMaxErr;// m 		m maximum acceptable (modelled) AssistNow Autonomous orbit error (valid range = 5..1000, or 0 = reset to firmware default)
	U8		reserved14;	// 			Always set to zero
	U8		reserved15;	// 			Always set to zero
	U16		reserved3;	// 			Always set to zero
	U32		reserved4;	// 			Always set to zero
} CFG_NAVX5_STRUCT;

#define	SVINFO_DATA_MAX		16
#define	RAW_DATA_MAX		16
#define	RAW_DATA_DELTA		2
typedef struct			// RXM-RAW (0x02 0x10) structure of heder
{
	S32		rcvTow;		// ms		Measurement time of week in receiver local time
	S16		week;		// weeks	Measurement week number in receiver local time
	U8		numSV;		// Number of satellites following
	U8		reserved1;	// Reserved
} RXM_RAW_HEDER;

typedef struct			// RXM-RAW (0x02 0x10) structure for 1 satelite data
{
	F64		cpMes;		// cycles	Carrier phase measurement	[L1 cycles]
	F64		prMes;		// m		Pseudorange measurement		[m]
	F32		doMes;		// Hz		Doppler measurement (positive sign for approaching satellites) [Hz]
	U8		sv;			// Space Vehicle number
	S8		mesQI;		// Nav Measurements Quality Indicator: >=4: prMes+doMes=OK	>=5 : prMes+doMes+cpMes=OK; <6 : likely loss of carrier lock in previous interval
	S8		cno;		// dBHz		Signal strength C/No
	U8		lli;		// Loss of lock indicator (RINEX definition)
} RXM_RAW_DATA;

typedef struct
{
	void*	ptr;
	U8		sv;			// Space Vehicle number
	U8		active;		// 
	F64		CpMesDiff;	// cycles	Carrier phase measurement	[L1 cycles]	(Difference from previously received data)
	F64		PrMesDiff;	// m		Pseudorange measurement		[m]			(Difference from previously received data)
	S16		cyclesLastResidual;
	S16		cyclesDeltaAverage;
	S16		cyclesDeltaSum;
	S16		metersLastResidual;
	S16		metersDeltaAverage;
	S16		metersDeltaSum;
	P_S16	ptrInDeltaCyclesBuff;
	P_S16	ptrInDeltaMetersBuff;
	P_S16	ptrInDeltaLatBuff;
	P_S16	ptrInDeltaLonBuff;
	S16		position;	// 
	S16		azim;		// Azimuth
	S8		elev;		// Elevation
	U8		reserved;	// 
} RXM_DELTAS;

typedef struct			// RXM-SVSI (0x02 0x20) structure of heder
{
	U32		iTOW;		// ms		GPS time of week of the navigation epoch
	S16		week;		// weeks	GPS week number of the navigation epoch
	U8		numVis;		// Number of visible satellites
	U8		numSV;		// Number of per-SV data blocks following
} RXM_SVSI_HEDER;

typedef struct			// RXM-SVSI (0x02 0x20) structure for 1 satelite data
{
	U8		svid;		// Satellite ID
	U8		svFlag;		// Information Flags (see graphic below)
	S16		azim;		// Azimuth
	S8		elev;		// Elevation
	U8		age;		// Age of Almanac and Ephemeris:
} RXM_SVSI_DATA;

typedef	struct
{
	S32		ecefX;		// cm	ECEF X coordinate
	S32		ecefY;		// cm	ECEF Y coordinate
	S32		ecefZ;		// cm	ECEF Z coordinate
	U32		pAcc;		// cm	Position Accuracy Estimate

	U8		raw_numSV;	// Number of satellites used in RAW data
	U8		svsi_numSV;	// Number of satellites used in SVSI data
	U8		svinfo_numCh;// Number of channels	used in SVINFO data
	U8		numVis;		// Number of Visible satellites
} UBX_DATA;
*/
typedef struct			// CFG-PRT (0x06 0x00)
{						// Get/Set Port Configuration for UART
	U8		portID;		// Port Identifier Number (see SerialCommunication Ports Description for valid UARTport IDs)
	U8		res0;		// Reserved
	FLAGS	f_txReady;	// TX ready PIN configuration (see graphic below)	(NOT IN u-blox5 - reserced)
	U32		mode;		// A bit mask describing the UART mode (see graphic below)
	U32		baudRate;	// Bits/s Baudrate in bits/second
	FLAGS	inProtoMask;// Active inp protocols mask. Multiple protocols can be definedon a single port. (see graphic below)
	FLAGS	outProtoMask;// Active out protocols mask. Multiple protocols can be definedon a single port. (see graphic below)
	FLAGS	flags;		// Flags bit mask (see graphic below)
	U16		pad;		// Always set to zero
} CFG_PRT;

typedef struct		// NAV-POSECEF (0x01 0x01)
{
	U32	iTOW;		// ms	GPS time of week of the navigation epoch.
	S32	ecefX;		// cm	ECEF X coordinate
	S32	ecefY;		// cm	ECEF Y coordinate
	S32	ecefZ;		// cm	ECEF Z coordinate
	U32	pAcc;		// cm	Position Accuracy Estimate
} NAV_POSECEF;

typedef struct		// NAV-POSLLH (0x01 0x02)
{
	U32	iTOW;		// ms	GPS time of week of the navigation epoch.
	S32	lon;		// deg	Longitude
	S32	lat;		// deg	Latitude
	S32	height;		// mm	Height above ellipsoid
	S32	hMSL;		// mm	Height above mean sea level
	U32	hAcc;		// mm	Horizontal accuracy estimate
	U32	vAcc;		// mm	Vertical accuracy estimate
} NAV_POSLLH;

typedef struct		// NAV-STATUS (0x01 0x03)
{
	U32	iTOW;		// ms	GPS time of week of the navigation epoch.
	U8	gpsFix;		// -	0x00 = no fix	GPSfix Type
					//		0x01 = dead reckoning only
					//		0x02 = 2D-fix
					//		0x03 = 3D-fix
					//		0x04 = GPS + dead reckoning combined
					//		0x05 = Time only fix
					//		0x06..0xff = reserved
	U8	flags;		// fl	Navigation Status Flags
	U8	diffStat;	// -	Differential Status
	U8	res;		// -	Reserved
	U32	ttff;		// -	Time to first fix (millisecond time tag)
	U32	msss;		// -	Milliseconds since Startup / Reset
} NAV_STATUS;

typedef struct			// NAV-PVT (0x01 0x07)
{
	U32		iTOW;		// ms		GPS time of week of the navigation epoch.
	U16		year;		// y		Year (UTC)
	U8		month;		// month	Month,				range 1..12 		(UTC)
	U8		day;		// d		Day of month,		range 1..31 		(UTC)
	U8		hour;		// h		Hour of day,		range 0..23 		(UTC)
	U8		min;		// min		Minute of hour,		range 0..59 		(UTC)
	U8		sec;		// s		Seconds of minute,	range 0..60 		(UTC)
	U8		valid;		// 			Validity Flags (see graphic below)
	U32		tAcc;		// ns		Time accuracy estimate					(UTC)
	S32		nano;		// ns		Fraction of second,	range -1e9 .. 1e9	(UTC)
	U8		fixType;	//			GNSSfix Type,		range 0..5
						//	0x00 = No Fix
						//	0x01 = Dead Reckoning only
						//	0x02 = 2D-Fix
						//	0x03 = 3D-Fix
						//	0x04 = GNSS + dead reckoning combined
						//	0x05 = Time only fix
						//	0x06..0xff: reserved
	U8		flags;		//			Fix Status Flags (see graphic below)
	U8		reserved;	//			Reserved
	U8		numSV;		//			Number of satellites used in Nav Solution
	UNI32	lon;		// deg		Longitude	(S32)
	UNI32	lat;		// deg		Latitude	(S32)
	S32		height;		// mm		Height above ellipsoid
	S32		hMSL;		// mm		Height above mean sea level
	U32		hAcc;		// mm		Horizontal accuracy estimate
	U32		vAcc;		// mm		Vertical accuracy estimate
	S32		velN;		// mm/s		NED north	velocity
	S32		velE;		// mm/s		NED east	velocity
	S32		velD;		// mm/s		NED down	velocity
	S32		gSpeed;		// mm/s		Ground Speed (2-D)
	S32		heading;	// deg		Heading of motion 2-D
	U32		sAcc;		// mm/s		Speed Accuracy Estimate
	U32		headingAcc;	// deg		Heading Accuracy Estimate
	U16		pDOP;		//			Position DOP
	FLAGS	reserved2;	//			Reserved
	U32		reserved3;	//			Reserved
} NAV_PVT;

typedef struct			// NAV-VELNED (0x01 0x12)
{						// Velocity Solution in NED
	U32		iTOW;		// ms	GPS time of week of the navigation epoch.
	S32		velN;		// cm/s	NED north velocity
	S32		velE;		// cm/s	NED east velocity
	S32		velD;		// cm/s	NED down velocity
	U32		speed;		// cm/s	Speed		 	(3-D)
	UNI32	gSpeed;		// cm/s	Ground Speed 	(2-D)				(unsigned)
	UNI32	heading;	// deg	Heading		 	(2-D)				(Scale 1e-5)
	U32		sAcc;		// cm/s	Speed			Accuracy Estimate
	U32		cAcc;		// deg	Course/Heading	Accuracy Estimate	(Scale 1e-5)
} NAV_VELNED;

typedef struct			// NAV-TIMEUTC (0x01 0x21)
{						// UTC Time Solution
	U32		iTOW;		// ms	GPS time of week of the navigation epoch.
	U32		tAcc;		// ns	Time Accuracy Estimate
	S32		nano;		// ns	Nanoseconds of second, range -500000000 ... 500000000 (UTC)
	U16		year;		// y	Year,			range 1999..2099	(UTC)
	U8		month;		// mon	Month,			range 1..12			(UTC)
	U8		day;		// d	Day of Month,	range 1..31			(UTC)
	U8		hour;		// h	Hour of Day,	range 0..23			(UTC)
	U8		min;		// min	Minute of Hour,	range 0..59			(UTC)
	U8		sec;		// s	Seconds of Min,	range 0..59			(UTC)
	U8		valid;		// fl	Validity Flags
} NAV_TIMEUTC;

typedef struct
{
	U16		year;		// y	Year,			range 1999..2099	(UTC)
	U8		month;		// mon	Month,			range 1..12			(UTC)
	U8		day;		// d	Day of Month,	range 1..31			(UTC)
	U8		hour;		// h	Hour of Day,	range 0..23			(UTC)
	U8		min;		// min	Minute of Hour,	range 0..59			(UTC)
	U8		sec;		// s	Seconds of Min,	range 0..59			(UTC)
	U8		gpsFix;		// -	0x00 = no fix	GPSfix Type
						//		0x01 = dead reckoning only
						//		0x02 = 2D-fix
						//		0x03 = 3D-fix
						//		0x04 = GPS + dead reckoning combined
						//		0x05 = Time only fix
						//		0x06..0xff = reserved
	UNI32	lon;		// deg	Longitude	(S32)
	UNI32	lat;		// deg	Latitude	(S32)
	UNI32	gSpeed;		// cm/s	Ground Speed 	(2-D)
	UNI32	heading;	// deg	Heading		 	(2-D)				(Scale 1e-5)
} GPS_UBX_DATA;

typedef union
{	//	++ssssss LaLaLaLa LoLoLoLo	| 64sec	| Lat_91.2m/s(328.3km/h) | lon_104.5m/s(376km/h)
	U8	bytes[3];
	struct
	{
		struct
		{
			U8	sec		:6;
			U8	lonSign	:1;
			U8	latSign	:1;
		};
		U8		lat;
		U8		lon;
	};
} GPS_OFFSET;
/*
typedef union
{	//	++ssssss LaLaLaLa LoLoLoLo	| 64sec	| Lat_91.2m/s(328.3km/h) | lon_104.5m/s(376km/h)
	U8	bytes[4];
	struct
	{
		struct
		{
			U8	sec		:6;
			U8	lonSign	:1;
			U8	latSign	:1;
		};
		U8		lat;
		U8		lon;
		U8		lon2;
	};
} GPS_OFFSET_2;
*/
typedef union				// 0x12345678
{
	U8	bytes[4];
	struct
	{
		U32 sec		:6;		//         38
		U32 lonSign	:1;		//         1
		U32 latSign	:1;		//         0
		U32 lat		:12;	//      456
		U32 lon		:12;	//   123
	};
} GPS_OFFSET_2;


/*
	
2016-04-15 22:32:04	56.9976190  24.2731754
2016-04-15 22:32:21	56.9976030  24.2732202
2016-04-15 22:32:44 56.9975742	24.2732074




typedef union
{	//	f+ssssss +LLLLLLL LLLLllll llllllll	| 64sec	/ Lat_20.5km/h	/ lon_23.5km/h
	//	F++ssssL LLLLLLLL LLLlllll llllllll	| 16sec	/ Lat_164km/h	/ lon_188km/h
	U8	bytes[4];
	U32	u32;
	struct	{	//	f+ssssss +LLLLLLL LLLLllll llllllll	| 64sec	/ Lat_20.5km/h	/ lon_23.5km/h
		U32	fast	:1;
		U32	latSign	:1;
		U32	lonSign	:1;
		U32	sec		:6;
		U32	lat		:11;
		U32	lon		:12;
	} slow;
	struct	{	//	F++ssssL LLLLLLLL LLLlllll llllllll	| 16sec	/ Lat_164km/h	/ lon_188km/h
		U32	fast	:1;
		U32	latSign	:1;
		U32	lonSign	:1;
		U32	sec		:4;
		U32	lat		:12;
		U32	lon		:13;
	} fast;
} GPS_OFFSET;
*/
//______________________________________________UBX_________________________________________________________________________________________
const	U8	CfgPort_Uart[]			= {U16(UBX_ID_CFG_PRT), 1										};	// ubx	pull uart settings
//const	U8	CfgPort_UartSet[]		= {U16(UBX_ID_CFG_PRT),	1,											// ubx	set	 uart settings
//	0, U16(0), U32(0x000008C0), U32(GPS_BAUDRATE), U16(0x0007), U16(0x0003), U16(0), U16(0)};
const	U8	CfgMeasureRate[]		= {U16(UBX_ID_CFG_RATE),											// ubx	Navigation/Measurement Rate Settings
	U16(1000 / MEASURE_RATE_HZ), U16(1), U16(1)};														//		ms, cycles Nav Rate, UTC(0)/GPS(1) time 
const	U8	NavPosllh_Pull[]		= {U16(UBX_ID_NAV_POSLLH)										};								// ubx	POSLLH msg pull
const	U8	CfgMsgRate_RMC[]		= {U16(UBX_ID_CFG_MSG),	U16(NMEA_ID_RMC),	0					};	// nmea	RMC   msg rate		NavMeasureRate / x
const	U8	CfgMsgRate_VTG[]		= {U16(UBX_ID_CFG_MSG), U16(NMEA_ID_VTG), 	0					};	// nmea	VTG   msg rate		NavMeasureRate / x
const	U8	CfgMsgRate_GGA[]		= {U16(UBX_ID_CFG_MSG), U16(NMEA_ID_GGA), 	0					};	// nmea	GGA   msg rate		NavMeasureRate / x
const	U8	CfgMsgRate_GSA[]		= {U16(UBX_ID_CFG_MSG), U16(NMEA_ID_GSA), 	0					};	// nmea	GGA   msg rate		NavMeasureRate / x
const	U8	CfgMsgRate_GSV[]		= {U16(UBX_ID_CFG_MSG), U16(NMEA_ID_GSV), 	0					};	// nmea	GGA   msg rate		NavMeasureRate / x
const	U8	CfgMsgRate_GLL[]		= {U16(UBX_ID_CFG_MSG), U16(NMEA_ID_GLL), 	0					};	// nmea	GGA   msg rate		NavMeasureRate / x

const	U8	CfgMsgRate_STATUS[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_STATUS),	MEASURE_RATE_HZ	};	// ubx	POSLLH	msg rate	NavMeasureRate / 1
const	U8	CfgMsgRate_TIMEUTC[]	= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_TIMEUTC),MEASURE_RATE_HZ	};	// ubx	TIMEUTC	msg rate	NavMeasureRate / 1
const	U8	CfgMsgRate_POSLLH[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_POSLLH),	MEASURE_RATE_HZ	};	// ubx	POSLLH	msg rate	NavMeasureRate / 1
const	U8	CfgMsgRate_VELNED[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_VELNED),	MEASURE_RATE_HZ	};	// ubx	POSLLH	msg rate	NavMeasureRate / 1

const	U8	CfgMsgRate_POSECEF[]	= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_POSECEF),0				};	// ubx	POSECEF	msg rate	NavMeasureRate / 1
const	U8	CfgMsgRate_SVINFO[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_SVINFO),	4				};	// ubx	PVT		msg rate	NavMeasureRate / 2
const	U8	CfgMsgRate_PVT[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_NAV_PVT),	MEASURE_RATE_HZ	};	// ubx	PVT		msg rate	NavMeasureRate / 2
const	U8	CfgMsgRate_RAW[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_RXM_RAW),	1				};	// ubx	RAWdata	msg rate	NavMeasureRate / 1
const	U8	CfgMsgRate_SVSI[]		= {U16(UBX_ID_CFG_MSG), U16(UBX_ID_RXM_SVSI),	0				};	// ubx	SVSI	msg rate	NavMeasureRate / 1
const	U8	CfgMsgNavx5[]			= {U16(UBX_ID_CFG_NAVX5),											// ubx	Navigation Engine Expert Settings
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif	// _gpsDefines_h_
/*	Visādi GPS aprēķini

NMEA  grādi|min.minDecimal
Lat		 56|59.84787 degrees = 0:90
Lon		024|16.39522 degrees = 0:180

288km/h = 8000cm/s						0x1FFF				  	0x0FFF					0x7FF
Lat 1g = 111.32km	1e-7 = 1.1132cm		8192g-7 = 320.6km/h		4096g-7 = 164.15km/h	2048g-7 = 82.07km/h
Lon	1g = 63.75km	1e-7 = 0.6375cm		8192g-7 = 188.0km/h		4096g-7 = 94.0km/h		2048g-7 = 47.0km/h
Lat 1min = 1855.33m	1a-5 = 1.85533cm
Lon 1min = 1062.5m	1a-5 = 1.0625cm
$GPRMC,161241.00,A,5659.84653,N,02416.39666,E,0.028,,200212,,,D*73

14bit			13bit			12bit			11bit
0x3FFF (16384)	0x1FFF (8192)	0x0FFF (4096)	0x7FF (2048)
656.6km/h		320.6km/h		164.15km/h		82.07km/h
376.0km/h		188.0km/h		94.0km/h		47.0km/h

F++ssLLL LLLLLLLL LLllllll llllllll	| 4 sec / Lat_320km/h / lon_376km.h		| 1/1/1/2/13/14
f++ssssL LLLLLLLL LLLlllll llllllll	| 16sec / Lat_164km/h / lon_188km/h 	| 1/1/1/4/12/13

S+ssssss +LLLLLLL LLLLllll llllllll	| 64sec	/ Lat_20.5km/h	/ lon_23.5km/h
F++ssssL LLLLLLLL LLLlllll llllllll	| 16sec	/ Lat_164km/h	/ lon_188km/h

UBX	 TIKAI grādi: signed 1e-7 -> grādi /= 10000000 ( *= 0.0000001)
Lat		569789191
Lon		242525372

Lat 11bit (Slow)	22.787204m
Lon 12bit (Slow)








*/