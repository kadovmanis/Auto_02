#include	"Timers.h"
#include	"Ports.h"
#include	"Gsm.h"
#include	"Tcp.h"
#include	"Uart2.h"
#include	"Uart3.h"
#include	<string.h>
//#include	"Memory.h"
#include	"Input.h"


volatile	U16		LedStatus1 = LED_STATUS_OFF, LedStatus2 = LED_STATUS_OFF, LedStatus3 = LED_STATUS_OFF, LedStatus4 = LED_STATUS_OFF;
volatile	U16		Ms = 0;
volatile	U16		LedBit, Sec, Tics_ms = 0;
volatile	TIME_ST	SysTime;
volatile	U16		StartYear = 2000;


//-------------------------------------------------------------------------------------------------
//	OSC_4__MHZ_FRCDIV	Fast RC Oscillator with Postscaler (FRCDIV)
//	OSC_05_MHZ_FRC_16	Fast RC Oscillator/16 (500 KHz)
//	OSC_31_KHZ_LPRC		Low-Power RC Oscillator (LPRC)
//	OSC_0__MHZ_SOSC		Secondary Oscillator (SOSC)
//	OSC_0__MHZ_PRI_PLL	Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL)
//	OSC_0__MHZ_PRI		Primary Oscillator (XT, HS, EC)
//	OSC_32_MHZ_FRCPLL	Fast RC Oscillator with Postscaler and PLL module (FRCPLL)
//	OSC_8__MHZ_FRC		Fast RC Oscillator (FRC)
//-------------------------------------------------------------------------------------------------
OSC Oscillator = Osc_4__MHz;
const U16 BaudRates[5][8] =
{
	{
		((( 8000000 /2 / 16) / 9600) - 1),		// 26.04166
		(((32000000 /2 / 16) / 9600) - 1),		// 104.1666
		(((       0 /2 / 16) / 9600) - 1),		// 0
		(((       0 /2 / 16) / 9600) - 1),		// 0
		(((       0 /2 / 16) / 9600) - 1),		// 0
		(((   31000 /2 / 16) / 9600) - 1),		// 0
		(((  500000 /2 / 16) / 9600)),			// 1,627604
		((( 4000000 /2 / 16) / 9600) - 1)		// 13
	},
	{
		((( 8000000 /2 / 16) / 19200) - 1),		// 13.0208
		(((32000000 /2 / 16) / 19200) - 1),		// 53.0833
		(((       0 /2 / 16) / 19200) - 1),		// 0
		(((       0 /2 / 16) / 19200) - 1),		// 0
		(((       0 /2 / 16) / 19200) - 1),		// 0
		(((   31000 /2 / 16) / 19200) - 1),		// 0
		(((  500000 /2 / 16) / 19200) - 1),		// 0
		((( 4000000 /2 / 16) / 19200))			// 6.5104
	},
	{
		((( 8000000 /2 / 16) / 38400)),			// 6.5104
		(((32000000 /2 / 16) / 38400) - 1),		// 26.0416
		(((       0 /2 / 16) / 38400) - 1),		// 0
		(((       0 /2 / 16) / 38400) - 1),		// 0
		(((       0 /2 / 16) / 38400) - 1),		// 0
		(((   31000 /2 / 16) / 38400) - 1),		// 0
		(((  500000 /2 / 16) / 38400)),			// 0
		((( 4000000 /2 / 16) / 38400) - 1)		// 3.2552
	},
	{
		((( 8000000 /2 / 16) / 57600) - 1),		// 4.340278
		(((32000000 /2 / 16) / 57600) - 1),		// 17.36111
		(((       0 /2 / 16) / 57600) - 1),		// 0
		(((       0 /2 / 16) / 57600) - 1),		// 0
		(((       0 /2 / 16) / 57600) - 1),		// 0
		(((   31000 /2 / 16) / 57600) - 1),		// 0
		(((  500000 /2 / 16) / 57600)),			// 0
		((( 4000000 /2 / 16) / 57600) - 1)		// 2.170139
	},
	{
		((( 8000000 /2 / 16) / 115200) - 1),	// 2,170138
		(((32000000 /2 / 16) / 115200)),		// 8,680555
		(((       0 /2 / 16) / 115200) - 1),	// 0
		(((       0 /2 / 16) / 115200) - 1),	// 0
		(((       0 /2 / 16) / 115200) - 1),	// 0
		(((   31000 /2 / 16) / 115200) - 1),	// 0
		(((  500000 /2 / 16) / 115200)),		// 0
		((( 4000000 /2 / 16) / 115200) - 1)		// 1,085069
	},
};


