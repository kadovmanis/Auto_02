#include	"Spi_i2c.h"
#include	"Ports.h"
#include	"Timers.h"

// _________________________________SPI2 functions__________________________________________________
#define	I2C_WRITE				0x00
#define	I2C_READ				0x01

#define	SPI1STAT_VAL			0b0000000000010100	// SPI1STAT Register
#define	SPI2STAT_VAL			0b0000000000001000;	// SPI2STAT Register
#define	SPI3STAT_VAL			0b0000000000010100	// SPI3STAT Register
							//	  |||||||||||||||+	-- SPIRBF: SPIx Receive Buffer Full Status
							//	  ||||||||||||||+-	-- SPITBF: SPIx Transmit Buffer Full Status bit
							//	  |||||||||||+++--	-- SISEL<2:0>: SPIx Buffer Interrupt Mode bits (valid in Enhanced Buffer mode)	101=Interrupt when the last bit is shifted out of SPIxSR; now the transmit is complete
							//	  |||||||||||+++--	-- 																				110 = Interrupt when the last bit is shifted into SPIxSR; as a result, the TX FIFO is empty
							//	  ||||||||||+-----	-- SRXMPT: Receive FIFO Empty bit (valid in Enhanced Buffer mode)
							//	  |||||||||+------	-- SPIROV: Receive Overflow Flag
							//	  ||||||||+-------	-- SRMPT: Shift Register (SPIxSR) Empty bit (valid in Enhanced Buffer mode)
							//	  |||||+++--------	-- SPIBEC<2:0>: SPIx Buffer Element Count bits (valid in Enhanced Buffer mode)
							//	  |||++-----------	-- Unimplemented
							//	  ||+-------------	-- SPISIDL: Stop in Idle Mode
							//	  |+--------------	-- Unimplemented
							//	  +---------------	-- SPIEN: SPIx Enable

#define	SPI1CON1_VAL			0b0000000001111111	// SPI1CON1 Register
#define	SPI2CON1_VAL_CLK_HI		0b0000100001011111	// SPI2CON1 Register if SCK Idle state - high level
#define	SPI2CON1_VAL_CLK_LO		0b0000100000011111	// SPI2CON1 Register if SCK Idle state - low level
#define	SPI3CON1_VAL			0b0000000001111111;	// SPI3CON1 Register
							//	  ||||||||||||||++	-- PPRE<1:0>: Primary Prescale bits (Master mode)	(1:1)
							//	  |||||||||||+++--	-- SPRE<2:0>: Secondary Prescale bits (Master mode)	(1:2)
							//	  ||||||||||+-----	-- MSTEN: Master Mode Enable
							//	  |||||||||+------	-- CKP: Clock Polarity Select (1 = SCK Idle state - high level)
							//	  ||||||||+-------	-- SSEN: Slave Select Enable
							//	  |||||||+--------	-- CKE: SPIx Clock Edge Select (0- SDO data CHANGES on Idle to Active CLK)
							//	  ||||||+---------	-- SMP: SPIx Data Input Sample Phase (0 = SDI sampled at the middle of data output time; 1 = at the end)
							//	  |||||+----------	-- MODE16: Word/Byte Communication Select (0 = 8bit, 1 = 16bit)
							//	  ||||+-----------	-- DISSDO: Disable SDOx Pin bit
							//	  |||+------------	-- DISSCK: Disable SCKx Pin bit (SPI Master modes only)
							//	  +++-------------	-- Unimplemented

#define	SPI1CON2_VAL			0b0000000000000001	// SPI1CON2 Register
#define	SPI2CON2_VAL			0b0000000000000001	// SPI2CON2 Register
//		SPI2CON2_VAL			0b0000000000000000	// SPI2CON2 Register
#define	SPI3CON2_VAL			0b0000000000000001	// SPI3CON2 Register
							//	  |||||||||||||||+	-- SPIBEN: Enhanced Buffer Enable
							//	  ||||||||||||||+-	-- SPIFE: Frame Sync Pulse Edge Select
							//	  |||+++++++++++--	-- Unimplemented
							//	  ||+-------------	-- SPIFPOL: Frame Sync Pulse Polarity bit (Frame mode only)
							//	  |+--------------	-- SPIFSD: Frame Sync Pulse Direction Control on SSx Pin
							//	  +---------------	-- FRMEN: Framed SPIx Support

