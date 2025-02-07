#include	"Adc.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Tcp.h"
#include	"Gsm.h"
#include	"Gps.h"
#include	<string.h>
#include	<stdio.h>
#ifdef		UART4_WIFI
 #include	"Wifi.h"
#else
 #define	Wifi_Off()
 #define	WIFI_EN		0
#endif

//int test = 0, test1 = 0, test2 = 0;

typedef struct
{
	U16	mn;
	U16	mx;
	U16	t;			// time		previous
	U16	min;		// min		of measured values
	U16	center;		// average	of measured values
	U16	max;		// max		of measured values
	U16	time;		// time		measured
	S16	drift;
	S8	upDwn;
} ADC_INPUT;

ADC_INPUT					Ext[3] = {{0}, {0}, {0}};
static		U16				CurrLimit	= 212;

volatile	POWER_STATE		PowerState = power_NoPower;
//volatile	BATTERY_LEVEL	AdcPower, AdcBattery;
volatile	U16				Power = 0, Battery = 0;
volatile	U16				Ext1 = 0, Ext2 = 0, Ext3;
volatile	S16				Cnt = 255;
static		S16				CalibrationBat, CalibrationPow;

#define		DC_LEVEL_3_80	567
//const		U16		DcDcReductionSteps[15] = {DC_LEVEL_3_80, 586, 607, 629, 651, 673, 697, 721, 747, 773, 800, 828, 857, 887, 0};
const		U16		DcDcReductionSteps[15] = {DC_LEVEL_3_80, 586, 604, 623, 643, 664, 686, 709, 733, 758, 784, 811, 818, 847, 0};
//													17	 18	  19   20	21	 22	  23   24	25	 26	  27   28	29
void AdcInit(void)
{
	CalibrationBat	= DataFromFlash.CalibreBat;
	CalibrationPow	= DataFromFlash.CalibrePow;
	//AD1CON1 Register
	AD1CON1 = 0b0000000011100100;	// AD1CON1 Register
			//	|||||||||||||||+	-- DONE: A/D Conversion Status bit
			//	||||||||||||||+-	-- SAMP: A/D Sample Enable bit
			//	|||||||||||||+--	-- ASAM: A/D Sample Auto-Start bit
			//	|||||||||||++---	-- Unimplemented
			//	||||||||+++-----	-- SSRC<2:0>: Conversion Trigger Source Select bits (auto-convert)
			//	||||||++--------	-- FORM<1:0>: Data Output Format bits
			//	|||+++----------	-- Unimplemented
			//	||+-------------	-- ADSIDL: Stop in Idle Mode bit
			//	|+--------------	-- Unimplemented
			//	+---------------	-- ADON: A/D Operating Mode bit
    //AD1CON2 Register
	AD1CON2 = 0b0000010000011000;	// AD1CON2 Register
			//	|||||||||||||||+	-- ALTS: Alternate Input Sample Mode Select bit
			//	||||||||||||||+-	-- BUFM: Buffer Mode Select bit
			//	|||||||||+++++--	-- SMPI<4:0>: Sample/Convert Sequences Per Interrupt Selection bits
			//	||||||||+-------	-- BUFS: Buffer Fill Status bit (valid only when BUFM = 1)
			//	||||||++--------	-- Unimplemented
			//	|||||+----------	-- CSCNA: Scan Input Selections for the CH0+ S/H Input for MUX A Input Multiplexer Setting bit
			//	||||+-----------	-- Unimplemented
			//	|||+------------	-- Reserved
			//	+++-------------	-- VCFG<2:0>: Voltage Reference Configuration bits - VR+ = AVDD; VR- = AVSS (default)

	// AD1CON3 Register
	AD1CON3 = 0b0001000000000001;	// AD1CON3 Register
			//	||||||||++++++++	-- ADCS<7:0>: A/D Conversion Clock Select bits
			//	|||+++++--------	-- SAMC<4:0>: Auto-Sample Time bits
			//	|++-------------	-- Reserved
			//	+---------------	-- ADRC: A/D Conversion Clock Source bit

	// AD1CSSL - A/D Input Scan Select Register (low)
    AD1CSSL				= 0x003F;	// AN0-AN5 is selected for input scan
	// AD1CSSH - A/D Input Scan Select Register (high)
	_CSSL26				= 1;		// Internal core voltage (VCAP) is selected for input scan

	_AD1IF	= 0;		// Clear the ADC1 Interrupt Flag
	_AD1IE	= 1;		// Enable ADC1 Interrupt
	_AD1IP	= 4;		// Set	  ADC  Interrupt priority
	_ADON	= 1;		// A/D Converter is On
}

