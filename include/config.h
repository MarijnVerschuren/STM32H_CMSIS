//
// Created by marijn on 3/4/24.
//

#ifndef CONFIG_H
#define CONFIG_H


/*!<
 * USB selection
 *
 * flash:		EXTREMELY HIGH
 * ram:			EXTREMELY HIGH
 * power:		EXTREMELY HIGH
 * */
#define	USB_LIB					// enable / compile USB library
#ifdef	USB_LIB
//#define	USB_CLASS_COMPOSITE_LIB		// TODO
#define	USB_CLASS_HID_KEYBOARD_LIB	// TODO
//#define	USB_CLASS_HID_MOUSE_LIB		// TODO
//#define	USB_CLASS_HID_CUSTOM_LIB	// TODO
//#define	USB_CLASS_MSC_LIB			// TODO: Mass Storage Class

#define USB_CLASS_HID_LIB defined(USB_CLASS_COMPOSITE_LIB) || defined(USB_CLASS_HID_KEYBOARD_LIB) || defined(USB_CLASS_HID_MOUSE_LIB) || defined(USB_CLASS_HID_CUSTOM_LIB)
#endif // USB_LIB


#endif // CONFIG_H
