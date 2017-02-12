#ifndef		_ports_see_h_
#define		_ports_see_h_

//___________________________________________________________________________________
#define InitPort_B()	LATB = 0x9000; TRISB = 0x61FF; ODCB = 0x0000; ANSB = 0x003F
#define MEM_SCK			PPS_OUT_B15	//				O	1
#define MEM_SI			PPS_IN_B14	//				I	0
#define GSM_DCD			_RB13		//				I	0
#define MEM_CS			_LATB12		//				O___1______
#define LED1			_LATB11		//				O	0
#define LED2			_LATB10		//				O	0
#define GSM_DTR			_LATB9		//				O	0
#define GSM_STATUS		_RB8		//				I___0______PullUp
#define SET_TX			PPS_OUT_B7	//				I	0
#define SET_RX			PPS_IN_B6	//				I	0
#define AN_DC_DC		ADC1BUF5	// _RB5	// AN5	I	0	A
#define AN_EXT1			ADC1BUF4	// _RB4	// AN4	I___0___A_		Not used
#define AN_EXT2			ADC1BUF3	// _RB3	// AN3	I	0	A		Not used
#define AN_EXT3			ADC1BUF2	// _RB2	// AN2	I	0	A		Not used
#define AN_BAT			ADC1BUF1	// _RB1	// AN1	I	0	A
#define AN_POW			ADC1BUF0	// _RB0	// AN0	I___0___A_		Not used
#define AN_V18			ADC1BUF6	// Vcap					A
#define GSM_RTS			_TRISB8

//___________________________________________________________________________________
#define InitPort_C()	LATC = 0x0000; TRISC = 0xFFFF; ODCC = 0x0000; ANSC = 0x0000
#define OSCO			_LATC15		//				I	0
//#define NC			_RC14		//				I	0
//#define NC			_RC13		//				I	0
#define OSCI			_LATC12		//				I___0______

//___________________________________________________________________________________
#define InitPort_D()	LATD = 0x0004; TRISD = 0xFDFB; ODCD = 0x0000; ANSD = 0x0000
//#define NC			_RD15		//				I	0
//#define NC			_RD14		//				I	0
//#define NC			_RD13		//				I	0
//#define NC			_RD12		//				I___0______
//#define NC			_RD11		//				I	0
//#define NC			_RD10		//				I	0
#define PWM_DC			PPS_OUT_D9	//				O	0
#define SW1				_RD8		//				I___0______
//#define NC			_RD7		//				I	0
//#define NC			_RD6		//				I	0
#define GPS_TX			PPS_OUT_D4	//				I	0
#define GPS_RX			PPS_IN_D5	//				I___0______
#define CHARGE_STAT		_RD3		//				I	0
#define EN_POW			_LATD2		//				O	1
//#define NC			_RD1		//				I	0
//#define NC			_RD0		//				I___0______

//___________________________________________________________________________________
#define InitPort_E()	LATE = 0x0000; TRISE = 0xFFEF; ODCE = 0x0000
//#define NC			_RE15		//				I	0
//#define NC			_RE14		//				I	0
//#define NC			_RE13		//				I	0
//#define NC			_RE12		//				I___0______
//#define NC			_RE11		//				I	0
//#define NC			_RE10		//				I	0
//#define NC			_RE9		//				I	0
//#define NC			_RE9		//				I___0______
#define SW2				_RE7		//				I	0
#define SENS1			_RE6		//				I	0
#define SENS2			_RE5		//				I	0
#define GPS_POWER		_LATE4		//				O___0______
#define GPS_INT			_LATE3		//				I	0
#define GPS_TIME_INT	_RE2		//				I	0
//#define NC			_RE1		//				I	0
//#define NC			_RE0		//				I___0______

//___________________________________________________________________________________
#define InitPort_F()	LATF = 0x0000; TRISF = 0xFFEF; ODCF = 0x0000; ANSF = 0x0000
//#define NC			_RF15		//				I	0
//#define NC			_RF14		//				I	0
//#define NC			_RF13		//				I	0
//#define NC			_RF12		//				I___0______
//#define NC			_RF11		//				I	0
//#define NC			_RF10		//				I	0
//#define NC			_RF9		//				I	0
//#define NC			_RF8		//				I___0______
#define USB_5V			_RF7		//				I	0
//#define NC			_RF6		//				I	0
//#define NC			_RF5		//				I	0
#define MEM_SO			PPS_OUT_F4		//				O___0______
#define USB_ID			_RF3		//				I	0
//#define NC			_RF2		//				I	0
//#define NC			_RF1		//				I	0
//#define NC			_RF0		//				I___0______

//___________________________________________________________________________________
#ifndef		USB_PORT				// RG2, RG3 = D+,D- by default -> disable if USB not used
	#define InitPort_G()	LATG = 0x0000; TRISG = 0xFFFF; ODCG = 0x0040; ANSG = 0x0000; U1CNFG2bits.UTRDIS = 1
#else
	#define InitPort_G()	LATG = 0x0000; TRISG = 0xFFFF; ODCG = 0x0040; ANSG = 0x0000
#endif
//#define NC			_RG15		//				I	0
//#define NC			_RG14		//				I	0
//#define NC			_RG13		//				I	0
//#define NC			_RG12		//				I___0______
//#define NC			_RG11		//				I	0
//#define NC			_RG10		//				I	0
#define GSM_RX			PPS_IN_G8	//				I	0
#define GSM_TX			PPS_OUT_G9	//				I___0______
#define GSM_RI			_RG7		//				I	0
#define GSM_P_KEY		_TRISG6		//				I	0
//#define NC			_RG5		//				I	0
//#define NC			_RG4		//				I___0______
#define USB_D_MINUS		_LATG3		//				I	0
#define USB_D_PLUS		_LATG2		//				I	0
//#define NC			_RG1		//				I	0
//#define NC			_RG0		//				I___0______

#endif	//	_ports_see_h_