void OSCconfig(OSC osc)
{
	Oscillator = osc;
//	CLKDIV	= 	0b0000000100000000
			//	  |||||||||||+++++	---	Unimplemented (Reserved)
			//	  ||||||||||+-----	---	PLLEN: 96 MHz PLL Enable bit
			//	  ||||||||++------	---	CPDIV<1:0>: System Clock Select bits (postscaler select from 32 MHz clock branch)
			//	  |||||+++--------	---	RCDIV<2:0>: FRC Postscaler Select bits
			//	  ||||+-----------	---	DOZEN: DOZE Enable bit
			//	  |+++------------	---	DOZE<2:0>: CPU Peripheral Clock Ratio Select bits
			//	  +---------------	---	ROI: Recover on Interrupt bit

	if ((osc == Osc_32_MHz) /*|| (osc == Osc_0_PriPLL)*/)
		_PLLEN = 1;					// Enable PLL 96 MHz


	__builtin_write_OSCCONH(osc);
	__builtin_write_OSCCONL(0x01);	// Initiate Clock Switch

	// Wait for Clock switch to occur
	while (_OSWEN);

	// Wait for PLL to lock
	if ((osc == Osc_32_MHz) /*|| (osc == Osc_0_PriPLL)*/)
	{
		while(_LOCK != 1);
	}
	else
		_PLLEN = 0;					// Disable PLL 96 MHz

	TimersInit();
//	Uart1SetBaudrate(BaudRates[0][osc]);
//	Uart2_SetBaudrate(BaudRate_115200);
//	Uart2_SetBaudrate(BaudRates[0][osc]);
	Uart3_SetBaudrate(BaudRates[3][osc]);
}