#define	BATTERY_0_VOLT_LEVEL			13			// 
#define	BATTERY_100_PERCENT_LEVEL		4200		// 4.2V			4200 - 2600 = 1600; 1600 / 100% = 16
#define	BATTERY_0___PERCENT_LEVEL		2560//2592		// 2.6V - (1% / 2)
#define	BATTERY_PERCENTS(V_x_1000)		((V_x_1000 - BATTERY_0___PERCENT_LEVEL) >> 4)	// ((Volts x 1000) - 0%_level ) / 1%_Value ( / 16)

#define	POWER_ADD_VAL					175			// Power step = 20.5 (20.5 x 2 = 41)
#define	POWER_MULTIPLIER				50			// (20.5 * 25 * 25) >> 10 = 1,0009765625
#define	POWER_DIVIDE_SHIFT				10
#define	POWER_DIVIDE_REMAINDER			0x3FF
#define	POWER_MW_CUT					25			// 0.999 * 1024 = 1 022,976

/* Battery
// 3.59V    535
// 4.01		585
// 4.05		590
// 4.15		605
// 4.20		612
*/

inline	void ADC_Dc_Update		(void);
inline	void ADC_BatteryLevel	(void);
inline	void ADC_PowerLevel		(void);
inline	void ADC_ExternLevel	(void);
inline	void ADC_PowerControll	(void);
inline	void ADC_Test			(void);
//	*********************************************************************
//	*							ADC INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)	//	*
{																	//	*
	_AD1IF	= 0;				// Clear the ADC1 Interrupt Flag		*

	ADC_Dc_Update();
	ADC_BatteryLevel();
	ADC_PowerLevel();
#if (TEST == TEST_ADC)
	ADC_ExternLevel();
#endif
	if (--Cnt < 0)
	{
		Cnt = 1632;
		ADC_PowerControll();
	}
//	ADC_Test();
}																	//	*

inline void ADC_Dc_Update(void)
{
	if ((Battery > 3800)				||
		((!GSM_IE) && (Battery > 3300))	)	// Don't run DcUp if gsm disabled
		OC1R = 0;
	else
	{
		register U16 adc		= AN_V18;
		register U16 step;
		if (adc > AN_V18_VALUE)
		{
			register U8 procPowRed = (U8)((adc - 562) / 21);
			step		= DcDcReductionSteps[procPowRed];
		}
		else
			step		= DC_LEVEL_3_80;
		adc = AN_DC_DC;
		if		(adc > step)	{	if (OC1R)				OC1R--;	}			//  567
		else if	(adc < step)	{	if (OC1R < DC_DUTY_MAX)	OC1R++;	}
	}
/*	pirms lietot - JATESTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if		(adc < step)
	{
		if (!OC1R)						OC1R++;
		else if (PR2 > DC_PERIOD_MIN)	PR2--;
	}
	else if (adc > step)
	{
		if (PR2 < DC_PERIOD)			PR2++;
		else							OC1R = 0;
	}
*/
}
/*
// 3.59V    535
// 4.01		585
// 4.05		590
// 4.15		605
// 4.20		612
*/
inline void ADC_BatteryLevel(void)
{
	static	 U16	prev;
	static	 U16	valMax = 0, valMin = 0xFFFF;
	static	 S8		drift = 128;
	register U16	val = AN_BAT;
	
	if (valMax < val)		valMax = val;
	if (valMin > val)		valMin = val;
	if (!Cnt)
	{
		val = valMax + valMin;
		if		(val > prev)	drift++;
		else if	(val < prev) 	drift--;
		prev = val;

		if (drift > 2)
		{
			drift -= 2;
			Battery = (val < 163)?	0 : (((val - 162) << 2) + CalibrationBat); 
		}
		else if (drift < -2)
		{
			drift += 2;
			Battery = (val < 163)?	0 : (((val - 162) << 2) + CalibrationBat); 
		}
	#if (TEST == TEST_ADC)
//		test1 = Battery;
//		test1 = valMax;
//		test2 = valMin;
	#endif
		valMax	= 0;
		valMin	= 0xFFFF;
	}
}

