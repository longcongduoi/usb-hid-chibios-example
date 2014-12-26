/*

  Copyright (c) 2014 Guillaume Duc <guillaume@guiduc.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include "ch.h"
#include "hal.h"

#include "usb_hid.h"

static struct usb_hid_in_report_s usb_hid_in_report;
static struct usb_hid_out_report_s usb_hid_out_report;

/*
 * Button thread
 *
 * This thread regularely checks the value of the wkup
 * pushbutton. When its state changes, the thread sends a IN report.
 */
static WORKING_AREA (buttonThreadWorkingArea, 128);

static uint8_t wkup_old_state, wkup_cur_state;

static msg_t
buttonThread (void __attribute__ ((__unused__)) * arg)
{
  wkup_old_state = 0;

  while (1)
    {
      wkup_cur_state = palReadPad (GPIOA, GPIOA_BUTTON_WKUP);

      if (wkup_cur_state != wkup_old_state)
	{
	  chSysLock ();

	  if (usbGetDriverStateI (&USBD2) == USB_ACTIVE)
	    {
	      chSysUnlock ();

	      usb_hid_in_report.wkup_pb_value = wkup_cur_state;

	      // Send the IN report
	      usb_send_hid_report (&usb_hid_in_report);
	    }
	  else
	    chSysUnlock ();

	  wkup_old_state = wkup_cur_state;
	}

      chThdSleepMilliseconds (50);
    }

  return 0;
}


/*
 * Main thread
 */
int
main (void)
{
  // ChibiOS/RT init
  halInit ();
  chSysInit ();

  palClearPad (GPIOC, GPIOC_LED);

  // Init USB
  init_usb_serial_string ();
  init_usb_queues ();
  init_usb_driver ();

  // Wait until the USB is active
  while (USBD2.state != USB_ACTIVE)
    chThdSleepMilliseconds (1000);

  palSetPad (GPIOC, GPIOC_LED);

  chThdSleepMilliseconds (500);

  // Start the button thread
  chThdCreateStatic (buttonThreadWorkingArea,
		     sizeof (buttonThreadWorkingArea),
		     NORMALPRIO, buttonThread, NULL);

  /*
   * Main loop: waits until a OUT report is received and updates the
   * LED
   */
  while (1)
    {
      if (chIQReadTimeout
	  (&usb_input_queue, (uint8_t *) & usb_hid_out_report,
	   USB_HID_OUT_REPORT_SIZE, TIME_INFINITE) == USB_HID_OUT_REPORT_SIZE)
	{
	  if (usb_hid_out_report.led_value == 0)
	    palSetPad (GPIOC, GPIOC_LED);
	  else
	    palClearPad (GPIOC, GPIOC_LED);
	}
    }
}
