#include	"Timers.h"
#include	"system.h"
#include	"usb_device.h"
#include	"app_device_custom_hid.h"

#ifdef LED_TIM
	#define	LED_USB_ON	LED_STATUS_1_I
	#define	LED_USB_OFF	LED_STATUS_1
#else
	#define	LED_USB_ON	LED_STATUS_ON
	#define	LED_USB_OFF	LED_STATUS_OFF
#endif

void SYSTEM_Initialize( SYSTEM_STATE state )
{
}

void Usb_Init (void)
{
	USBDeviceInit();
	USBDeviceAttach();
}

void Usb_RegularFunctions (void)
{
	#if defined(USB_POLLING)
		USBDeviceTasks();
	#endif
	if(( USBGetDeviceState()	>= CONFIGURED_STATE )	&&
	   ( USBIsDeviceSuspended()	== false )					)
	    APP_DeviceCustomHIDTasks();
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif

void APP_LEDUpdateUSBStatus(void)
{
//	if(USBIsDeviceSuspended() == true)
//	{
//		LED_USB = LED_STATUS_1_I;
//		return;
//	}
	switch(USBGetDeviceState())
	{
	case DETACHED_STATE:		LED_USB = LED_USB_OFF;			break;
	case CONFIGURED_STATE:		LED_USB = LED_USB_ON;			break;
	default:					LED_USB = LED_STATUS_ERROR;		break;
	}
}

bool USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, uint16_t size)
{
	switch( event )
	{
		case EVENT_TRANSFER:
			break;

		case EVENT_SOF:            // We are using the SOF as a timer to time the LED indicator.  Call the LED update function here.
			APP_LEDUpdateUSBStatus();
			break;

		case EVENT_SUSPEND:			// Update the LED status for the suspend event.
			LED_USB = LED_USB_OFF;	//LED_STATUS_1;
			break;

		case EVENT_RESUME:			// Update the LED status for the resume event.
			APP_LEDUpdateUSBStatus();
			break;

		case EVENT_CONFIGURED:		// When the device is configured, we can (re)initialize the demo code.
			APP_DeviceCustomHIDInitialize();
			break;

		case EVENT_SET_DESCRIPTOR:
			break;

		case EVENT_EP0_REQUEST:		// We have received a non-standard USB request.  The HID driver needs to check to see if the request was for it.
			USBCheckHIDRequest();
			break;

		case EVENT_BUS_ERROR:
			break;

		case EVENT_TRANSFER_TERMINATED:
			break;

		default:
			break;
	}
	return true;
}