// **********************************************************************
//	Init All Timers
// **********************************************************************
void TimersInit(void)
{
	T1CON				= 0;		//	Timer1 reset
	T2CON				= 0;		//	Timer2 reset
	T3CON				= 0;		//	Timer3 reset
	T4CON				= 0;		//	Timer4 reset
	OC1CON1bits.OCM		= 0; 		// Output compare channel is disabled (PWM)
	OC2CON1bits.OCM		= 0; 		// Output compare channel is disabled (PWM)

	switch (Oscillator)
	{
	case Osc_8__MHz:
		T1CONbits.TCKPS	= 0b10;				//	T1 Prescaler	| 11b - 1:256;	10b - 1:64
		T2CONbits.TCKPS	= 0b00;				//	T2 Prescaler	| 01b - 1:8;	00b - 1:1
		T3CONbits.TCKPS	= 0b10;				//	T3 Prescaler
		PR1		= (4000000 / 64 / 1000);	//	FOSC / Prescaler / 1000Hz	(mS)
//		PR2		= DC_PERIOD;				//	DC/DC period
		PR3		= (4000000 / 64 / 1000);	//	FOSC / Prescaler / 1000Hz 	(mS)
		break;
	case Osc_32_MHz:
		T1CONbits.TCKPS	= 0b10;					//	T1 Prescaler	| 00b - 1:1
		T2CONbits.TCKPS	= 0b00;					//	T2 Prescaler	| 01b - 1:8
		T3CONbits.TCKPS	= 0b10;					//	T3 Prescaler	| 10b - 1:64
		T4CONbits.TCKPS	= 0b01;					//	T4 Prescaler	| 11b - 1:256
		PR1		= (16000000 /  64 / 1000) - 1;	//	FOSC / Prescaler / 1000Hz	(1mS)
		PR2		= (DC_PERIOD	 		  - 1);	//	DC/DC period
		PR3		= (16000000	/  64 / 100)  - 1;	//	FOSC / Prescaler / 100Hz 	(10mS)
		PR4		= (16000000 /   8 / 100)  - 1;	//	FOSC / Prescaler / 100Hz
		break;
	case Osc_31_kHz:
		T1CONbits.TCKPS	= 0b00;				//	T1 Prescaler	| 11b - 1:256;	10b - 1:64
		T2CONbits.TCKPS	= 0b00;				//	T2 Prescaler	| 01b - 1:8;	00b - 1:1
		T3CONbits.TCKPS	= 0b00;				//	T3 Prescaler
		PR1		= (15500 / 1 / 1000);		//	FOSC / Prescaler / 1000Hz	(mS)
//		PR2		= DC_PERIOD;				//	DC/DC period
		PR3		= (15500 / 1 / 1000);		//	FOSC / Prescaler / 1000Hz 	(mS)
		break;
	case Osc_05_MHz:
		T1CONbits.TCKPS	= 0b00;				//	T1 Prescaler	| 11b - 1:256;	10b - 1:64
		T2CONbits.TCKPS	= 0b00;				//	T2 Prescaler	| 01b - 1:8;	00b - 1:1
		T3CONbits.TCKPS	= 0b00;				//	T3 Prescaler
		PR1		= (250000 / 1 / 1000);		//	FOSC / Prescaler / 1000Hz	(mS)
//		PR2		= DC_PERIOD;				//	DC/DC period
		PR3		= (250000 / 1 / 1000);		//	FOSC / Prescaler / 1000Hz 	(mS)
		break;
	case Osc_4__MHz:
	case Osc_0_Pri:
	case Osc_0_PriPLL:
	case Osc_0_SOSC:
	default:
		T1CONbits.TCKPS	= 0b01;				//	T1 Prescaler	| 11b - 1:256;	10b - 1:64
		T2CONbits.TCKPS	= 0b00;				//	T2 Prescaler	| 01b - 1:8;	00b - 1:1
		T3CONbits.TCKPS	= 0b10;				//	T3 Prescaler
		PR1		= (2000000 / 8 / 1000);		//	FOSC / Prescaler / 1000Hz	(mS)
//		PR2		= DC_PERIOD;				//	DC/DC period
		PR3		= (2000000 / 64 / 1000);	//	FOSC / Prescaler / 1000Hz 	(mS)
		break;
	}

	OC1R					= 0;		// Initialize Compare Register1 with 0% duty cycle				(default)
//	OC1RS					= 0;		// Initialize Secondary Compare Register1 with 0% duty cycle	(default)
//	OC1CON2bits.SYNCSEL		= 0b01100;	// Trigger/Synchronization Source = Timer2						(default)
//	OC1CON2bits.OCTRIG		= 0;																		(default)
//	OC1CON1bits.TRIGMODE	= 0;																		(default)
//	OC1CON2bits.TRIGSTAT	= 0;																		(default)
//	OC1CON1bits.OCSIDL		= 0;		// Output capture will continue to operate in CPU Idle mode		(default)
//	OC1CON1bits.OCFLT		= 0;		// No PWM Fault condition has occurred (this bit is only used when OCM<2:0> = 111)	(default)
//	OC1CON1bits.OCTSEL		= 0b111;	// FCY is the clock source for output Compare
//	OC1CON1bits.OCTSEL		= 0b000;	// Timer2 is the clock source for output Compare				(default)
	OC1CON1bits.OCM			= 0b110;	// Edge-Aligned PWM Mode on OCx (Fault pin disabled)

	_T1IP = 4;		//	Timer1 Interrupt priority level=4
	_T2IP = 4;		//	Timer2 Interrupt priority level=4
	_T3IP = 4;		//	Timer3 Interrupt priority level=4
	_T4IP = 4;		//	Timer4 Interrupt priority level=4

	memset((char*)&SysTime, 0, sizeof(SysTime));
	T1CONbits.TON = 1;		//	Enable Timer1 and start the counter
	_T1IF = 0;				//	Reset Timer1 interrupt flag
	_T1IE = 1;				//	Enable Timer1 interrupt

	T2CONbits.TON = 1;		//	Enable Timer2 and start the counter
//	_T2IF = 0;				//	Reset Timer2 interrupt flag
//	_T2IE = 1;				//	Enable Timer2 interrupt

	T3CONbits.TON = 1;		//	Enable Timer3 and start the counter
	_T3IF = 0;				//	Reset Timer3 interrupt flag
	_T3IE = 1;				//	Enable Timer3 interrupt

	T4CONbits.TON = 1;		//	Enable Timer4 and start the counter
//	_T4IF = 0;				//	Clear  Timer4 interrupt flag
//	_T4IE = 1;				//	Enable Timer4 interrupt

//	_IC3IP	= 5;			//	Set		IC3 interrupt priority
//	_IC3IF	= 0;			//	Clear	IC3 interrupt flag
//	_IC3IE	= 1;			//	Enable	IC3 interrupt

	LED_TIM	= LED_STATUS_1;
}

