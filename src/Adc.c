#include	"Adc.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Tcp.h"
#include	<string.h>
#include	<stdio.h>

int test = 0, test1 = 0, test2 = 0;

volatile	POWER_STATE		PowerState = power_NoPower;
volatile	BATTERY_LEVEL	AdcPower, AdcBattery;
volatile	U16				Power = 0, Battery = 0;
volatile	S16				Cnt = 255;

#define		DC_LEVEL_3_80	567
//const		U16		DcDcReductionSteps[15] = {DC_LEVEL_3_80, 586, 607, 629, 651, 673, 697, 721, 747, 773, 800, 828, 857, 887, 0};
const		U16		DcDcReductionSteps[15] = {DC_LEVEL_3_80, 586, 604, 623, 643, 664, 686, 709, 733, 758, 784, 811, 818, 847, 0};
//													17	 18	  19   20	21	 22	  23   24	25	 26	  27   28	29
void AdcInit(void)
{
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
inline	void ADC_PowerControll	(void);
inline	void ADC_Test			(void);
//	*********************************************************************
//	*							ADC INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)	//	*
{																	//	*
	_AD1IF	= 0;				// Clear the ADC1 Interrupt Flag		*
	if (--Cnt < 0)		Cnt = 1632;

	ADC_Dc_Update();
	ADC_BatteryLevel();
	ADC_PowerLevel();
	ADC_PowerControll();
//	ADC_Test();
}																	//	*

inline void ADC_Dc_Update(void)
{
	if (Battery > 3800)
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

// 3.59V    535
// 4.01		585
// 4.05		590
// 4.15		605
// 4.20		612

inline void ADC_BatteryLevel(void)
{
//	static	 U16	val_sum = 0;
	static	 U16	prev;
	static	 U16	valMax, valMin;
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
			val >>= 1;
			Battery = (val < 82)?	0 : ((val - 81) << 3);
		}
		else if (drift < -2)
		{
			drift += 2;
			val >>= 1;
			Battery = (val < 82)?	0 : ((val - 81) << 3);
		}
//		val_sum += (!val_sum)?	(val << 6) : valMin;
//		val = val_sum >> 6;
//		val_sum -= val;
//		Battery = (val < 82)?	0 : ((val - 81) << 3);

//			test1 = valMax;
//			test2 = valMin;

		valMax	= 0;
		valMin	= 0xFFFF;
//		test = Battery;
	}

//	static	U16 val_sum = 0;
//	static  S16 cnt     = 0;
//	register U16 val = AN_BAT;

//	val_sum += (!val_sum)?	(val << 6) : val;
//	val = val_sum >> 6;
//	val_sum -= val;
//
//	Battery = (val < 82)?	0 : ((val - 81) << 3);
}

