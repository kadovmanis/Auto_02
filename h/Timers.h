#ifndef	_timers_h_
#define	_timers_h_

#include	"Typedef.h"

#define	DELAY_1()				Nop()
#define	DELAY_2()				Nop(); Nop()
#define	DELAY_3()				Nop(); Nop(); Nop()
#define	DELAY_4()				Nop(); Nop(); Nop(); Nop()
#define	DELAY_1uS()				__delay32(16)
#define	DELAY_2uS()				__delay32(32)
#define	DELAY_I2C()				DELAY_2uS(); DELAY_2uS(); DELAY_2uS(); DELAY_2uS()
#define	BCD_UP(X)				X += ((X & 0x0F) == 0x09)? 7:1

#define	DC_PERIOD				160	//	8000
#define	DC_PERIOD_MIN			4
#define	DC_DUTY_MAX				50	//	300						// (DC_PERIOD / 2)

#define LED_STATUS_1			0b1000000000000000
#define LED_STATUS_1_I			0b0011111111111111
#define LED_STATUS_2			0b1001000000000000
#define LED_STATUS_3			0b1001001000000000
#define LED_STATUS_3_S			0b0000001000000000
#define LED_STATUS_4			0b1001001001000000
#define LED_STATUS_4_S			0b0000000001000000
#define LED_STATUS_5			0b1001001001001000
#define LED_STATUS_OFF			0b0000000000000000
#define LED_STATUS_ON			0b1111111111111111
#define LED_STATUS_SLOW			0b1111111100000000
#define LED_STATUS_ERROR		0b1010101010101010

typedef struct
{
	U16	year;		// y	Year,			range 1999..2099	(UTC)
	U8	month;		// mon	Month,			range 1..12			(UTC)
	U8	day;		// d	Day of Month,	range 1..31			(UTC)
	U8	hour;		// h	Hour of Day,	range 0..23			(UTC)
	U8	min;		// min	Minute of Hour,	range 0..59			(UTC)
	U8	sec;		// s	Seconds of Min,	range 0..59			(UTC)
	U8	gpsFix;		// -	0x00 = no fix	GPSfix Type
} TIME_ST;

void	OSCconfig		(OSC osc);
void	TimersInit		(void);
void	DelayMs			(U16 ms);
U16		GetTicsMs		(void);
U16		GetTimeSinceMs	(U16 previousTics);
U16		GetSec			(void);
U16		GetTimeSinceSec	(U16 previousSec);
void	GetTcpTime		(U32* tcpTime);
void	SetTcpTime		(U32* tcpTime);

extern	volatile	TIME_ST	SysTime;
extern	volatile	U16		StartYear;
extern	volatile	U16		IC3val[3];
//extern const		U16		BaudRates[5][8];

#endif