// **********************************************************************
inline	void DcDcUpdate		(void);
inline	void SysTimeIrq		(void);
inline	void LedStatusUpdate(void);
//extern inline void SpiMemTimer(void);
extern	volatile U16	WifiSec;
// ......................................................................
// Timer1 INTERRUPT			1000Hz - 1ms
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt( void )
{
	_T1IF = 0;				//	Reset Timer1 interrupt flag

	Tics_ms++;
//	DcDcUpdate();
	SysTimeIrq();
	LedStatusUpdate();
}

inline void DcDcUpdate(void)
{
/*	U16 step, adc = AN_V18;

	ProcPowerReduction	= (adc > AN_V18_VALUE)?	(U8)((adc - 562) / 21): 0;
	step				= DcDcReductionSteps[ProcPowerReduction];

	adc = AN_DC_DC;
	if (adc > step)
	{
		if (OC1R)					OC1R--;
	}
	else if (adc < step)
	{
//		if (OC1R < DC_DUTY_MAX)		OC1R++;
	}
*/
}

inline	void SysTimeIrq	(void)
{
	if (++Ms >= 1000)
	{	Ms		= 0;
//		if (Sec & 0x0001)
			LedBit	= 0x8000;
		Sec++;
		#ifdef UART4_WIFI
			WifiSec++;
		#endif
		FL_GSM_SEC = 1;
		if (SysTime.gpsFix)					SysTime.gpsFix	= 0;
		else if	  (++SysTime.sec  >= 60) {	SysTime.sec		= 0;
			  if  (++SysTime.min  >= 60) {	SysTime.min		= 0;
			   if (++SysTime.hour >= 24) {	SysTime.hour	= 0;
				   ++SysTime.day;
			   }
			}
		}
		if (FL_WEB_ONLINE)
		{
			if (OnlineTimer)	OnlineTimer--;
			else				FL_WEB_ONLINE = 0;
		}
	}
}


