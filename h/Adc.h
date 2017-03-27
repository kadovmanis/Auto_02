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
	power_BatteryLow,
	power_BatteryOk,
	power_BatteryMiss,
	power_External
} POWER_STATE;

extern	volatile	BATTERY_LEVEL	AdcPower, AdcBattery;
extern	volatile	POWER_STATE		PowerState;
extern	volatile	U16				Power, Battery;

//extern volatile	U8	ProcPowerReduction;
//extern const	U16	DcDcReductionSteps[15];

void	AdcInit			(void);
void	Tcp_AdcPacket	(void);

int		Adc_TestVal		(void);

#endif

