# usb-hid-chibios-example

## Introduction

This is a small example application that uses the HID (Human Interface
Device) class to communicate over USB.

This project is based on [ChibiOS](http://www.chibios.org) real-time
operating system and is designed to run on the
[Olimex](https://www.olimex.com/)
[STM32-E407 board](https://www.olimex.com/Products/ARM/ST/STM32-E407/open-source-hardware). It
should work with only minor modifications on other STM32 boards
supported by ChibiOS.

On the client side, a small client uses the HIDRAW interface of the
Linux kernel to communicate with the board.

In this example, two HID reports are defined:

* IN report (board->PC): 2 bytes
  * First byte: a sequence number
  * Second byte: the state of the WKUP pushbutton
* OUT report (PC->board): 2 bytes
  * First byte: a sequence number
  * Second byte: the state of the green LED

Two interrupt USB endpoints (one IN and one OUT) are used.

## How to run

### Prerequisites

You first need to download the latest version of the stable 2.6.x
branch of ChibiOS.

### Board

The code for the board is in the directory `stm32-e407`.

To compile it you first need to modify the `Makefile`. Modify the
following line:

```
CHIBIOS = /home/tux/src/ChibiOS-RT
```

to point to the directory containing the sources of ChibiOS.

Then, just type `make` and the result image to program to the
STM32-E407 board is `build/ch.elf`.

### Client (Linux)

The code of the Linux client is in the directory `linux-client`.

Just type `make`to compile the client.

### Run

1. Download the image to the STM32-E407 board
2. Plug a USB cable into the OTG2 mini-USB socket of the board
3. Find the corresponding `hidraw` device that was created (example `/dev/hidraw3`)
4. Launch the client: `test-usb-hid /dev/hidraw3`
5. Each time the WKUP button of the board is pushed, the green LED should toggle

## References

* [Device Class Definition for Human Interface Devices (HID)](http://www.usb.org/developers/hidpage/HID1_11.pdf)
* [The Linux kernel HIDRAW interface](https://www.kernel.org/doc/Documentation/hid/hidraw.txt)
* [ChibiOS/RT documentation](http://chibios.sourceforge.net/html/index.html)
