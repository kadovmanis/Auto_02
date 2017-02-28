#ifndef	_typedef_h_
#define	_typedef_h_

#include	"Define.h"
#include	<stdbool.h>
#include	<stdint.h>
#include	<float.h>
#include	<libpic30.h>

#define	true				1
#define	false				0
#ifndef	NULL
	#define	NULL			0
#endif	// NULL
#define	ON					1
#define	OFF					0

#define CR					'\r'
#define LF					'\n'
#define TAB					'\t'
#define SP					' '
#define ESC					27
#define CTRL_Z				0x1A
#define	STX					0x02		// STX (start of text) used in GPRS packets

typedef unsigned	char	U8, *P_U8;	//, NEAR *NP_U8, FAR;	/*u8SampleVariable, pu8SamplePointer*/
typedef signed		char	S8;			//, NEAR *NP_S8, FAR *P_S8;		/*s8SampleVariable, ps8SamplePointer*/
typedef	const		char	R8,	*P_R8;
//typedef	const rom	U8	RU8, *P_RU8;
//typedef	const rom	S8	RS8, *P_RS8;

//short, signed short 16 -32768 32767
//unsigned short 16 0 65535
typedef unsigned	int		U16, *P_U16;	//, NEAR *NP_U16, FAR;	/*u16SampleVariable, pu16SamplePointer*/
typedef signed		int		S16, *P_S16;	//, NEAR *NP_S16, FAR *P_S16;	/*s16SampleVariable, ps16SamplePointer*/
typedef	const		int		R16, *P_R16;
//typedef	const rom	U16	RU16, *P_RU16;
//typedef	const rom	S16	RS16, *P_RS16;

//typedef	short	long	S24;
//typedef	unsigned short long	U24;
//typedef	const rom	U24	RU24, *P_RU24;
//typedef	const rom	S24	RS24, *P_RS24;

typedef unsigned	long	U32, *P_U32;
typedef	signed		long	S32;
//typedef	const rom	U32	RU32, *P_RU32;
//typedef	const rom	S32	RS32, *P_RS32;

typedef unsigned long long	U64, *P_U64;
typedef	signed long long	S64;

typedef float				F32,		*P_F32;
typedef double				D32, F64,	*P_D32;
typedef long double			D64,		*P_D64;
//float						32		-126 127 2-126 2128
//double* 					32		-126 127 2-126 2128
//long double 				64		-1022 1023 2-1022 21024


typedef union
{
	U16 word;
	struct
	{
		U8	u8l;
		U8	u8h;
	};
	struct
	{
		U16 flag_0	:1;
		U16 flag_1	:1;
		U16 flag_2	:1;
		U16 flag_3	:1;
		U16 flag_4	:1;
		U16 flag_5	:1;
		U16 flag_6	:1;
		U16 flag_7	:1;
		U16 flag_8	:1;
		U16 flag_9	:1;
		U16 flag_10	:1;
		U16 flag_11	:1;
		U16 flag_12	:1;
		U16 flag_13	:1;
		U16 flag_14	:1;
		U16 flag_15	:1;
	};
} FLAGS;

typedef union
{
	U8 byte;
	struct
	{
		U8 flag_0	:1;
		U8 flag_1	:1;
		U8 flag_2	:1;
		U8 flag_3	:1;
		U8 flag_4	:1;
		U8 flag_5	:1;
		U8 flag_6	:1;
		U8 flag_7	:1;
	};
} FLAGS_8;

typedef union
{
	U16	u16;
	S16	s16;
	struct
	{
		U8	u8l;
		U8	u8h;
	};
	struct
	{
		S8	s8l;
		S8	s8h;
	};
} UNI16;

typedef union
{
	U32	u32;
	S32	s32;
	struct
	{
		U16	u16l;
		U16	u16h;
	};
	struct
	{
		S16	s16l;
		S16	s16h;
	};

	struct
	{
		U8	u8ll;
		U8	u8lh;
		U8	u8hl;
		U8	u8hh;
	};
	struct
	{
		S8	s8ll;
		S8	s8lh;
		S8	s8hl;
		S8	s8hh;
	};
	struct
	{
		FLAGS fl_l;
		FLAGS fl_h;
	};
} UNI32;

