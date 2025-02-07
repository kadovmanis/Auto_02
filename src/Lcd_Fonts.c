#include <Lcd_Fonts.h>

#if	(LCD != LCD_NO)
//#include	<string.h>
//#include	<stdio.h>

const FONT_INFO* Fonts[] = 
{
	&Arial_7x5_Info,
	&SansSerif_16_Info,
	&Segoe_24_Info
};

U8 SwitchCode(U8 ch)
{
//	U8 a = ch;
	if (ch > 0xE0)
		ch -= 0xE0;
	else
	{
		switch (ch)
		{
		case 0xBA:  ch = 0x00;	break;		// 00h	_E0		_BA	ŗ
		case 0xAA:	ch = 0x01;	break;		// 01h	_E1		_AA	Ŗ
		case 0xC2:	ch = 0x03;	break;		// 03h	_E3		_C2	Ā
		case 0xC4:	ch = 0x05;	break;		// 05h	_E5		_C4	Ä
		case 0xC7:	ch = 0x06;	break;		// 06h	_E6		_C7	Ē
		case 0xC8:	ch = 0x09;	break;		// 09h	_E9		_C8	Č		
		case 0xCC:	ch = 0x0A;	break;		// 0Ah	_EA		_CC	Ģ
		case 0xCD:	ch = 0x0B;	break;		// 0Bh	_EB		_CD	Ķ
		case 0xCE:	ch = 0x11;	break;		// 11h	_F1		_CE	Ī
		case 0xCF:	ch = 0x13;	break;		// 13h	_F3		_CF	Ļ
		case 0xD0:	ch = 0x15;	break;		// 15h	_F5		_D0	Š
		case 0xD2:	ch = 0x17;	break;		// 17h	_F7		_D2	Ņ
		case 0xD4:	ch = 0x18;	break;		// 18h	_F8		_D4	Õ
		case 0xD6:	ch = 0x19;	break;		// 19h	_F9		_D6	Ö
		case 0xDB:	ch = 0x1A;	break;		// 1Ah	_FA		_DB	Ū
		case 0xDC:	ch = 0x1D;	break;		// 1Dh	_FD		_DC	Ü
		case 0xDE:	ch = 0x1F;	break;		// 1Fh	_FF		_DE	Ž
		default:	ch = 0x20;	break;		//
		}
	}
//	char test[32];
//	sprintf(test, "sw %02X- %02X ", a, ch);
//	LcdString_old(0, 0, test);
	return ch;
}