/* Power
// 456			22.00										/					465.6		9.6
// 287			14.04									/	169	7.96	0.0471
// 248-251		12.17		12V = 249 (12 * 20 = 240)	/ 	38	1.87	0.0492	257.566	   ~9.6
// 224-227		11.13		23	1.04	45.2			/ 	24	1.04	0.0433
// 206-208		10.26		19	870		45.7			/ 	18	0.87	0.0483
// 185-187		9.26		21							/ 	21	1		0.0476
// 166-168		8.34		19							/ 	19	0.92	0.0484
// 103			5.32		146	6.94		353			/ 	63	3.02	0.0479	112.59		9.592
//													_______ 353	16.68	0.04725
// ADD = 16, step 21
// 456	22	461		22.00	457		| 902	_________________ +5 step 21
// 435	21	440		20.84	430	23	| 861
// 414	20	419					21	| 820
// 393	19	398		19.10	393	21	| 779
// 393	18	377		18.14	375	18	| 737
// 372	17	356		16.96	349	22	| 597
// 351	16	335		16.14	333	19	| 656
// 330	15	314		14.97	308	21	| 615
// 309	14	293		14.05	288	20	| 574	____________________ -6 step 20
// 288	13	272					20	| 533
// 267	12	251		12.18	249	19	| 492
// 246	11	230		11.10	226	23	| 451	____________________ -6 step 20
// 225	10	209		10.23	207	19	| 410
// 204	 9	188		9.22	185	22	| 369
// 183	 8	167		8.31	167	18	| 328
// 162	 7	146		7.30	146	21	| 287
// 141	 6	125					22	| 246
// 120	 5	104		5.29	103	21	| 205
// 99	 4	83						| 164
*/
inline void ADC_PowerLevel (void)
{
	static	 U16	prev, valMax, valMin;
	static	 S8		drift = 128;
	register U16	val = AN_POW;
	
	if (valMax < val)		valMax = val;
	if (valMin > val)		valMin = val;
	if (!Cnt)
	{
		val = valMax + valMin;
		if		(val > prev)	drift++;
		else if	(val < prev) 	drift--;
		prev = val;

		if (drift > 2)
		{
			drift -= 2;
			register U32 a = val + 19;	// 9.6 x2
			a *= 189;
			Power = ((SHIFT_DIVIDE____8(a)) + CalibrationPow);
		}
		else if (drift < -2)
		{
			drift += 2;
			register U32 a = val + 19;	// 9.6 x2
			a *= 189;
			Power = ((SHIFT_DIVIDE____8(a)) + CalibrationPow);
		}
	#if (TEST == TEST_ADC)
//		test = Power;
//		test1 = valMax;
//		test2 = valMin;
	#endif
		valMax	= 0;
		valMin	= 0xFFFF;
	}
}

#define	ADC_DRIFT_1	2

inline	void ADC_ExternLevel	(void)
{
	register U16 i;
	for (i = 0; i < 3; i++)
	{
		register U16	val = (!i)?	AN_EXT1 : ((i == 1)?	AN_EXT2 : AN_EXT3);
		if (val < Ext[i].mn)	Ext[i].mn = val;
		if (val > Ext[i].mx)	Ext[i].mx = val;
		
		if (val > Ext[i].center)
		{
			if ((Ext[i].upDwn < 10) && (++Ext[i].upDwn == 10))
			{
				Ext[i].time	= GetTimeSinceMs(Ext[i].t);
				Ext[i].t	= GetTicsMs();
			}
		}
		else
			Ext[i].upDwn = 0;
		
		if (!Cnt)
		{
			if (i == 1)
			{
				static	 U16 cntr = 0;
				register U16 a = ((Ext[i].mn + Ext[i].mx) + 1) >> 1;
				register U16 d = (a > Ext[i].center)?	(a - Ext[i].center) : (Ext[i].center - a);
				if ((d < ADC_DRIFT_1) && (cntr))
				{
					cntr += a;
					a = cntr >> 6;
					cntr -= a;
				}
				else
				{
					cntr = (a << 6);
					Ext[i].center = a;
				}
			}
			else
				Ext[i].center	= ((Ext[i].mn + Ext[i].mx) + 1) >> 1;
			Ext[i].min		= Ext[i].mn;
			Ext[i].max		= Ext[i].mx;
			Ext[i].mn		= 0xFFFF;
			Ext[i].mx		= 0;
		}
	}
}