inline void LedStatusUpdate(void)
{
	static	U8	ledTimer = 0;

	if (ledTimer)
		ledTimer--;
	else	if (LedBit)
	{
#if	(LED_COUNT == 2)
		static	U8	oneLedCnt = 0;
		switch (oneLedCnt)
		{
		case 0:	LED1 = (LedBit & 0xF000)?		1:0;
				LED2 = (LedBit & LedStatus1)?	1:0;	break;
		case 1:	LED1 = (LedBit & 0x8000)?		1:0;
				LED2 = (LedBit & LedStatus2)?	1:0;	break;
		case 2:	LED1 = (LedBit & 0x8000)?		1:0;
				LED2 = (LedBit & LedStatus3)?	1:0;	break;
		case 3:
		default:LED1 = (LedBit & 0x8000)?		1:0;
				LED2 = (LedBit & LedStatus4)?	1:0;	break;
		}
#else
 #ifdef	LED1
		LED1 = (LedStatus1 & LedBit)? 1:0;
 #endif
 #ifdef	LED2
		LED2 = (LedStatus2 & LedBit)? 1:0;
 #endif
 #ifdef	LED3
		LED3 = (LedStatus3 & LedBit)? 1:0;
 #endif
 #ifdef LED4
		LED4 = (LedStatus4 & LedBit)? 1:0;
 #endif
#endif
		if (LedBit >>= 1)
			ledTimer = 62;						// 1000hz / 16bit = 62.5
#if	(LED_COUNT == 2)
		else if (++oneLedCnt > 3)
			oneLedCnt = 0;
#endif
	}
}

// **********************************************************************
// Timer2 INTERRUPT			50kHz
// **********************************************************************
void __attribute__ ((interrupt, no_auto_psv)) _T2Interrupt(void)
{
	_T2IF = 0;				//	Reset Timer2 interrupt flag
}

// **********************************************************************
extern	inline	void	SpiMemTimer	(void);
extern	inline	void	UsbMsTimer	(void);
//#if (HARDWARE == HW_SEE)
// extern inline  void    SwitchTimer (void);
//#endif
// ......................................................................
// Timer3 INTERRUPT			100Hz - 10ms
// **********************************************************************
void __attribute__ ((interrupt, no_auto_psv)) _T3Interrupt(void)
{
	_T3IF = 0;				//	Reset Timer3 interrupt flag
	GSM_IF	= 1;
	SpiMemTimer();
	#ifdef UART4_WIFI
		WIFI_IF = 1;
		#if	(TEST != TEST_WIFI)
			if ((!WIFI_IE) && (WifiSec >= WIFI_WAKE_UP_S))
				WIFI_IE = 1;
		#endif
	#endif

    #if (HARDWARE == HW_SEE)
		SwitchTimer();
	#endif
//	UsbMsTimer();
}

void DelayMs(U16 ms)
{
	ms++;
	ms += Tics_ms;
	while (Tics_ms != ms);
}

U16 GetTicsMs(void)
{
//	return ((U16)(Bcd_2Dec(TimeBcd.sec)) * mS);
	return Tics_ms;
}

U16 GetTimeSinceMs(U16 previousTics)
{
	return (Tics_ms >= previousTics)?	(Tics_ms - previousTics):((0 - previousTics) + Tics_ms);
}

U16 GetSec(void)
{
//	return (Bcd_2Dec(TimeBcd.min) * 60) + Bcd_2Dec(TimeBcd.sec);
	return	Sec;
}

U16 GetTimeSinceSec(U16 previousSec)
{
//	U16 currentSec = GetSec();
//	return (currentSec > previousSec)?	(currentSec - previousSec):((0xFFFF - previousSec) + currentSec);
	return (Sec >= previousSec)?			(Sec - previousSec):((0 - previousSec) + Sec);
}

// ggggggdd dddhhhhh MMMMmmmm mmssssss
typedef union
{
	UNI32	dword;
	struct
	{
		U16	sec		:6;
		U16	min		:6;
		U16	moon	:4;
		U16	hour	:5;
		U16	day		:5;
		U16	year	:6;
	};
} TCP_TIME;

void GetTcpTime	(U32* tcpTime)
{
	static TCP_TIME	TCPtime = {.dword.u32 = 0};
	static U8		hour = 255;
	if (hour != SysTime.hour)
	{
		hour = SysTime.hour;
		TCPtime.year	= (SysTime.year > StartYear)? (SysTime.year - StartYear) : SysTime.year;
		TCPtime.day		= SysTime.day;
		TCPtime.hour	= SysTime.hour;
	}
	TCPtime.moon		= SysTime.month;
	TCPtime.min			= SysTime.min;
	TCPtime.sec			= SysTime.sec;

	*tcpTime = TCPtime.dword.u32;
}

