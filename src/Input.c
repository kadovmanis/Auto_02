#include	"Input.h"
#include	"Ports.h"
#include	"Gps.h"
#include	"Gsm.h"

#define	ACTIVE_TIME_100MS	6000						// 10min (600s)

#define	CN1_PORTS	0b0000001000000000
/*					  |||||||||||||||+-	CN0IE	C14
					  ||||||||||||||+--	CN1IE	C13
					  |||||||||||||+---	CN2IE	B0
					  ||||||||||||+----	CN3IE	B1
					  |||||||||||+-----	CN4IE	B2
					  ||||||||||+------	CN5IE	B3
					  |||||||||+-------	CN6IE	B4
					  ||||||||+--------	CN7IE	B5
					  |||||||+---------	CN8IE	G6
					  ||||||+----------	CN9IE	G7		GSM_RI
					  |||||+-----------	CN10IE	G8
					  ||||+------------	CN11IE	G9
					  |||+-------------	CN12IE	B15
					  ||+--------------	CN13IE	D4
					  |+---------------	CN14IE	D5
					  +----------------	CN15IE	D6				*/
#define	CN2_PORTS	0b0000000000000000
/*					  |||||||||||||||+-	CN16IE	D7
					  ||||||||||||||+--	CN17IE	F4
					  |||||||||||||+---	CN18IE	F5
					  ||||||||||||+----	CN19IE	
					  |||||||||||+-----	CN20IE	
					  ||||||||||+------	CN21IE	
					  |||||||||+-------	CN22IE	C15
					  ||||||||+--------	CN23IE	C12
					  |||||||+---------	CN24IE	B6
					  ||||||+----------	CN25IE	B7
					  |||||+-----------	CN26IE	B8
					  ||||+------------	CN27IE	B9
					  |||+-------------	CN28IE	B10
					  ||+--------------	CN29IE	B11
					  |+---------------	CN30IE	B12
					  +----------------	CN31IE	B13				*/
#define	CN3_PORTS	0b0000000000000000
/*					  |||||||||||||||+-	CN32IE	B14
					  ||||||||||||||+--	CN33IE	B15
					  |||||||||||||+---	CN34IE	
					  ||||||||||||+----	CN35IE	
					  |||||||||||+-----	CN36IE	
					  ||||||||||+------	CN37IE	
					  |||||||||+-------	CN38IE	
					  ||||||||+--------	CN39IE	
					  |||||||+---------	CN40IE	
					  ||||||+----------	CN41IE	
					  |||||+-----------	CN42IE	
					  ||||+------------	CN43IE	
					  |||+-------------	CN44IE	
					  ||+--------------	CN45IE	
					  |+---------------	CN46IE	
					  +----------------	CN47IE					*/
#define	CN4_PORTS	0b1101000000111000
/*					  |||||||||||||||+-	CN48IE	
					  ||||||||||||||+--	CN49IE	D0
					  |||||||||||||+---	CN50IE	D1
					  ||||||||||||+----	CN51IE	D2	EN_POW
					  |||||||||||+-----	CN52IE	D3	CHARGE_STAT
					  ||||||||||+------	CN53IE	D8	SW1
					  |||||||||+-------	CN54IE	D9
					  ||||||||+--------	CN55IE	D10
					  |||||||+---------	CN56IE	D11
					  ||||||+----------	CN57IE	
					  |||||+-----------	CN58IE	E0
					  ||||+------------	CN59IE	E1
					  |||+-------------	CN60IE	E2	GPS_TIME_INT
					  ||+--------------	CN61IE	E3
					  |+---------------	CN62IE	E4	GPS_POWER
					  +----------------	CN63IE	E5	SENS2		*/
#define	CN5_PORTS	0b0000000010000011
/*					  |||||||||||||||+-	CN64IE	E6	SENS1
					  ||||||||||||||+--	CN65IE	E7	SW2
					  |||||||||||||+---	CN66IE	
					  ||||||||||||+----	CN67IE	
					  |||||||||||+-----	CN68IE	F0
					  ||||||||||+------	CN69IE	F1
					  |||||||||+-------	CN70IE	
					  ||||||||+--------	CN71IE	F3	USB_ID
					  |||||||+---------	CN72IE	
					  ||||||+----------	CN73IE	
					  |||||+-----------	CN74IE	
					  ||||+------------	CN75IE	
					  |||+-------------	CN76IE	
					  ||+--------------	CN77IE	
					  |+---------------	CN78IE	
					  +----------------	CN79IE					*/