inline void ADC_PowerControll (void)
{
	register POWER_STATE	newState = power_NoPower;
	static	U16				pow = 0, bat = 0;
	
	#if (TEST == TEST_ADC)
//		test	= Ext1;
//		test1	= Ext2;
//		test2	= Ext3;
	#endif

	register U16 diff = (Power > pow)?	(Power - pow)	: (pow - Power);
	if (diff > 300)						// Power	+/- 0.3V ?
	{									// send changes & test changes
		pow = Power;
		bat = Battery;
		FL_POWER_CHANGES = 1;
	}
	else
	{
		diff = (Battery	> bat)?	(Battery - bat) : (bat - Battery);
		if (diff < 100)
			return;
		else if (diff > 300)			// Battery	+/- 0.3V ?
		{
			bat = (bat + Battery) >> 1;
			FL_POWER_CHANGES = 1;		// send changes & test changes
		}
	}


	if		(Power		>  4800)		newState = power_External;
	else								newState = power_BatteryOk;

	if		(Battery	>  4200)		newState++;		// inadequate battery level !!!
	else if	(Battery	<  2900)		newState -= 2;	// battery too low - switch off or charge
	else if	(Battery	<  3400)		newState--;		// battery too low - don't use gsm

	if	(PowerState != newState)				// Power situation changed
	{
		PowerState = newState;
		switch (newState)
		{
		case power_BatteryLow:
		case power_BatteryCharge:
			// TODO - send battery low alarm and switch off GSM
			break;
		case power_ExternalCharge:
			if (GSM_IE)							Gsm_Off();
		#if	(HARDWARE == HW_AUTO)
			if (!GPS_POWER)						Gps_On();
			if (WIFI_EN)						Wifi_Off();
		#endif
			break;
		case power_BatteryOk:
		case power_BatteryError:
		case power_External:
		case power_BatteryMiss:
		#if	(HARDWARE == HW_AUTO)
			if (!GPS_POWER)						Gps_On();
//			if ((!GSM_IE) && (FLASH_FLAG_GSM))	Gsm_On();
			if (!GSM_IE)						Gsm_On();
		#endif
			break;
		case power_NoPower:
		default:
			Gsm_Off();
			Wifi_Off();
			GPS_POW_OFF();
			Reset();
			break;						// TODO: Switch off device
		}
	}
}

inline	void ADC_Test			(void)
{
	static	U16	pow = 0, bat = 0;
	if ((!FL_WIFI_OFFLINE)					&&
		(!FL_POWER_CHANGES)					&&
		((pow	!= Power) ||
		 (bat	!= Battery))	)
	{
		pow	= Power;
		bat	= Battery;
		FL_POWER_CHANGES = 1;
	}
}

void	Tcp_AdcPacket	(void)
{
	char str[128];

//	register U16 val = Battery >> 3;
//	AdcBattery.V		= val >> POWER_DIVIDE_SHIFT;
//	AdcBattery.mV		= val & POWER_DIVIDE_REMAINDER;
//	if (AdcBattery.mV > 999)	AdcBattery.mV = 999;
//	AdcBattery.level	= BATTERY_PERCENTS((AdcBattery.V * 1000) + AdcBattery.mV);

//	sprintf(str, "Bat: %d.%02dV (%d), Pow: %d.%02dV (%d)", AdcBattery.V, AdcBattery.mV, Battey, AdcPower.V, AdcPower.mV, Power);
//	sprintf(str, "Bat: %u.%03u (%u%%), Pow: %d", AdcBattery.V, AdcBattery.mV, AdcBattery.level, Power);
	sprintf(str, "Bat: %d, Pow: %d", Battery, Power);
	Tcp_SendText(str);
	FL_POWER_CHANGES = 0;
}

