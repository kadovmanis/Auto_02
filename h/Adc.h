#ifndef	_adc_h_
#define	_adc_h_

#include	"Typedef.h"

#define	AN_V18_VALUE	572

typedef struct
{
	U8	level;	// in percents (%)
	U8	V;
	U16	mV;
} BATTERY_LEVEL;

typedef enum
{
	power_NoPower = 0,
	power_BatteryLow,		// Battery power < 3.3V don't run gsm module
	power_BatteryOk,		// Run on Battery
	power_BatteryError,		// Battery power < inadequate > (too hight)
	power_BatteryCharge,	// Ext Power presented, Battery power too small
	power_ExternalCharge,	// Ext Power presented, Battery power too small
	power_External,			// Ext Power presented, Battery Ok
	power_BatteryMiss		// Ext Power presented, Battery power < inadequate > (too hight)
} POWER_STATE;

//extern	volatile	BATTERY_LEVEL	AdcPower, AdcBattery;
extern	volatile	POWER_STATE		PowerState;
extern	volatile	U16				Power, Battery;

//extern volatile	U8	ProcPowerReduction;
//extern const	U16	DcDcReductionSteps[15];

void	AdcInit			(void);
void	Tcp_AdcPacket	(void);

U16		Adc_GetPowerState(void);
U16		Adc_GetPower	(void);
U16		Adc_GetBattery	(void);
U16		Adc_GetCoreV	(void);
U16		Adc_GetDcDc		(void);

void	Adc_GetAllVal	(char* txt);
void	Adc_GetVolt		(char* txt);
void	Adc_GetAmper	(char* txt);
void	Adc_GetPowBat	(char* txt);
void	Adc_GetLimit	(char* txt);
void	Adc_SetLimit	(U16 limit);

int		Adc_TestVal1	(void);
int		Adc_TestVal2	(void);
int		Adc_TestVal3	(void);
int		Adc_TestVal4	(void);

#endif

