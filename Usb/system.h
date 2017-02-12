#ifndef SYSTEM_H
#define SYSTEM_H

#include	"Typedef.h"
#include "usb.h"
#include "usb_device_hid.h"
#include "usb_config.h"

/*** System States **************************************************/
typedef enum
{
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

void SYSTEM_Initialize( SYSTEM_STATE state );

/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
//void SYSTEM_Tasks(void);
#define SYSTEM_Tasks()

void Usb_Init				(void);
void Usb_RegularFunctions	(void);

#endif //SYSTEM_H
