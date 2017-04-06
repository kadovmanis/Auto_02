#ifndef	_lcd_fonts_h_
#define	_lcd_fonts_h_

//#define	DISPLAY_BLUE

#include	"Typedef.h"
#include	"Fonts.h"


#if (LCD == LCD_RGB)
 #define	DISP_WITH_PIX				96
 #define	DISP_HIGH_PIX				64
 #define	DISP_HIGH_BYTES				8
 #define	DISP_PIX_BYTES				2

 #define DRAW_LINE                       0x21
 #define DRAW_RECTANGLE                  0x22
 #define COPY_WINDOW                     0x23
 #define DIM_WINDOW                      0x24
 #define CLEAR_WINDOW                    0x25
 #define FILL_WINDOW                     0x26
 #define DISABLE_FILL                    0x00
 #define ENABLE_FILL                     0x01
 #define CONTINUOUS_SCROLLING_SETUP      0x27
 #define DEACTIVE_SCROLLING              0x2E
 #define ACTIVE_SCROLLING                0x2F

 #define SET_COLUMN_ADDRESS              0x15
 #define SET_ROW_ADDRESS                 0x75
 #define SET_CONTRAST_A                  0x81
 #define SET_CONTRAST_B                  0x82
 #define SET_CONTRAST_C                  0x83
 #define MASTER_CURRENT_CONTROL          0x87
 #define SET_PRECHARGE_SPEED_A           0x8A
 #define SET_PRECHARGE_SPEED_B           0x8B
 #define SET_PRECHARGE_SPEED_C           0x8C
 #define SET_DISPLAY_START_LINE          0xA1
 #define SET_DISPLAY_OFFSET              0xA2
 #define NORMAL_DISPLAY                  0xA4
 #define ENTIRE_DISPLAY_ON               0xA5
 #define ENTIRE_DISPLAY_OFF              0xA6
 #define INVERSE_DISPLAY                 0xA7
 #define DIM_MODE_SETTING                0xAB
 #define SET_MASTER_CONFIGURE            0xAD
 #define DIM_MODE_DISPLAY_ON             0xAC
 #define POWER_SAVE_MODE                 0xB0
 #define PHASE_PERIOD_ADJUSTMENT         0xB1
 #define DISPLAY_CLOCK_DIV               0xB3
 #define SET_GRAy_SCALE_TABLE            0xB8
 #define ENABLE_LINEAR_GRAY_SCALE_TABLE  0xB9
 #define SET_PRECHARGE_VOLTAGE           0xBB
 #define SET_V_VOLTAGE                   0xBE

#else
 #define	DISP_WITH_PIX				128
 #define	DISP_HIGH_PIX				64
 #define	DISP_HIGH_BYTES				8
 #define	DISP_PIX_BYTES				1

 #define	COMAND0						0xAA
 #define	COMAND1						0x55

 #define	X_START						0xB0
 #define	X_LAST						0xB7
 #define	Y_START_H					0x10
 #if	(LCD == LCD_GREEN)
	#define	Y_START_L					0x03
 #else	// LCD_BLUE
	#define	Y_START_L					0x00
 #endif
 #define	BOOSTER_4X_L				0x00
 #define	INTERNAL_VOLTAGE_5			0x25
 #define	INTERNAL_VOLTAGE_7			0x27
 #define	POWER_CONTROL_CMD			0x2F
 #define	START_ADDR					0x40
 #define	EL_VOLUME_MODE				0x81
 #define	ADC_1						0xA1
 #define	LCD_BIAS_SET_0				0xA2
 #define	LCD_BIAS_SET_1				0xA3
 #define	NORMAL_INDICATION			0xA6
 #define	FULL_INDICATION				0xA7
 #define	INDICATOR					0xAC
 #define	COM_OUT_DIR_RIGHT			0xC0
 #define	COM_OUT_DIR_LEFT			0xC8
 #define	CLOCK_RATIO					0xD5
 #define	LCD_RESET_CMD				0xE2
 #define	BOOSTER_RATIO				0xF8
 #define	SET_ADDRESS_X				0xFD
 #define	SET_ADDRESS_Y_H				0xFE
 #define	SET_ADDRESS_Y_L				0xFF
#endif

#define	SET_REMAP_DATA_FORMAT			0xA0
#define	MULTIPLEX_RATIO					0xA8
#define	LCD_OFF_CMD						0xAE
#define	LCD_ON_CMD						0xAF

#define	LCD_STATE_LINE					0
#define	LCD_STATE_ADDRESS				1
#define	LCD_STATE_INIT					3

 #define	SPACE						1
 #define	CHAR_7x5_WIDTH				5
 #define	CHAR_7x5_SPACE				1
 #define	CHAR_24x15_HIGH				3
 #define	CHAR_24x15_WIDTH			15
 #define	CHAR_40x21_HIGH				5
 #define	CHAR_40x21_WIDTH			21

U8 SwitchCode(U8 ch);

extern	const FONT_INFO* Fonts[];
extern	const U8	LcdInitSequence[];
//extern	const U8	LcdClearSequence[DISP_WITH_PIX];
extern	const U8 font7x5[0x80][CHAR_7x5_WIDTH];
//extern	const U8 font24x15[10][CHAR_24x15_HIGH][CHAR_24x15_WIDTH]; 
//extern	const U8 font40x21[10][CHAR_40x21_HIGH][CHAR_40x21_WIDTH];
//extern	const U8 arrows[17][30];
//extern	const U8 bmp1[2][8 * 128];
//extern	const U8 bigDot[8];
//extern	const U8 round[6];

#endif	// _lcd_fonts_h_

