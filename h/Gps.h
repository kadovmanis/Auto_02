#ifndef	_gps_h_
#define	_gps_h_

#include	"Typedef.h"

//#define	GPS_EXT_IF		_PMPIF				// Parallel Master Port Interrupt used for
//#define	GPS_EXT_IE		_PMPIE				// GPS_EXT Interrupt

void Gps_On	(void);
/*
LAT_LON_VALUE	GpsValue			(char* str);
LAT_LON_VALUE	Difference			(LAT_LON_VALUE* home, LAT_LON_VALUE* target);
LAT_LON_VALUE	Hypotenuse			(LAT_LON_VALUE* x, LAT_LON_VALUE* y);

extern	GPS_DATA	GpsNmeaData;
extern	U16			GpsDirection;


extern U8				ReceivedData[256];
extern RXM_SVSI_DATA	SvsiData[16];
extern NAV_SVINFO_DATA	SvinfoData[SVINFO_DATA_MAX];
extern RXM_RAW_DATA		RawData[3][RAW_DATA_MAX];
extern U8				NewRawIdx;
extern S16				RawLatitude, RawLongitude;

extern RXM_DELTAS		RawDeltas[RAW_DATA_MAX];

extern UBX_DATA			CurrentUbxData;
extern GPS_DATA			CurrentGpsData;

extern U16				GpsDirection, NorthDirection, HomeDirection, DistanceKm, DistanceM;
extern S16				GpsAltitude;

extern	TARGET_DATA		Targets[TARGET_NUMBER];
*/
#endif	// _gps_h_
/*
$GPGGA,094105.00,,,,,0,00,2.94,,,,,,*50
$GPGSA,A,1,,,,,,,,,,,,,4.35,2.94,3.21*0D
$GPGSV,3,1,12,02,43,068,44,04,09,037,,05,09,108,41,10,17,056,*7E
$GPGSV,3,2,12,12,40,123,48,25,78,161,42,29,66,232,35,30,06,299,*71
$GPGSV,3,3,12,31,45,290,27,33,16,224,38,37,25,183,44,40,16,135,41*77
$GPGLL,,,,,094105.00,V,N*43
$GPRMC,094106.00,V,,,,,,,250912,,,N*7A
$GPVTG,,,,,,,,,N*30
$GPGGA,094106.00,,,,,0,00,1.52,,,,,,*5A
*/

