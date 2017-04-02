#ifndef		_ports_home_h_
#define		_ports_home_h_

//___________________________________________________________________________________
#define InitPort_B()	LATB = 0x2000; TRISB = 0x5BFF; ODCB = 0x0000; ANSB = 0x003F
#define MEM_SO			PPS_OUT_B15	//				O	0
#define MEM_SI			PPS_IN_B14	//				I	0
#define MEM_CS			_LATB13		//				O	1
#define GPS_TIME_INT	_RB12		//				I___0______
#define GPS_INT			_LATB11		//				I	0
#define GPS_POWER		_LATB10		//				O	0
#define GPS_RX			PPS_IN_B9	//				I	0
#define GPS_TX			PPS_OUT_B8	//				I___0______
#define SET_TX			PPS_OUT_B7	//				I	0
#define SET_RX			PPS_IN_B6	//				I	0
#define AN_DC_DC		ADC1BUF5	// _RB5	// AN5	I	0	A
#define AN_EXT1			ADC1BUF4	// _RB4	// AN4	I___0___A_
#define AN_EXT2			ADC1BUF3	// _RB3	// AN3	I	0	A
#define AN_EXT3			ADC1BUF2	// _RB2	// AN2	I	0	A
#define AN_BAT			ADC1BUF1	// _RB1	// AN1	I	0	A
#define AN_POW			ADC1BUF0	// _RB0	// AN0	I___0___A_
#define AN_V18			ADC1BUF6	// Vcap					A

//___________________________________________________________________________________
#ifdef UART4_WIFI
	#define InitPort_C()	LATC = 0x0000; TRISC = 0xFFFF; ODCC = 0x0000; ANSC = 0x0000
	#define OSCO			_LATC15		//				I	0
	#define WIFI_RX			PPS_IN_C14	//				I	0
	#define WIFI_IO2		_RC13		//				I	0
	#define OSCI			_LATC12		//				I___0______
#else
	#define InitPort_C()	LATC = 0x6000; TRISC = 0x9FFF; ODCC = 0x0000; ANSC = 0x0000
	#define OSCO			_LATC15		//				I	0
	#define LCD_BACKLIGHT	_LATC14		//				O	1
	#define LCD_POWER		_LATC13		//				O	1
	#define OSCI			_LATC12		//				I___0______
#endif
//___________________________________________________________________________________
#ifdef ETH_W5100
	#define InitPort_D()	LATD = 0x0F26; TRISD = 0xF014; ODCD = 0x0010; ANSD = 0x0000
	//#define NC			_RD15		//				I	0
	//#define NC			_RD14		//				I	0
	//#define NC			_RD13		//				I	0
	//#define NC			_RD12		//				I___0______
	#if (LCD == LCD_BLUE)
		#define LCD_SDA		PPS_OUT_D11	//				O	1
		#define LCD_RST		_LATD10		//				O	1
		#define LCD_CD		_LATD9		//				O	1
		#define LCD_CS		_LATD8		//				O___1______
	#elif (LCD == LCD_RGB)
		#define LCD_SDA		PPS_OUT_D11	//				O	1
		#define LCD_RST		_LATD10		//				O	1
		#define LCD_CD		_LATD9		//				O	1
		#define LCD_CS		_LATD8		//				O___1______
	#else
		#define LCD_SCK		PPS_OUT_D11	//				O	1
		#define LCD_CD		_LATD10		//				O	1
		#define LCD_RST		_LATD9		//				O	1
		#define LCD_CS		_LATD8		//				O___1______
	#endif
	#define LED2			_LATD7		//				O	0
	#define LED3			_LATD6		//				O	0
	#define W51_SCS			_LATD5		//				O	1
	#define GSM_P_KEY		_TRISD4		//              I___0______
	#define W51_SCK			PPS_OUT_D3	//				O	0
	#define W51_SI			PPS_IN_D2	//				I	1
	#define W51_SO			PPS_OUT_D1	//				O	1
	#if (LCD == LCD_BLUE)
		#define LCD_SCK		PPS_OUT_D0	//				O___0______
	#elif (LCD == LCD_RGB)
		#define LCD_SCK		PPS_OUT_D0	//				O___0______
	#else
		#define LCD_SDA		PPS_OUT_D0	//				O___0______
	#endif