U16		Adc_GetPowerState	(void)
{
	return (U16)PowerState;
}

U16		Adc_GetPower		(void)
{
	return Power;
}

U16		Adc_GetBattery		(void)
{
	return Battery;
}

U16		Adc_GetCoreV		(void)
{
	return AN_V18;
}

U16		Adc_GetDcDc			(void)
{
	return AN_DC_DC;
}

void	Adc_GetAllVal	(char* txt)
{
	sprintf(txt, "Adc: %u %u %u\tdiff: %u\tt: %u ",
//	sprintf(txt, "Adc: %u %u %u, %u %u %u, %u %u %u\tt: %u %u %u ",
//				Ext[0].min, Ext[0].center, Ext[0].max,
				Ext[1].min, Ext[1].center, Ext[1].max, (Ext[1].max - Ext[1].min), Ext[1].time);
//				Ext[2].min, Ext[2].center, Ext[2].max,
//				Ext[0].time, Ext[1].time, Ext[2].time);
//				0);

	//	 Adc: 177 181 524 528 204 845	t:  20 1 20
}

/*
 * 1A	0.066V		ASC712
 * 1A	0.044V		ADC input (10k / 20k)
 * 
 * 1adc	~0,003125V
 * 1A	~14.08adc
 * 1adc	~71,022727273mA
 * 71,022727273 * 128 = 9090.9
 * (1adc * 9091) / 128 = 71.0234375

 * 117	2.610
 * 118	2.644
 * 119	2.645
	23.261261261
	4
 */
// 2.582 A		114
#define	I_NULL_VAL	526
#define	ADC_DRIFT		5
#define	ADC_DRIFT_OFF	3
#define	MA_ROUND		2
#define	MA_MUL			92
#define	MA_MAX_VAL		(((65536 - MA_ROUND) / MA_MUL) + ADC_DRIFT_OFF)
#define	ADC_TO_MA(X)	((((X - ADC_DRIFT_OFF) * MA_MUL) + MA_ROUND) >> 2)
void	Adc_GetVolt	(char* txt)
{
	register U16 ac = (((Ext[2].max - Ext[2].min + 5) * 23)) >> 6;	// rounding
	register U16 hz = 1000 / Ext[2].time;
	sprintf(txt, "Ac %uV %uHz ", ac, hz);
}

void	Adc_GetAmper	(char* txt)
{
	register U16 amp, diff	= (Ext[1].max - Ext[1].min);
	if		(diff <= ADC_DRIFT)	amp = 0;				// less than drift
	else if	(diff > MA_MAX_VAL)	amp = 20000;			// more than 16.3A (not fit in U16)
	else						amp = ADC_TO_MA(diff);	// mA = adcDiff * 23.261.. (* 93 / 4)

	if (diff > CurrLimit)
	{
		sprintf(txt, "OwerCurrent!!! ");
		#ifdef	EXT_OUT2
			EXT_OUT2 = 0;
		#endif
	}
	else
		sprintf(txt, "%5dmA (%u) ", amp, diff);
}

void	Adc_GetPowBat	(char* txt)
{
	sprintf(txt, "Bat: %2u,%03uV \n\rPow: %2u,%03uV  ",
		 Battery / 1000, Battery % 1000, Power / 1000, Power % 1000);
}

void	Adc_GetLimit	(char* txt)
{
	sprintf(txt, "limit: %dmA (%u) ", ADC_TO_MA(CurrLimit), CurrLimit);
}
void	Adc_SetLimit	(U16 limit)
{
	CurrLimit = limit;
}

int	Adc_TestVal1	(void)
{
//	return Ext_1.min;
	return Power;
}

int	Adc_TestVal2	(void)
{
//	return Ext_1.center;
	return Battery;
}

int	Adc_TestVal3	(void)
{
//	return Ext_1.max;
	return 0;
}

int	Adc_TestVal4	(void)
{
//	return Ext_1.time;
	return 0;
}