void Spi1_Init(void)
{
	SPI1STAT = SPI1STAT_VAL;
	SPI1CON1 = SPI1CON1_VAL;
	SPI1CON2 = SPI1CON2_VAL;
	_SPI1IP		= 3;				// SPI1 Interrupt priority level=3
	_SPI1IF		= 1;				// Clear SPI1 Interrupt Flag
	_SPI1IE		= 0;				// Clear SPI1 Interrupt
	SPI1STATbits.SPIEN	= 1;		// Enable SPI1
}

void Spi3_Init(void)
{
	SPI3STAT = SPI3STAT_VAL;
	SPI3CON1 = SPI3CON1_VAL;
	SPI3CON2 = SPI3CON2_VAL;
	_SPI3IP		= 3;				// SPI3 Interrupt priority level=3
	_SPI3IF		= 0;				// Clear SPI3 Interrupt Flag
	_SPI3IE		= 0;				// Clear SPI3 Interrupt
	SPI3STATbits.SPIEN	= 1;		// Enable SPI3
}

void Spi2_Init(void)
{
	SPI2STAT = SPI2STAT_VAL;
//	SPI2CON1 = (FLAG_IDLE_PR_CLK)?	SPI2CON1_VAL_CLK_HI : SPI2CON1_VAL_CLK_LO;
	SPI2CON1 = SPI2CON1_VAL_CLK_HI;
	SPI2CON2 = SPI2CON2_VAL;
	_SPI2IP		= 5;				// SPI1 Interrupt priority level=5
	_SPI2IF		= 0;				// Clear SPI2 Interrupt Flag
	_SPI2IE		= 1;				// Clear SPI2 Interrupt
	SPI2STATbits.SPIEN	= 1;		// Enable SPI2
}

//	*********************************************************************
//	*							SPI1 INTERRUPT							*
//	*********************************************************************
/*
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)
{
	_SPI1IF	= 0;				// Clear SPI1 Interrupt Flag
	SPI1STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
//	Mem1Irq();
}
*/
//	*********************************************************************
//	*							SPI2 INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _SPI2Interrupt(void)
{
	_SPI2IF	= 0;				// Clear SPI2 Interrupt Flag
	SPI2STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
//	Printer_SPI_IRQ();
}

//	*********************************************************************
//	*							SPI3 INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _SPI3Interrupt(void)
{
	_SPI3IF	= 0;				// Clear SPI3 Interrupt Flag
	SPI3STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
//	Mem2Irq();
}

void Spi1Clear (void)
{
	register U8 a;
	while(SPI1_BUSY())		{a += SPI1BUF;}
	SPI1STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
}

void Spi3Clear (void)
{
	register U8 a;
	while(SPI3_BUSY())		{a += SPI3BUF;}
	SPI3STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
}

void Spi2Clear (void)
{
	U8 a,i = 8;
	while(i--)
		a += SPI2BUF;			// to clear SPIRBF (Receive Buffer Full Status bit)
}

U8	Spi1Byte (U8 data)
{
	U8 a = SPI1BUF;				// to clear SPIRBF (Receive Buffer Full Status bit)
	SPI1BUF = data;
	while (SPI1STATbits.SRXMPT) a++;
	a = SPI1BUF;
	return a;
}

U8	Spi2Byte (U8 data)
{
	U8 a = SPI2BUF;				// to clear SPIRBF (Receive Buffer Full Status bit)
	SPI2BUF = data;
	while (!SPI2STATbits.SPIRBF) a++;
	a = SPI2BUF;
	return a;
}

U8	Spi3Byte (U8 data)
{
	U8 a = SPI3BUF;				// to clear SPIRBF (Receive Buffer Full Status bit)
	SPI3BUF = data;
	while (SPI3STATbits.SRXMPT) a++;
	a = SPI3BUF;
	return a;
}

