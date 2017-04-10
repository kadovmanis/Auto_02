#ifndef	_w5100_h_
#define	_w5100_h_

typedef enum
{
	W5100_UNCNOWN = 0,
	W5100_DISCONNECTED,
	W5100_INIT_REQUIRED,
	W5100_ACTIVE,
	W5100_ERROR
} W5100_STATE;


W5100_STATE W51_GetEthDeviceState	(void);
//void	W51_Init		(void);
void	W51_Run			(void);
#if (TEST == TEST_W5100)
	void W51_Test		(void);
#endif

void	W51_GetSocketStatus			(char* txt);

#endif  // _w5100_h_
