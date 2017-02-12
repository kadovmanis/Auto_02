#ifndef		__TCP_TYPE_H__
#define		__TCP_TYPE_H__

/*****************	PACKET TYPES	******************
0x01	GPS_DATA		TCP_HEDER(8B)	*** Etalon LAT & LON swapped !!!!!! (GPS protocol (UBX) related) ***
		data:	Timestamp(4B)	Lon(4B), Lat(4B), Offset(+sec(1B),+lat<<5(1B),+lon<<6(1B)), Offset(),....
0x02	GPS_DATA		TCP_HEDER(8B)	*** Etalon LAT & LON swapped !!!!!! (GPS protocol (UBX) related) ***
		data:	Timestamp(4B)	Lon(4B), Lat(4B), Offset(+sec(1B),+lat<<5(12b),+lon<<6(12b)), Offset(),....
0x21	Online On/Off	TCP_HEDER(8B)
		data:	OnlineTimer(2B) - 0=Off, OnlineTimeoutSec(2B) - Packets interval
0xF0	(240) - GPRS Connection OK	no data
0xF1	(241) - WIFI Connection OK	no data
*****************************************************/

#define	TYPE_GPS_DATA		0x01	// (1)
#define	TYPE_GPS_DATA_2		0x02	// (2)
#define	TYPE_GPS_DATA_3		0x03	// (3)
#define	TYPE_POWER_DATA		0x08	// (8)
#define	TYPE_CURRENT_DATA	0x0F	// (15)

#define	TYPE_ONLINE			0x21	// (33)

#define	TYPE_BOOT_IDLE		0xD0	// (208)
#define	TYPE_BOOT_ST		0xD1	// (209)
#define	TYPE_BOOT_SET_ADDR	0xD2	// (210)
#define	TYPE_BOOT_WRITE		0xD3	// (211)
#define	TYPE_BOOT_READ		0xD4	// (212)
#define	TYPE_BOOT_FLASH		0xD5	// (213)
#define	TYPE_BOOT_RESET		0xDE	// (222)
#define	TYPE_BOOT_STATE		0xDF	// (223)

#define	TYPE_BOOT_START		0xE0	// (224)	/ 0xFE
#define	TYPE_RESET			0xE1	// (225)	/ 0x01
#define	TYPE_FLASH			0xE2	// (226)	/ 0x02
#define	TYPE_MEM_STATUS		0xE3	// (227)	0x1F
#define	TYPE_MEM_GET_ADDR	0xE4	// (228)	0x10
#define	TYPE_MEM_SET_ADDR	0xE5	// (229)	0x11
#define	TYPE_MEM_BOOT_ADDR	0xE6	// (230)	0x1A
#define	TYPE_MEM_WRITE		0xE7	// 0x12
#define	TYPE_MEM_READ		0xE8	// 0x13
#define	TYPE_MEM_S_ERASE	0xE9	// 0x15
#define	TYPE_MEM_B_ERASE	0xEA	// 0x16
#define	TYPE_MEM_FEEL		0xEB	// 0x1E
#define	TYPE_DEV_HW_INFO	0xEC	// (236)	/ 0xFD
#define	TYPE_DEV_SW_INFO	0xED	// (237)	/ 0xFD
#define	TYPE_TEST			0xEF	// (239)	/ 0xFF

#define	TYPE_OK_GPRS		0xF0	// (240)
#define	TYPE_OK_WIFI		0xF1	// (241)
#define	TYPE_GSM_SETUP		0xFD	// (253)
#define	TYPE_TEXT			0xFE	// (254)	/ 0xFC


#endif	//	__TCP_TYPE_H__