/* _________________________________I2C functions___________________________________________________
#define	I2C_WRITE				0x00
#define	I2C_READ				0x01

inline void I2c2Start	(void);
inline void I2c2Stop		(void);
inline U8	I2c2Send		(U8 data);
inline U8	I2c2Get		(U8 ack);
inline void I2c2AltStart	(void);
inline void I2c2AltStop	(void);
inline U8	I2c2AltSend	(U8 data);
inline U8	I2c2AltGet	(U8 ack);

inline void I2c2Start(void)
{
	I2C2_SCL_HI();	DELAY_2uS();
	I2C2_SDA_HI();	DELAY_2uS();
	I2C2_SDA_LO();	DELAY_2uS();
	I2C2_SCL_LO();	DELAY_2uS();
}

inline void I2c2Stop(void)
{
	I2C2_SDA_LO();	DELAY_2uS();
	I2C2_SCL_HI();	DELAY_2uS();
	I2C2_SDA_HI();	DELAY_2uS();
}

// return	1 if ack = OK
//			0 if ack = NOK
inline U8	I2c2Send(U8 data)
{
	U8 i = 0x80;
	while (i)
	{
		I2C2_SDA = (i & data)? 1:0;
		DELAY_I2C();
		I2C2_SCL_HI();	DELAY_I2C();
		I2C2_SCL_LO();	DELAY_I2C();
		i >>= 1;
	}
	I2C2_SDA_IN();
	DELAY_I2C();
	I2C2_SCL_HI();		DELAY_I2C();
	if (!I2C2_SDA_INPUT)	i = 1;
	I2C2_SCL_LO();		DELAY_I2C();
	return i;
}

// ack > 0 : OK
// ack = 0 : NOK
inline U8	I2c2Get(U8 ack)
{
	U8 i = 0x80, data = 0;
	I2C2_SDA_IN();
	while (i)
	{
		I2C2_SCL_HI();		DELAY_I2C();
		if (I2C2_SDA_INPUT)	data |= i;
		I2C2_SCL_LO();		DELAY_2uS();
		i >>= 1;
	}
	if (ack)		I2C2_SDA_LO();
	I2C2_SCL_HI();	DELAY_I2C();
	I2C2_SCL_LO();	DELAY_I2C();
	return data;
}

U8	I2c2Test(U8 data)
{

	U8 rc = 0x0F;								// OK
	I2c2Start();
	if (!I2c2Send(data))	rc = 0xEE;			// NOK
	I2c2Stop();
	return rc;
}

// _________________________________I2C functions for MPU6000/Compass_______________________________
U8	I2c2WriteByte(U8 slave, U8 address, U8 data)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_WRITE))					return 0;
	if (!I2c2Send(address))								return 0;
	if (!I2c2Send(data))								return 0;
	I2c2Stop();
	return 1;
}

U8	I2c2Write(U8 slave, U8 address, P_U8 data, U8 len)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_WRITE))					return 0;
	if (!I2c2Send(address))								return 0;
	while (len)
	{
		if (!I2c2Send(*data++))							return 0;
		len--;
	}
	I2c2Stop();
	return 1;
}

U8	I2c2Read(U8 slave, P_U8 data, U8 len)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_READ))					return 0;
	while (len)
	{
		U8 ack = (--len)? 1:0;
		*data++ = I2c2Get(ack);
	}
	I2c2Stop();
	return 1;
}

U8	I2c2ReadReverse(U8 slave, P_U8 data, U8 len)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_READ))					return 0;
	while (len)
	{
		U8 ack = (--len)? 1:0;
		data[len] = I2c2Get(ack);
	}
	I2c2Stop();
	return 1;
}

U8	I2c2ReadFrom(U8 slave, U8 address, P_U8 data, U8 len)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_WRITE))					return 0;
	if (!I2c2Send(address))								return 0;
	return I2c2Read(slave, data, len);
}

U8	I2c2ReadReverseFrom(U8 slave, U8 address, P_U8 data, U8 len)
{
	I2c2Start();
	if (!I2c2Send(slave | I2C_WRITE))					return 0;
	if (!I2c2Send(address))								return 0;
	return I2c2ReadReverse(slave, data, len);
}
*/