#define	U32_SIGN(X)		((UNI32)(X).fl_h.flag_15)
typedef enum
{
	Osc_8__MHz	= 0,	// 0b000 Fast RC Oscillator (FRC)
	Osc_32_MHz,			// 0b001 Fast RC Oscillator with Postscaler and PLL module (FRCPLL)
	Osc_0_Pri,			// 0b010 Primary Oscillator (XT, HS, EC)
	Osc_0_PriPLL,		// 0b011 Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL)
	Osc_0_SOSC,			// 0b100 Secondary Oscillator (SOSC)
	Osc_31_kHz,			// 0b101 Low-Power RC Oscillator (LPRC)
	Osc_05_MHz,			// 0b110 Fast RC Oscillator/16 (500 KHz)
	Osc_4__MHz,			// 0b111 Fast RC Oscillator with Postscaler (FRCDIV)
} OSC;

typedef enum
{
	BaudRate_9600 = 0,	// 
	BaudRate_19200,		// 
	BaudRate_38400,		// 
	BaudRate_57600,		//
	BaudRate_115200,	// 
	BaudRate_none		// 
} BAUDRATE;

#define	BAUD_9600		((FCY / 16) / 9600 ) - 1
#define	BAUD_19200		((FCY / 16) / 19200 ) - 1
#define	BAUD_38400		((FCY / 16) / 38400 ) - 1
#define	BAUD_115200		((FCY / 16) / 115200 ) - 1

#define Lo(X)			(U8)(X & 0x00FF)
#define Hi(X)			(U8)((X>>8) & 0x00FF)
#define	U16(X)			Lo(X), Hi(X)
#define	U32(X)			(U8)((X>>0) & 0x00FF), (U8)((X>>8) & 0x00FF), (U8)((X>>16) & 0x00FF), (U8)((X>>24) & 0x00FF)
#define	IS_DIGIT(X)		((X >= '0') && (X <= '9'))
#define	IS_NOT_DIGIT(X)	((X <  '0') || (X >  '9'))
#define FLOAT_TO_S16(X) (((X)>=0)?	(S16)((X) + 0.5): (S16)((X) - 0.5))

#define	SHIFT_DIVIDE____2(X)	((X)>>= 1)
#define	SHIFT_DIVIDE____4(X)	((X)>>= 2)
#define	SHIFT_DIVIDE____8(X)	((X)>>= 3)
#define	SHIFT_DIVIDE___16(X)	((X)>>= 4)
#define	SHIFT_DIVIDE___32(X)	((X)>>= 5)
#define	SHIFT_DIVIDE___64(X)	((X)>>= 6)
#define	SHIFT_DIVIDE__128(X)	((X)>>= 7)
#define	SHIFT_DIVIDE__256(X)	((X)>>= 8)
#define	SHIFT_DIVIDE__512(X)	((X)>>= 9)
#define	SHIFT_DIVIDE_1024(X)	((X)>>= 10)

#define	U16_TO_U8_DATA(X)		((X >> 8) & 0xFF),	(X & 0xFF)
#define	U24_TO_U8_DATA(X)		((X >> 16) & 0xFF), ((X >> 8) & 0xFF),	(X & 0xFF)
#define	U32_TO_U8_DATA(X)		((X >> 24) & 0xFF), ((X >> 16) & 0xFF),	((X >> 8) & 0xFF),	(X & 0xFF)

#define DIFFERENCE(X,Y)			(X > Y)?	(X - Y) : (Y - X)

extern  OSC 			Oscillator;
extern	char			Build_Date[];
extern	char			Build_Time[];
extern	volatile U16	LedStatus1, LedStatus2, LedStatus3, LedStatus4;

extern	void	StopOnError (const char* msg, const char* errFile, int errLine);
#endif