const U8 LcdInitSequence[] =
{
	#if	(LCD == LCD_RGB)
		LCD_OFF_CMD,					// Display Off
	    SET_CONTRAST_A,			0x91,//0xFF,	// Set contrast for color A					145	0x91
	    SET_CONTRAST_B,			0x50,//0xFF,	// Set contrast for color B					80	0x50
	    SET_CONTRAST_C,			0x7D,//0xFF,	// Set contrast for color C					125	0x7D
	    MASTER_CURRENT_CONTROL,	0x06,	// master current control					6
	    SET_PRECHARGE_SPEED_A,	0x64,	// Set Second Pre-change Speed For ColorA	100
	    SET_PRECHARGE_SPEED_B,	0x78,	// Set Second Pre-change Speed For ColorB	120
	    SET_PRECHARGE_SPEED_C,	0x64,	// Set Second Pre-change Speed For ColorC	100
		SET_REMAP_DATA_FORMAT,	0x76,	// set remap & data format						0x72
	    SET_DISPLAY_START_LINE,	0x0,	// Set display Start Line
	    SET_DISPLAY_OFFSET,		0x0,	// Set display offset
	    NORMAL_DISPLAY,					// Set display mode
		MULTIPLEX_RATIO,		0x3F,	// Set multiplex ratio
	    SET_MASTER_CONFIGURE,	0x8E,	// Set master configuration
	    POWER_SAVE_MODE,		0x00,	// Set Power Save Mode							0x00
	    PHASE_PERIOD_ADJUSTMENT,0x31,	// phase 1 and 2 period adjustment				0x31
	    DISPLAY_CLOCK_DIV,		0xF0,	// display clock divider/oscillator frequency
	    SET_PRECHARGE_VOLTAGE,	0x3A,	// Set Pre-Change Level
	    SET_V_VOLTAGE,			0x3E,	// Set vcomH
	    DEACTIVE_SCROLLING,				// disable scrolling
		LCD_ON_CMD,						// Set display on
	#elif (LCD == LCD_BLUE)
		LCD_OFF_CMD,		//--turn off oled panel
		Y_START_L,			//---set low column address
		Y_START_H,			//---set high column address
		START_ADDR,			//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
		EL_VOLUME_MODE,		//--set contrast control register
		0xCF,				// Set SEG Output Current Brightness
		ADC_1,				//--Set SEG/Column Mapping     
		COM_OUT_DIR_LEFT,	//Set COM/Row Scan Direction   
		NORMAL_INDICATION,	//--set normal display
		MULTIPLEX_RATIO,	//--set multiplex ratio(1 to 64)
		0x3f,				//--1/64 duty
		0xD3,				//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
		0x00,				//-not offset
		CLOCK_RATIO,		//--set display clock divide ratio/oscillator frequency
		0x80,				//--set divide ratio, Set Clock as 100 Frames/Sec
		0xD9,		//--set pre-charge period
		0xF1,		//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		0xDA,		//--set com pins hardware configuration
		0x12,		
		0xDB,		//--set vcomh
		0x40,		//Set VCOM Deselect Level
		0x20,		//-Set Page Addressing Mode (0x00/0x01/0x02)
		0x02,		//
		0x8D,		//--set Charge Pump enable/disable
		0x14,		//--set(0x10) disable
		0xA4,		// Disable Entire Display On (0xa4/0xa5)
		NORMAL_INDICATION,	// Disable Inverse Display On (0xa6/a7) 
		LCD_ON_CMD,			//--turn on oled panel
	#else
		LCD_RESET_CMD,
		SET_REMAP_DATA_FORMAT,
		0x60,

		ADC_1,					// ADC reverse

		POWER_CONTROL_CMD,
		0x25,				// INTERNAL_VOLTAGE
		0xA3,				// LCD_BIAS_SET
		EL_VOLUME_MODE,
		0x1F,				//lcd light adj
		COM_OUT_DIR_RIGHT,
		NORMAL_INDICATION,
		LCD_ON_CMD,
	#endif

	0x11

/*

	#ifdef	DISPLAY_BLUE
		LCD_RESET_CMD,
		RAM_ADDR_SET,
		0x60,
	#else
		START_ADDR,			// Display start line set
	#endif
	ADC_1,					// ADC reverse
	COM_OUT_DIRECTION,		// Common output mode - Normal COM0~COM63
	NORMAL_INDICATION,		// Display normal
	LCD_BIAS_SET_0,			// Set bias 1/9 (Duty 1/65)
	POWER_CONTROL_CMD,		// Power control - Booster, Regulator and Follower on
	BOOSTER_RATIO,			// Set internal Booster to 4x
	0x00,					// 0 - 2x, 3x, 4x, ; 1 - 5x; 3 - 6x
	INTERNAL_VOLTAGE_7,		// V0 voltage regulator set
	EL_VOLUME_MODE,			// Contrast set
	0x1F,					// Parameter
	INDICATOR,				// Static indicator set - No indicator
	0x00,					// 1- ON; 0 - OFF
	LCD_ON_CMD//,
*/
};
/*
RU8 LcdClearSequence[DISP_WITH_PIX] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
*/
const U8 font7x5[0x80][CHAR_7x5_WIDTH] =
{
	{0xF8,0x10,0x88,0x48,0x10},		// 00h			_BA	ŗ
	{0xFE,0x12,0xB2,0x52,0x8C},		// 01h			_AA	Ŗ
	{0x40,0xAA,0xAA,0xAA,0xF0},		// 02h	_E2	ā
	{0xF0,0x4A,0x4A,0x4A,0xF0},		// 03h			_C2	Ā
	{0x40,0xAA,0xA8,0xAA,0xF0},		// 04h	_E4	ä
	{0xFC,0x22,0x22,0x22,0xFC},		// 05h			_C4	Ä
	{0xF8,0xAA,0xA8,0xAA,0x88},		// 06h			_C7	Ē
	{0x70,0xAA,0xAA,0xAA,0x30},		// 07h	_E7	ē
	{0x70,0x8A,0x8C,0x8A,0x40},		// 08h	_E8	č
	{0x78,0x85,0x86,0x85,0x48},		// 09h			_C8	Č
	{0x7C,0x82,0xD2,0x92,0xF4},		// 0Ah			_CC	Ģ
	{0xFE,0x10,0xA8,0x44,0x82},		// 0Bh			_CD	Ķ
	{0x18,0xA5,0xA6,0xA5,0x7C},		// 0Ch	_EC	ģ
	{0xFE,0x20,0xD0,0x48,0x80},		// 0Dh	_ED	ķ
	{0x00,0x8A,0xFA,0x82,0x00},		// 0Eh	_EE	ī
	{0x00,0xC2,0x7E,0x40,0x00},		// 0Fh	_EF	ļ

	{0x90,0xAA,0xAC,0xAA,0x40},		// 10h	_F0	š
	{0x00,0x8A,0xFA,0x8A,0x00},		// 11h			_CE	Ī
	{0xF8,0x10,0x88,0x48,0xF0},		// 12h	_F2	ņ
	{0xFE,0x80,0xC0,0xA0,0x80},		// 13h			_CF	Ļ
	{0x70,0x8A,0x8A,0x8A,0x70},		// 14h	_F4	õ
	{0x88,0x95,0x96,0x95,0x64},		// 15h			_D0	Š
	{0x70,0x8A,0x88,0x8A,0x70},		// 16h	_F6	ö
	{0xFE,0x08,0xD0,0x20,0xFE},		// 17h			_D2	Ņ
	{0x70,0x8A,0x8A,0x8A,0x70},		// 18h			_D4	Õ
	{0x78,0x86,0x84,0x86,0x78},		// 19h			_D6	Ö
	{0x78,0x82,0x82,0x82,0x78},		// 1Ah			_DB	Ū
	{0x78,0x82,0x82,0x42,0xF8},		// 1Bh	_FB	ū
	{0x78,0x82,0x80,0x42,0xF8},		// 1Ch	_FC	ü
	{0x78,0x82,0x80,0x82,0x78},		// 1Dh			_DC	Ü
	{0x88,0xCA,0xAC,0x9A,0x88},		// 1Eh	_FE	ž
	{0xC4,0xA5,0x96,0x8D,0x84},		// 1Fh			_DE	Ž

	{0x00,0x00,0x00,0x00,0x00},		// 20h ' '
	{0x00,0x00,0x9E,0x00,0x00},		// 21h	!
	{0x00,0x0E,0x00,0x0E,0x00},		// 22h	"
	{0x28,0xFE,0x28,0xFE,0x28},		// 23h	#
	{0x48,0x54,0xFE,0x54,0x24},		// 24h	$
	{0x46,0x26,0x10,0xC8,0xC4},		// 25h	%
	{0x6C,0x92,0xA9,0x44,0xA0},		// 26h	&
	{0x00,0x0A,0x06,0x00,0x00},		// 27h	'
	{0x00,0x38,0x44,0x82,0x00},		// 28h	(
	{0x00,0x82,0x44,0x38,0x00},		// 29h	)
	{0x28,0x10,0x7C,0x10,0x28},		// 2Ah	*
	{0x10,0x10,0x7C,0x10,0x10},		// 2Bh	+
	{0x00,0xA0,0x60,0x00,0x00},		// 2Ch	,
	{0x10,0x10,0x10,0x10,0x10},		// 2Dh	-
	{0x00,0xC0,0xC0,0x00,0x00},		// 2Eh	.
	{0x40,0x20,0x10,0x08,0x04},		// 2Fh	/

	{0x7C,0xA2,0x92,0x8A,0x7C},		// 30h	0
	{0x00,0x84,0xFE,0x80,0x00},		// 31h	1
	{0x84,0xC2,0xA2,0x92,0x8C},		// 32h	2
	{0x42,0x82,0x8A,0x96,0x62},		// 33h	3
	{0x30,0x28,0x24,0xFE,0x20},		// 34h	4
	{0x4E,0x8A,0x8A,0x8A,0x72},		// 35h	5
	{0x78,0x94,0x92,0x92,0x60},		// 36h	6
	{0x02,0xE2,0x12,0x0A,0x06},		// 37h	7
	{0x6C,0x92,0x92,0x92,0x6C},		// 38h	8
	{0x0C,0x92,0x92,0x52,0x3C},		// 39h	9
	{0x00,0x6C,0x6C,0x00,0x00},		// 3Ah	:
	{0x00,0xAC,0x6C,0x00,0x00},		// 3Bh	;
	{0x10,0x28,0x44,0x82,0x00},		// 3Ch	<
	{0x28,0x28,0x28,0x28,0x28},		// 3Dh	=
	{0x00,0x82,0x44,0x28,0x10},		// 3Eh	>
	{0x04,0x02,0xB2,0x12,0x0C},		// 3Fh	?

	{0x64,0x92,0xF2,0x82,0x7C},		// 40h	@
	{0xFC,0x22,0x22,0x22,0xFC},		// 41h	A
	{0xFE,0x92,0x92,0x92,0x6C},		// 42h	B
	{0x7C,0x82,0x82,0x82,0x44},		// 43h	C
	{0xFE,0x82,0x82,0x44,0x38},		// 44h	D
	{0xFE,0x92,0x92,0x92,0x82},		// 45h	E
	{0xFE,0x12,0x12,0x12,0x02},		// 46h	F
	{0x7C,0x82,0x92,0x92,0xF4},		// 47h	G
	{0xFE,0x10,0x10,0x10,0xFE},		// 48h	H
	{0x00,0x82,0xFE,0x82,0x00},		// 49h	I
	{0x40,0x80,0x82,0x7E,0x02},		// 4Ah	J
	{0xFE,0x10,0x28,0x44,0x82},		// 4Bh	K
	{0xFE,0x80,0x80,0x80,0x80},		// 4Ch	L
	{0xFE,0x04,0x18,0x04,0xFE},		// 4Dh	M
	{0xFE,0x08,0x10,0x20,0xFE},		// 4Eh	N
	{0x7C,0x82,0x82,0x82,0x7C},		// 4Fh	O

	{0xFE,0x12,0x12,0x12,0x0C},		// 50h	P
	{0x7C,0x82,0xA2,0x42,0xBC},		// 51h	Q
	{0xFE,0x12,0x32,0x52,0x8C},		// 52h	R
	{0x8C,0x92,0x92,0x92,0x62},		// 53h	S
	{0x02,0x02,0xFE,0x02,0x02},		// 54h	T
	{0x7E,0x80,0x80,0x80,0x7E},		// 55h	U
	{0x3E,0x40,0x80,0x40,0x3E},		// 56h	V
	{0x7E,0x80,0x60,0x80,0x7E},		// 57h	W
	{0xC6,0x28,0x10,0x28,0xC6},		// 58h	X
	{0x0E,0x10,0xE0,0x10,0x0E},		// 59h	Y
	{0xC2,0xA2,0x92,0x8A,0x86},		// 5Ah	Z
	{0x00,0xFE,0x82,0x82,0x00},		// 5Bh	[
	{0x04,0x08,0x10,0x20,0x40},		// 5Ch	'\'
	{0x00,0x82,0x82,0xFE,0x00},		// 5Dh	]
	{0x08,0x04,0x02,0x04,0x08},		// 5Eh	^
	{0x80,0x80,0x80,0x80,0x80},		// 5Fh	_

	{0x00,0x02,0x04,0x08,0x00},		// 60h	`
	{0x40,0xA8,0xA8,0xA8,0xF0},		// 61h	a
	{0xFE,0x90,0x88,0x88,0x70},		// 62h	b
	{0x70,0x88,0x88,0x88,0x40},		// 63h	c
	{0x70,0x88,0x88,0x90,0xFE},		// 64h	d
	{0x70,0xA8,0xA8,0xA8,0x30},		// 65h	e
	{0x10,0xFC,0x12,0x12,0x04},		// 66h	f
	{0x18,0xA4,0xA4,0xA4,0x7C},		// 67h	g
	{0xFE,0x10,0x08,0x08,0xF0},		// 68h	h
	{0x00,0x88,0xFA,0x80,0x00},		// 69h	i
	{0x40,0x80,0x88,0x7A,0x00},		// 6Ah	j
	{0xFE,0x20,0x50,0x88,0x00},		// 6Bh	k
	{0x00,0x82,0xFE,0x80,0x00},		// 6Ch	l
	{0xF8,0x08,0xF0,0x08,0xF0},		// 6Dh	m
	{0xF8,0x10,0x08,0x08,0xF0},		// 6Eh	n
	{0x70,0x88,0x88,0x88,0x70},		// 6Fh	o

	{0xF8,0x28,0x28,0x28,0x10},		// 70h	p
	{0x10,0x28,0x28,0x30,0xF8},		// 71h	q
	{0xF8,0x10,0x08,0x08,0x10},		// 72h	r
	{0x90,0xA8,0xA8,0xA8,0x40},		// 73h	s
	{0x08,0x7E,0x88,0x80,0x40},		// 74h	t
	{0x78,0x80,0x80,0x40,0xF8},		// 75h	u
	{0x38,0x40,0x80,0x40,0x38},		// 76h	v
	{0x78,0x80,0x60,0x80,0x78},		// 77h	w
	{0x88,0x50,0x20,0x50,0x88},		// 78h	x
	{0x18,0xA0,0xA0,0xA0,0x78},		// 79h	y
	{0x88,0xC8,0xA8,0x98,0x88},		// 7Ah	z
	{0x00,0x10,0x6C,0x82,0x00},		// 7Bh	{
	{0x00,0x00,0xFE,0x00,0x00},		// 7Ch	|
	{0x00,0x82,0x6C,0x10,0x00},		// 7Dh	}
	{0x10,0x08,0x10,0x20,0x10},		// 7Eh	~
	{0x20,0x70,0xA8,0x20,0x20},		// 7Fh	<-
};
/*
const U8 font40x21[10][CHAR_40x21_HIGH][CHAR_40x21_WIDTH] = 
{
	{		
		{0x00,0x00,0x80,0xE0,0xF0,0xF0,0x78,0x78,0x3C,0x3C,0x3C,0x3C,0x3C,0x78,0x78,0xF0,0xF0,0xE0,0x80,0x00,0x00},	// '0'
		{0xF0,0xFE,0xFF,0xFF,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x1C,0x03,0x01,0x07,0xFF,0xFF,0xFE,0xF0},	
		{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xC0,0x38,0x07,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},	
		{0x1F,0xFF,0xFF,0xFF,0xC0,0x00,0x80,0x70,0x0E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFF,0xFF,0xFF,0x1F},	
		{0x00,0x00,0x03,0x0F,0x1F,0x1F,0x3D,0x3C,0x78,0x78,0x78,0x78,0x78,0x3C,0x3C,0x1F,0x1F,0x0F,0x03,0x00,0x00},	
	},		
	{		
		{0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xE0,0xF0,0xF8,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// '1'
		{0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x60,0x70,0x70,0x70,0x7F,0x7F,0x7F,0x7F,0x7F,0x70,0x70,0x70,0x60,0x00,0x00,0x00,0x00},	
	},		
	{		
		{0x00,0x80,0xE0,0xF0,0xF0,0xF8,0x78,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,0x80,0x00},	// '2'
		{0x0E,0x1F,0x1F,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x83,0xFF,0xFF,0xFF,0xFE},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xE0,0xF0,0xF8,0xFC,0xFE,0x7F,0x3F,0x1F,0x0F,0x03,0x00},	
		{0x00,0x80,0xE0,0xF8,0xFC,0xFE,0xFF,0x7F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x7E,0x7F,0x7F,0x7F,0x7F,0x7F,0x79,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x70},	
	},		
	{		
		{0x00,0x80,0xE0,0xF0,0xF0,0xF8,0x78,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,0x80,0x00},	// '3'
		{0x0E,0x1F,0x1F,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC3,0xFF,0xFF,0xFF,0x3E},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x3E,0x3E,0x3E,0x1C,0x1C,0x3E,0x3E,0x7F,0xFF,0xFF,0xF3,0xE1,0x80},	
		{0xF0,0xF8,0xF8,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0xFF,0xFF},	
		{0x00,0x03,0x0F,0x1F,0x1F,0x3E,0x3C,0x7C,0x78,0x78,0x78,0x78,0x78,0x7C,0x3C,0x3E,0x1F,0x1F,0x0F,0x03,0x00},	
	},		
	{		
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0x78,0x18,0x08,0x00,0x00,0x00,0x00,0x00},	// '4'
		{0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xFC,0xFF,0x7F,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x00,0xC0,0xF0,0xFC,0xFF,0x7F,0x1F,0x07,0x01,0x00,0x00,0xF0,0xF8,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x1F,0x1F,0x1F,0x1F,0x1F,0x1E,0x1E,0x1E,0x1E,0x1E,0xFF,0xFF,0xFF,0xFF,0x1E,0x1E,0x1E,0x1E,0x1E,0x1C,0x18},	
		{0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x70,0x70,0x7F,0x7F,0x7F,0x7F,0x7F,0x70,0x70,0x70,0x60,0x00,0x00,0x00},	
	},		
	{		
		{0xF8,0xF8,0xF8,0xF8,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x38},	// '5'
		{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x1F,0x1F,0x1F,0x1F,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x3E,0x3C,0x7C,0xF8,0xF8,0xF0,0xC0,0x00},	
		{0xF0,0xF8,0xF8,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0xFF,0xFF},	
		{0x00,0x03,0x0F,0x1F,0x1F,0x3E,0x3C,0x7C,0x78,0x78,0x78,0x78,0x78,0x7C,0x3C,0x3E,0x1F,0x1F,0x0F,0x03,0x00},	
	},		
	{		
		{0x00,0x80,0xE0,0xF0,0xF0,0xF8,0x78,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,0x80,0x00},	// '6'
		{0xFE,0xFF,0xFF,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x1F,0x1F,0x0E},	
		{0xFF,0xFF,0xFF,0xFF,0xF8,0x7C,0x3C,0x3E,0x1E,0x1E,0x1E,0x1E,0x1E,0x3E,0x3C,0x7C,0xF8,0xF8,0xF0,0xC0,0x00},	
		{0xFF,0xFF,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0xFF,0xFF},	
		{0x00,0x03,0x0F,0x1F,0x1F,0x3E,0x3C,0x7C,0x78,0x78,0x78,0x78,0x78,0x7C,0x3C,0x3E,0x1F,0x1F,0x0F,0x03,0x00},	
	},		
	{		
		{0x38,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0xF8,0xF8,0xF8,0xF8},	// '7'
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0xFF,0x3F,0x07,0x00},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xF8,0xFF,0xFF,0xCF,0xC1,0xC0,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x81,0xF1,0xFF,0xFF,0x1F,0x03,0x01,0x01,0x01,0x00,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xFC,0xFF,0x3F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	
	},		
	{	
		{0x00,0x80,0xE0,0xF0,0xF0,0xF8,0x78,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,0x80,0x00},	// '8'
		{0x3E,0xFF,0xFF,0xFF,0xE3,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE3,0xFF,0xFF,0xFF,0x3E},	
		{0x00,0xC0,0xF3,0xFF,0xFF,0x7F,0x3F,0x3F,0x1E,0x1E,0x1E,0x1E,0x1E,0x3F,0x3F,0x7F,0xFF,0xFF,0xF3,0xC0,0x00},	
		{0xFF,0xFF,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0xFF,0xFF},	
		{0x00,0x03,0x0F,0x1F,0x1F,0x3E,0x3C,0x7C,0x78,0x78,0x78,0x78,0x78,0x7C,0x3C,0x3E,0x1F,0x1F,0x0F,0x03,0x00},	
	},
	{		
		{0x00,0x80,0xE0,0xF0,0xF0,0xF8,0x78,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,0x80,0x00},	// '9'
		{0xFE,0xFF,0xFF,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFE},	
		{0x01,0x07,0x1F,0x3F,0x3F,0x7C,0x78,0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF8,0x78,0x7C,0x3F,0xFF,0xFF,0xFF,0xFF},	
		{0xF0,0xF8,0xF8,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0xFF},	
		{0x00,0x03,0x0F,0x1F,0x1F,0x3E,0x3C,0x7C,0x78,0x78,0x78,0x78,0x78,0x7C,0x3C,0x3E,0x1F,0x1F,0x0F,0x03,0x00},	
	}
};
*/
const U8 font24x15[10][CHAR_24x15_HIGH][CHAR_24x15_WIDTH] = 
{
	{		
		{0xC0,0xF0,0xF8,0x7C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x9C,0x7C,0xF8,0xF0,0xC0},	// '0'
		{0xFF,0xFF,0xFF,0x00,0x00,0x80,0x60,0x10,0x0C,0x03,0x00,0x00,0xFF,0xFF,0xFF},	
		{0x07,0x1F,0x3F,0x7C,0x72,0xE1,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0x3F,0x1F,0x07}	
	},		
	{		
		{0x00,0x00,0x00,0x00,0x10,0x18,0xF8,0xFC,0xFE,0x00,0x00,0x00,0x00,0x00,0x00},	// '1'
		{0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00},	
		{0x00,0x00,0x00,0x00,0x80,0xC0,0xFF,0xFF,0xFF,0xC0,0x80,0x00,0x00,0x00,0x00}	
	},		
	{		
		{0xC0,0xF0,0xF8,0x7C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x7C,0xF8,0xF0,0xC0},	// '2'
		{0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0x78,0x3C,0x1E,0x0F,0x07,0x03},	
		{0xF0,0xF8,0xFC,0xFE,0xEF,0xE7,0xE3,0xE1,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xC0}	
	},		
	{		
		{0xE0,0xF0,0xF8,0x3C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x3C,0xF8,0xF0,0xC0},	// '3'
		{0x00,0x01,0x00,0x00,0x00,0x00,0x10,0x38,0x38,0x38,0x38,0x7E,0xFF,0xFF,0xC7},	
		{0x0E,0x1F,0x3E,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0x3F,0x1F,0x07}	
	},		
	{		
		{0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xFC,0x3E,0x0E,0x02,0x00,0x00,0x00,0x00},	// '4'
		{0x00,0xC0,0xF0,0xFC,0x7F,0x0F,0x03,0x00,0xC0,0xE0,0xF0,0x00,0x00,0x00,0x00},	
		{0x07,0x07,0x07,0x07,0x07,0x07,0x87,0xC7,0xFF,0xFF,0xFF,0xC7,0x87,0x06,0x00}	
	},		
	{		
		{0xFE,0xFE,0xFE,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x06},	// '5'
		{0x07,0x0F,0x0F,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x7C,0xF8,0xF0,0xC0},	
		{0x0E,0x1F,0x3E,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0x3F,0x1F,0x07}	
	},		
	{		
		{0xE0,0xF0,0xF8,0x3C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x3C,0xF8,0xF0,0xC0},	// '6'
		{0xFF,0xFF,0xFF,0x7C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x7C,0xF8,0xF1,0xC0},	
		{0x0F,0x1F,0x3F,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0x3F,0x1F,0x07}	
	},		
	{		
		{0x06,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x8E,0xCE,0xFE,0x7E,0x1E},	// '7'
		{0x00,0x00,0x00,0x00,0x40,0x60,0xE0,0xE0,0xF8,0x7E,0x7F,0x67,0x21,0x00,0x00},	
		{0x00,0x00,0x80,0xE0,0xF8,0x7E,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00}	
	},		
	{		
		{0x00,0xE0,0xF8,0x3C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x3C,0xF8,0xE0,0x00},	// '8'
		{0x80,0xE1,0xF7,0xFF,0x3E,0x1C,0x1C,0x1C,0x1C,0x1C,0x3E,0xFF,0xF7,0xE1,0x80},	
		{0x0F,0x1F,0x3F,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0x3F,0x1F,0x07}	
	},		
	{		
		{0xE0,0xF0,0xF8,0x3C,0x1C,0x0E,0x0E,0x0E,0x0E,0x0E,0x1C,0x7C,0xF8,0xF0,0xC0},	// '9'
		{0x0F,0x1F,0x3F,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0xFF,0xFF,0xFF},	
		{0x0E,0x1F,0x3E,0x78,0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0x70,0x78,0x3F,0x1F,0x07}	
	}
};

