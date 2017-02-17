#ifndef		__CONFIG_BITS_H__
#define		__CONFIG_BITS_H__

// PIC24FJ128GB206 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#ifdef	BOOT_LOADER
// CONFIG1
	// CONFIG1( JTAGEN_OFF		& GCP_OFF			& GWRP_OFF		& ICS_PGx2			& FWDTEN_OFF	& WINDIS_OFF	& FWPSA_PR128	& WDTPS_PS32768	)
	#pragma	config WDTPS	= PS32768	// Watchdog Timer Postscaler (1:32,768)
	#pragma	config FWPSA	= PR128		// WDT Prescaler (Prescaler ratio of 1:128)
	#pragma	config WINDIS	= OFF		// Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
	#pragma	config FWDTEN	= OFF		// Watchdog Timer (Watchdog Timer is disabled)
	#pragma	config ICS		= PGx2		// Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
	#pragma	config GWRP		= OFF		// General Segment Write Protect (Writes to program memory are allowed)
	#pragma	config GCP		= OFF		// General Segment Code Protect (Code protection is disabled)
	#pragma	config JTAGEN	= OFF		// JTAG Port Enable (JTAG port is disabled)
#else
// CONFIG1
	// _CONFIG1( JTAGEN_OFF		& GCP_ON			& GWRP_OFF		& ICS_PGx2			& FWDTEN_OFF	& WINDIS_OFF	& FWPSA_PR128	& WDTPS_PS32768	)
	#pragma	config WDTPS	= PS32768	// Watchdog Timer Postscaler (1:32,768)
	#pragma	config FWPSA	= PR128		// WDT Prescaler (Prescaler ratio of 1:128)
	#pragma	config WINDIS	= OFF		// Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
	#pragma	config FWDTEN	= OFF		// Watchdog Timer (Watchdog Timer is disabled)
	#pragma	config ICS		= PGx2		// Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
	#pragma	config GWRP		= OFF		// General Segment Write Protect (Writes to program memory are allowed)
	#pragma	config GCP		= ON		// General Segment Code Protect (Code protection is enabled)
	#pragma	config JTAGEN	= OFF		// JTAG Port Enable (JTAG port is disabled)
#endif
// CONFIG2
// _CONFIG2( IESO_ON			& PLLDIV_NODIV		& PLL96MHZ_OFF	& FNOSC_FRC			& FCKSM_CSECMD	& OSCIOFNC_ON	& IOL1WAY_OFF	& POSCMOD_NONE	)
#pragma	config POSCMOD		= NONE		// Primary Oscillator Select (Primary oscillator is disabled)
#pragma	config IOL1WAY		= OFF		// IOLOCK One-Way Set Enable (The IOLOCK bit can be set and cleared as needed, provided the unlock sequence has been completed)
#pragma	config OSCIOFNC		= ON		// OSCO Pin Configuration (OSCO/CLKO/RC15 functions as port I/O (RC15))
#pragma	config FCKSM		= CSECMD	// Clock Switching and Fail-Safe Clock Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)
#pragma	config FNOSC		= FRC		// Initial Oscillator Select (Fast RC Oscillator (FRC))
#pragma	config PLL96MHZ		= OFF		// 96MHz PLL Startup Select (96 MHz PLL is software controlled (can be enabled by setting the PLLEN bit in CLKDIV<5>))
#pragma	config PLLDIV		= NODIV		// 96 MHz PLL Prescaler Select (Oscillator input is used directly (4 MHz input))
#pragma	config IESO			= ON		// Internal External Switchover (IESO mode (Two-Speed Start-up) is enabled)

// CONFIG3
// _CONFIG3( WPEND_WPENDMEM	& WPCFG_WPCFGDIS	& WPDIS_WPDIS	& ALTPMP_ALPMPDIS	& WUTSEL_FST	& SOSCSEL_EC	& WPFP_WPFP255					)
#pragma	config WPFP			= WPFP255	// Write Protection Flash Page Segment Boundary (Highest Page (same as page 85))
#pragma	config SOSCSEL		= EC		// Secondary Oscillator Power Mode Select (External clock (SCLKI) or Digital I/O mode()
#pragma	config WUTSEL		= FST		// Voltage Regulator Wake-up Time Select (Fast regulator start-up time is used)
#pragma	config ALTPMP		= ALPMPDIS	// Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma	config WPDIS		= WPDIS		// Segment Write Protection Disable (Segmented code protection is disabled)
#pragma	config WPCFG		= WPCFGDIS	// Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma	config WPEND		= WPENDMEM	// Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

//	_CONFIG4( 0xFFFF )

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.




#endif

