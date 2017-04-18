#ifndef		_gsm_h_
#define		_gsm_h_

#include	"Typedef.h"

//#define	GSM_IF			_SI2C1IF
//#define	GSM_IE			_SI2C1IE

void Gsm_On			(void);
void Gsm_Off		(void);
void Gsm_PwrOn		(void);
void Gsm_PwrKey		(void);
void Gsm_AT			(void);
void Gsm_AtCommand	(char* str);
void Gsm_DTR		(void);
U16	 Gsm_AUTO		(void);
void Gsm_OnLine		(void);
S8	 SendTcpPacket	(char* ptr, U16 len);
U8	 GprsConnected	(void);

void Gsm_Setup		(void);
void TcpPacket		(U8 type);

U8	 Gsm_GetCsq		(void);
U16	 Gsm_GetFlags	(void);
U16	 Gsm_GetState	(void);

U8	 GsmTest		(void);

#endif	//	_gsm_h_