#define	CN6_PORTS	0b0000000000000000
/*					  |||||||||||||||+-	CN80IE	
					  ||||||||||||||+--	CN81IE	
					  |||||||||||||+---	CN82IE	
					  ||||||||||||+----	CN83IE	G2
					  |||||||||||+-----	CN84IE	G3
					  ||||||||||+------	CN85IE	
					  |||||||||+-------	CN86IE	
					  ||||||||+--------	CN87IE	
					  |||||||+---------	CN88IE	
					  ||||||+----------	CN89IE	
					  |||||+-----------	CN90IE	
					  ||||+------------	CN91IE	
					  |||+-------------	CN92IE	
					  ||+--------------	CN93IE	
					  |+---------------	CN94IE	
					  +----------------	CN95IE					*/

typedef union
{
	U16 word;
	struct
	{
		U16 sens1		:1;		// SENS1
		U16 sens2		:1;		// SENS2
		U16 gsm_ri		:1;		// GSM_RI
		U16 charge_stat	:1;		// CHARGE_STAT
		U16 usb_5v		:1;		// USB_5V
		U16 usb_id		:1;		// USB_ID
		U16 sw1			:1;		// SW1
		U16 sw2			:1;		// SW2
		U16 gps_time	:1;		// GPS_TIME_INT
		U16 gps_power	:1;		// GPS_POWER
		U16 en_pow		:1;		// EN_POW
		U16 flag_7		:1;		// 
		U16 flag_8		:1;		// 
		U16 flag_9		:1;		// 
		U16 flag_10		:1;		// 
		U16 flag_11		:1;		// 
	};
} INPUTS;

static	INPUTS	Input;
static	U16		Sw1_cnt = 0, Sw2_cnt = 0, ActiveTime;

void InputInit	(void)
{
	#if (CN1_PORTS > 0)
		CNEN1 = CN1_PORTS;
	#endif
	#if (CN2_PORTS > 0)
		CNEN2 = CN2_PORTS;
	#endif
	#if (CN3_PORTS > 0)
		CNEN3 = CN3_PORTS;
	#endif
	#if (CN4_PORTS > 0)
		CNEN4 = CN4_PORTS;
	#endif
	#if (CN5_PORTS > 0)
		CNEN5 = CN5_PORTS;
	#endif
	#if (CN6_PORTS > 0)
		CNEN6 = CN6_PORTS;
	#endif
}


inline void SwitchTimer (void)
{
	if ((Input.sens1 != SENS1) || (Input.sens2 != SENS2))
	{
		Input.sens1 = SENS1;
		Input.sens2 = SENS2;
		ActiveTime	= ACTIVE_TIME_100MS;
	}

	if (Input.gsm_ri != GSM_RI)
	{
		Input.gsm_ri = GSM_RI;
		ActiveTime	= ACTIVE_TIME_100MS;
		// Wake
		// GSM Ring
	}

	if (Input.charge_stat != CHARGE_STAT)
	{
		Input.charge_stat = CHARGE_STAT;
		// Charge status changes
	}
	if (Input.usb_5v != USB_5V)
	{
		Input.usb_5v = USB_5V;
	}
	if (Input.usb_id != USB_ID)
	{
		Input.usb_id = USB_ID;
	}
	if (Input.gps_time != GPS_TIME_INT)
	{
		Input.gps_time = GPS_TIME_INT;
	}
	Input.gps_power = GPS_POWER;

	if (SW1)		Sw1_cnt++;
	else		{	Sw1_cnt = 0; Input.sw1 = 0;	}
	if (!SW2)		Sw2_cnt++;
	else		{	Sw2_cnt = 0; Input.sw2 = 0;	}


	if (!--ActiveTime)
	{
		ActiveTime	= ACTIVE_TIME_100MS;
		// Sleep
	}

	if ((Sw1_cnt > 100) && (!Input.sw1))
	{
		Input.sw1 = 1;
		Gsm_On();
	}

	if ((Sw2_cnt > 100) && (!Input.sw2))
	{
		Input.sw2 = 1;
		if (!GPS_IE)
			Gps_On();
		else
		{
			GPS_POW_OFF();
			GPS_IE = 0;
			GPS_LED = LED_STATUS_OFF;
		}
	}

	if (Sw2_cnt > 300)
		FL_POWER_OFF = 1;
}

U16 InputFlags	(void)
{
	return Input.word;
}

U16 ActiveTimeOut(void)
{
	return ActiveTime;
}