#else
	#define InitPort_D()	LATD = 0x0F00; TRISD = 0xF01E; ODCD = 0x0010; ANSD = 0x0000
	//#define NC			_RD15		//				I	0
	//#define NC			_RD14		//				I	0
	//#define NC			_RD13		//				I	0
	//#define NC			_RD12		//				I___0______
	#define LCD_SCK			PPS_OUT_D11	//				O	0
	#define LCD_CD			_LATD10		//				O	0
	#define LCD_RST			_LATD9		//				O	0
	#define LCD_CS			_LATD8		//				O___1______
	#define LED2			_LATD7		//				O	0
	#define LED3			_LATD6		//				O	0
	#define LED4			_LATD5		//				O	0
	#define GSM_P_KEY		_TRISD4		//				I___0______
	#define GSM_RI			_RD3		//				I	0
	#define GSM_RX			PPS_IN_D2	//				I	0
	#define GSM_TX			PPS_OUT_D1	//				I	0
	#define LCD_SDA			PPS_OUT_D0	//				O___0______
#endif

//___________________________________________________________________________________
#define InitPort_E()	LATE = 0x0010; TRISE = 0xFFED; ODCE = 0x0000
//#define NC			_RE15		//				I	0
//#define NC			_RE14		//				I	0
//#define NC			_RE13		//				I	0
//#define NC			_RE12		//				I___0______
//#define NC			_RE11		//				I	0
//#define NC			_RE10		//				I	0
//#define NC			_RE9		//				I	0
//#define NC			_RE9		//				I___0______
#define EXT_IN3			_RE7		//				I	0
#define EXT_IN2			_RE6		//				I	0
#define EXT_IN1			_RE5		//				I	0
#define W51_RST			_LATE4		//				O___1______
#define NC1				_LATE3		//				I	0
#define GSM_STATUS		_RE2		//				I	0	PullUp
#define GSM_DTR			_LATE1		//				O	0
#define GSM_DCD			_RE0		//				I___0______
#define GSM_RTS			_TRISE2

//___________________________________________________________________________________
#define InitPort_F()	LATF = 0x0010; TRISF = 0xFFEA; ODCF = 0x0002; ANSF = 0x0000
//#define NC			_RF15		//				O	0
//#define NC			_RF14		//				O	0
//#define NC			_RF13		//				O	0
//#define NC			_RF12		//				O___0______
//#define NC			_RF11		//				O	0
//#define NC			_RF10		//				O	0
//#define NC			_RF9		//				O	0
//#define NC			_RF8		//				O___0______
#define USB_5V			_RF7		//				I	0
//#define NC			_RF6		//				O	0
#define W51_INT			PPS_IN_F5	//				I	0
#define MEM_SCK			PPS_OUT_F4	//				O___1______
  #define W51_CS		_LATF3		//				I	0	
  #define USB_ID			_RF3		//				I	0		GSM_ANALOG_IN	_RF3
//#define NC			_RF2		//				O	0
#define GSM_POWER		_TRISF1		//				I	0
#define LED1			_LATF0		//				O___0______

//___________________________________________________________________________________
#ifndef		USB_PORT				// RG2, RG3 = D+,D- by default -> disable if USB not used
	#define InitPort_G()	LATG = 0x0000; TRISG = 0xFDFF; ODCG = 0x0000; ANSG = 0x0000; U1CNFG2bits.UTRDIS = 1
#else
	#define InitPort_G()	LATG = 0x0000; TRISG = 0xFDFF; ODCG = 0x0000; ANSG = 0x0000
#endif
#define USB_D_PLUS		_LATG2		//				I	0
#define USB_D_MINUS		_LATG3		//				I	0
#define EXT_IN4			_RG6		//				I	0
#define EXT_OUT1		_LATG7		//				I___0______________
#define EXT_OUT2		_LATG8		//				I	0
#define PWM_DC			PPS_OUT_G9	//				O	0

#endif	//	_ports_home_h_