/* Power
// 456			22.00										/ 		
// 287			14.04										/ 	 
// 248-251		12.17		12V = 249 (12 * 20 = 240)		/ 	
// 224-227		11.13		23	1.04	45.2				/ 	
// 206-208		10.26		19	870		45.7				/ 	
// 185-187		9.26		21								/ 	
// 166-168		8.34		19								/ 	
// 103			5.32		146	6.94		353				/ 	
// 
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
	static	 U16	prev;
	static	 U16	valMax, valMin;
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
			val >>= 1;
			Power = val;
//			Power = (val < 82)?	0 : ((val - 81) << 3);
		}
		else if (drift < -2)
		{
			drift += 2;
			val >>= 1;
			Power = val;
//			Power = (val < 82)?	0 : ((val - 81) << 3);
		}
		valMax	= 0;
		valMin	= 0xFFFF;

		test = Power;
		test1 = valMax;
		test2 = valMin;
	}


/*
	static	 U16 val_sum = 0, cnt = 0;
//	static	 S16 cnt = 0;
	register U16 val = AN_POW;

	val_sum += (!val_sum)?	(val << 6) : val;
	val = val_sum >> 6;
	val_sum -= val;

	if (Power == val)
		cnt = 0;
	else if (++cnt & 0x1000)
	{
		Power = val;
		cnt = 0;
	}
*/
/*	if (val < Power)
	{
		if (cnt > 0)
			cnt = 0;
		else if (--cnt < -32000)
		{
			cnt = 0;
			Power = val;
		}
	}
	else if (val > Power)
	{
		if (cnt < 0)
			cnt = 0;
		else if (++cnt > 32000)
		{
			cnt = 0;
			Power = val;
		}
	}
*	
	static U16 val_sum = 0, val_min = 0xFFFF, val_max = 0, cnt = 64, cnt1 = 64;
	register U16 val = AN_POW;
	if (val_min > val)
		val_min = val;
	if (val_max < val)
		val_max = val;
//	val_sum += val;

	if (!--cnt1)
	{
		val_sum += (val_min + val_max) >> 1;
		val_min = 0xFFFF;
		val_max = 0;
		cnt1 = 64;
		if (!--cnt)
		{
			Power = (val_sum >> 6);
			cnt = 64;
			val_sum = 0;
		}
	}
*
	static U16 val_last = 0xFFFF, val_sum = 0, cnt = 0;
	register U16 val = AN_POW;
	if (val < val_last)
	{
		if (val_sum)
		{
			Power = val_sum / cnt;
			val_sum = 0;
		}
		cnt = 0;
	}
	else
	{
		val_sum += val;
		cnt++;
	}
	val_last = val;


*
	static U16 val_1 = 0, val_2 = 0;
	val_1 += (!val_1)?	(AN_POW << 8) : AN_POW;
	val_2 += (!val_2)?	val_1 : (val_1 >> 8);
	Power = val_2 >> 8;
	val_1 -= Power;
	val_2 -= Power;
*/

/*	
	static		U16 adcSum = 0, pow = 0;
	register	U16	val		= AN_POW;				// value from ADC buff

	if (adcSum)		adcSum += val;					// equalize 64 times (rounding / remove shaking)
	else			adcSum  = val		<< 6;		// x64
	val						= adcSum	>> 6;		// /64
	adcSum -=  (val);								// 

//	if (pow != (val >> 8))
	{
		pow = (val >> 8);
		Power			= ((val * POWER_MULTIPLIER) + POWER_ADD_VAL);
		val				= ((val * POWER_MULTIPLIER) + POWER_ADD_VAL);
		AdcPower.V		=  val >> POWER_DIVIDE_SHIFT;
		val			   &=  POWER_DIVIDE_REMAINDER;
		val				= (val > POWER_MW_CUT)? (val - POWER_MW_CUT): 0;
		AdcPower.mV		=  val;
		AdcPower.level	= (AdcPower.V * 10) + ((val * 10) >> POWER_DIVIDE_SHIFT);
	}
*/
}

inline void ADC_PowerControll (void)
{
	register POWER_STATE	newState = power_NoPower;

	if		(AdcPower.level		> 45)	newState = power_External;	// External power	> 4.5V
	else if (AdcBattery.level	> 100)	newState = power_BatteryMiss;	// Battery level	> 100% (>4.2V)		0% = 2.6V, 1% step = 16mV, 
	else if (AdcBattery.level	> 50)	newState = power_BatteryOk;	// Battery level	> 43% (~3.3V)		0% = 2.6V, 1% step = 16mV, 
	else if (AdcBattery.level	> 13)	newState = power_BatteryLow;	// Battery level	> 13% (~2.8V)
//	else								newState = power_NoPower;

	if	(PowerState != newState)				// Power situation changed
	{
		PowerState = newState;
		switch (newState)
		{
		case power_External:
		case power_BatteryMiss:
		case power_BatteryOk:
//			if ((!GsmTimerEnabled) && (FLASH_FLAG_GSM))		Gsm_On();
//			if (!GPS_POWER)									Gps_On();
			break;
		case power_BatteryLow:
		case power_NoPower:
		default:
//			Gsm_Off();
//			GPS_POW_OFF();
			break;								// TODO: Switch off device
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

int	Adc_TestVal		(void)
{
	int ret = test;
	test = 0;
	return ret;
}

int	Adc_TestVal1	(void)
{
	return test1;
}

int	Adc_TestVal2	(void)
{
	return test2;
}