const U8 bigDot[8] = {0x18,0x3C,0x7E,0xFF,0xFF,0x7E,0x3C,0x18};
//const U8 round[6] = {0x18,0x3C,0x66,0x66,0x3C,0x18};
//rom const CLOCK LightON	= {0x18,0x15,0x00};
//rom const CLOCK LightOFF = {0x02,0x15,0x00};

const U8 arrows[17][30] =
{
	{0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,	0x01, 0x00, 0xC0, 0x78, 0x3F, 0x3F, 0x1F, 0x8F, 0x1F, 0x3F, 0x3F, 0x78, 0xC0, 0x00, 0x01},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE2, 0xF0, 0xF8, 0xFC, 0x00, 0x00, 0x00, 0x00,	0x01, 0x08, 0x0C, 0x0E, 0x07, 0x07, 0x07, 0x83, 0x0F, 0x3F, 0xFF, 0x00, 0x00, 0x00, 0x01},
	{0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xE2, 0xE0, 0xF0, 0xF0, 0x78, 0x18, 0x04, 0x00,	0x00, 0x01, 0x01, 0x01, 0x03, 0x03, 0x0F, 0x7F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x01},
	{0x20, 0x20, 0x60, 0x60, 0xE0, 0xE0, 0xE0, 0xE2, 0xE0, 0xE0, 0xE0, 0xE0, 0x60, 0x20, 0x00,	0x01, 0x00, 0x00, 0x00, 0x70, 0x3E, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01},

	{0x08, 0x18, 0x70, 0xF0, 0xF0, 0xE0, 0xE0, 0xE2, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00,	0x21, 0x30, 0x1C, 0x1E, 0x1F, 0x0F, 0x0F, 0x8F, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x01},
	{0x00, 0x00, 0x00, 0x00, 0x1C, 0xF8, 0xF0, 0xE2, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,	0x09, 0x08, 0x0C, 0x0C, 0x0E, 0x0E, 0x0F, 0x8F, 0x0F, 0x0F, 0x0F, 0x0E, 0x0C, 0x08, 0x01},
	{0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xE0, 0xFC, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x8F, 0x0F, 0x1F, 0x1F, 0x3C, 0x30, 0x40, 0x01},
	{0x00, 0x20, 0x60, 0xE0, 0xC0, 0xC0, 0xC0, 0x82, 0xE0, 0xF8, 0xFE, 0x00, 0x00, 0x00, 0x00,	0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x8F, 0x1F, 0x3F, 0x7F, 0x00, 0x00, 0x00, 0x01},

	{0x00, 0x00, 0x06, 0x3C, 0xF8, 0xF8, 0xF0, 0xE2, 0xF0, 0xF8, 0xF8, 0x3C, 0x06, 0x00, 0x00,	0x01, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x3F, 0xFF, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x01},
    {0x00, 0x00, 0x00, 0x00, 0xFE, 0xF8, 0xE0, 0x82, 0xC0, 0xC0, 0xC0, 0xE0, 0x60, 0x20, 0x00,	0x01, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x1F, 0x8F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0xE0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,	0x01, 0x40, 0x30, 0x3C, 0x1F, 0x1F, 0x0F, 0x8F, 0x07, 0x07, 0x03, 0x03, 0x01, 0x01, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE2, 0xF0, 0xF8, 0x1C, 0x00, 0x00, 0x00, 0x00,	0x01, 0x08, 0x0C, 0x0E, 0x0F, 0x0F, 0x0F, 0x8F, 0x0F, 0x0E, 0x0E, 0x0C, 0x0C, 0x08, 0x09},

	{0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE2, 0xE0, 0xE0, 0xF0, 0xF0, 0x70, 0x18, 0x08,	0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x07, 0x8F, 0x0F, 0x0F, 0x1F, 0x1E, 0x1C, 0x30, 0x21},
	{0x00, 0x20, 0x60, 0xE0, 0xE0, 0xE0, 0xE0, 0xE2, 0xE0, 0xE0, 0xE0, 0x60, 0x60, 0x20, 0x20,	0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x8F, 0x1F, 0x3E, 0x70, 0x00, 0x00, 0x00, 0x01},
	{0x00, 0x04, 0x18, 0x78, 0xF0, 0xF0, 0xE0, 0xE2, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00,	0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0x7F, 0x0F, 0x03, 0x03, 0x01, 0x01, 0x01, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0xFC, 0xF8, 0xF0, 0xE2, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,	0x01, 0x00, 0x00, 0x00, 0xFF, 0x3F, 0x0F, 0x83, 0x07, 0x07, 0x07, 0x0E, 0x0C, 0x08, 0x01},

	{0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE2, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00,	0x01, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x8F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x01}
};