void SetTcpTime	(U32* tcpTime)
{
	register TCP_TIME* t_Time = (TCP_TIME*)tcpTime;
	if (SysTime.day	!= t_Time->day)
	{
		SysTime.sec		= t_Time->sec;
		SysTime.min		= t_Time->min;
		SysTime.month	= t_Time->moon;
		SysTime.hour	= t_Time->hour;
		SysTime.day		= t_Time->day;
		SysTime.year	= t_Time->year + StartYear;
	}
}

// **********************************************************************
// ......................................................................
// IC3 INTERRUPT			GSM PWM Input Compare
// **********************************************************************
void __attribute__((interrupt, no_auto_psv)) _IC3Interrupt( void )
{
/*	if(!GSM_ANALOG_IN)
		IC3CON2bits.TRIGSTAT	= 1;	// Release	IC3 Timer by manual Trigger
	else
	{
		while (IC3CON1bits.ICBNE)		// Buffer Empty Status bit
			IC3val[0] = IC3BUF;
		IC3CON2bits.TRIGSTAT	= 0;	// Stop		IC3 Timer by manual Trigger
	}
*/
	_IC3IF	= 0;			//	Clear	IC3 interrupt flag
//	FLAG_IC3 = 1;
}


/*
inline void LedAndButtons(void)
{
    U8 i = (U8)(SystemTime.ms & 0x3F);
    if (!i)
    {
        Button1Check();
        LedStatusUpdate();
    }
    else if (i == 0x3F)
    {
        LED_OFF();
        if (SistemState == pps_system)
            Button2Check();
    }
}


inline void Button1Check(void)
{
	static	U8	button1Last, but1Time = 0;
    U8 but1 = BUTTON1;
	if (button1Last != but1)
	{
		button1Last = but1;
        but1Time = 1;
	}
    else if (but1Time)
    {
        if (but1)
        {
            Button_1 = but1Time;
            but1Time++;
        }
        else
        {
            but1Time = 0;
            Button_1 = but1;
        }
    }
}

inline void Button2Check(void)
{
	static	U8	button2Last, but2Time = 0;
    U8 but2 = !BUTTON2;

    if (button2Last != but2)
    {
        button2Last = but2;
        but2Time = 1;
    }
    else if (but2Time)
    {
        if (but2)
        {
            Button_2 = but2Time;
            but2Time++;
        }
        else
        {
            but2Time = 0;
            Button_2 = but2;
        }
    }
}

void GetTimeAscii(char* time)
{
	if (SystemTime.hours > 9)
		*time++ = (SystemTime.hours / 10) + '0';
	else
		*time++ = SPACE;
	*time++ = (SystemTime.hours % 10) + '0';
	*time++ = ':';
	*time++ = (SystemTime.minutes / 10) + '0';
	*time++ = (SystemTime.minutes % 10) + '0';
	*time++ = ':';
	*time++ = (SystemTime.seconds / 10) + '0';
	*time++ = (SystemTime.seconds % 10) + '0';
}

void GetDateAscii(char* date)
{
	if (SystemTime.day > 9)
		*date++ = (SystemTime.day / 10) + '0';
	else
		*date++ = SPACE;
	*date++ = (SystemTime.day % 10) + '0';
	*date++ = '.';
	*date++ = (SystemTime.mon / 10) + '0';
	*date++ = (SystemTime.mon % 10) + '0';
	*date++ = '.';
	*date++ = '2';
	*date++ = '0';
	*date++ = (SystemTime.year / 10) + '0';
	*date++ = (SystemTime.year % 10) + '0';
	*date++ = '\0';
}
*/