const U8 bmp1[2][8 * 128] =
{
	{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x80,0xC0,0xE0,0x30,0x38,0x18,0x08,0x6C,0xE4,0xF4,0xFE,0x5E,0x6E,0x6E,0x66,
0x20,0x20,0x20,0x30,0x38,0x38,0xB8,0xB8,0xA0,0xE0,0xE0,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0x3C,
0xFF,0xFB,0xE9,0x70,0x18,0x0C,0x06,0x03,0x19,0x1D,0x0C,0x06,0x02,0x00,0x00,0x00,
0x00,0x00,0xF0,0xFE,0x1F,0x0F,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x70,0xFC,0x3C,0xFC,0xF4,0x34,
0x77,0xF3,0xA3,0xE3,0xC3,0x43,0xE3,0xE3,0xE3,0xE6,0xE6,0xE6,0x46,0xC7,0xFF,0x1E,
0x67,0x79,0x1C,0x44,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0xC0,0xD0,0xF8,0xF0,0xBC,
0xBF,0xA3,0xB3,0xA6,0x26,0x26,0x24,0x84,0xC4,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,
0x80,0x80,0xC0,0xE0,0xF0,0x30,0x30,0xE0,0xF0,0x38,0x38,0x30,0x1E,0x1F,0x1D,0x39,
0x60,0x61,0x23,0x63,0xEF,0xEF,0xFF,0xFF,0x7F,0x3F,0x3F,0x1F,0xFF,0xFF,0x01,0x80,
0x80,0x80,0x80,0xE8,0xFC,0xB0,0xB8,0x3C,0x2F,0x3E,0x3F,0x1F,0x15,0x05,0x07,0x06,
0x06,0x06,0x06,0x06,0x06,0x02,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x06,0x3F,0x7F,0xC3,0xE2,0x73,0x1B,0x0F,0x07,0x07,0x03,
0x07,0x03,0x03,0x00,0x01,0x03,0x3E,0x7D,0xF3,0x7E,0x0C,0x0C,0x0C,0x18,0x30,0x70,
0xE0,0xF8,0x3C,0x0E,0x07,0x01,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x30,0x19,
0x01,0x01,0x01,0x00,0x00,0xC3,0xC3,0xC7,0xCE,0x5E,0x78,0x78,0x78,0x30,0x30,0x30,
0x18,0x18,0x18,0x18,0x18,0x08,0x18,0x18,0x18,0x10,0x30,0x30,0x60,0xC0,0xC0,0x80,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0x60,0x30,0x10,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0C,0x18,0x30,0x70,0xE0,0xC0,0x80,
0x00,0x02,0x00,0x80,0xC0,0xE0,0xF8,0x1C,0x0E,0x03,0x03,0x3F,0x7E,0xF0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xC0,0xC0,0xE0,0xE0,0xC0,0x40,0x40,0x40,0x40,0xC0,0xC0,0xC0,0xC0,0x80,0xC1,0xC3,
0xC3,0xC7,0xC6,0xC6,0x66,0x63,0x33,0x3B,0x1D,0x0E,0x03,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
0x03,0x03,0x03,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x7F,0xF0,
0xC0,0x06,0x38,0x70,0xE0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,
0x81,0xC0,0xC0,0x40,0x60,0x30,0x30,0x90,0x98,0xC8,0x6C,0x3C,0x1E,0x0E,0x86,0x82,
0x82,0xC2,0xC0,0xC0,0xC0,0xF8,0x68,0x68,0x68,0x68,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x03,0x07,0x0E,0x0C,0x18,0x10,0x31,0x21,0x61,0x63,0x63,0x63,0x63,0x61,0x61,0x31,
0x31,0x30,0x18,0x1C,0x1C,0x1E,0x1F,0x1B,0x0D,0x0C,0x06,0x1E,0x1F,0x0F,0x07,0x07,
0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},
	{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x70,0xFC,0x3C,0xFC,0xF4,0x34,
0x77,0xF3,0xA3,0xE3,0xC3,0x43,0xE3,0xE3,0xE3,0xE6,0xE6,0xE6,0x46,0x47,0x5F,0x1E,
0x46,0x74,0x96,0x16,0x57,0x77,0x35,0x36,0x24,0xC6,0x55,0x95,0xC6,0x86,0xD6,0xB6,
0xB4,0xA6,0xB2,0xA6,0x26,0x26,0x24,0x84,0xC4,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,
0x80,0x80,0xC0,0xE0,0xF0,0x30,0x30,0xE0,0xF0,0x38,0x38,0x30,0x1E,0x1F,0x1D,0x39,
0x60,0x61,0x23,0x63,0xEF,0xEF,0xFF,0xFF,0x7F,0x3F,0x3F,0x1F,0x01,0x01,0xFE,0xFE,
0x00,0x82,0x03,0x02,0x42,0x0A,0x02,0x13,0x29,0x7C,0xFD,0x9E,0x97,0x07,0x07,0x06,
0x06,0x06,0x06,0x06,0x06,0x02,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x06,0x3F,0x7F,0xC3,0xE2,0x73,0x1B,0x0F,0x07,0x07,0x03,
0x07,0x03,0x03,0x00,0x01,0x03,0x3E,0x7D,0xF3,0x7E,0x0C,0x0C,0x0C,0x18,0x30,0x70,
0xE0,0xF8,0x3C,0x0E,0x07,0x01,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x30,0x1B,
0x0E,0x30,0x41,0xA4,0x0C,0x18,0x10,0x40,0x00,0x40,0x00,0x48,0x51,0x2E,0x24,0x38,
0x18,0x78,0x98,0x18,0x18,0x08,0x18,0x18,0x18,0x10,0x30,0x30,0x60,0xC0,0xC0,0x80,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0x60,0x30,0x10,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0C,0x18,0x30,0x70,0xE0,0xC0,0x80,
0x00,0x02,0x00,0x80,0xC0,0xE0,0xF8,0x1C,0x0E,0x03,0x03,0x3F,0x7E,0xF0,0x00,0x00,
0x00,0x00,0x00,0x01,0x02,0x04,0x09,0x11,0x20,0x20,0x20,0x60,0x60,0xC0,0x80,0x40,
0xC0,0xC0,0xC1,0xC1,0xC1,0xC2,0xA8,0xF4,0x58,0xC8,0xC0,0xC0,0xC0,0x80,0xC1,0xC3,
0xC3,0xC7,0xC6,0xC6,0x66,0x63,0x33,0x3B,0x1D,0x0E,0x03,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
0x03,0x03,0x03,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x7F,0xF0,
0xC0,0x06,0x38,0x70,0xE0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,
0x81,0xC4,0xC0,0x48,0x68,0x38,0x39,0x99,0x9B,0xCB,0x6E,0x3E,0x1E,0x0E,0x86,0x82,
0x82,0xC2,0xC0,0xC0,0xC0,0xF8,0x68,0x68,0x68,0x68,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x03,0x07,0x0E,0x0C,0x18,0x10,0x31,0x21,0x61,0x63,0x63,0x63,0x63,0x61,0x61,0x31,
0x31,0x30,0x18,0x1C,0x1C,0x1E,0x1F,0x1B,0x0D,0x0C,0x06,0x1E,0x1F,0x0F,0x07,0x07,
0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	}
};

#endif	// LCD

