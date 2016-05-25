// License: MIT. See LICENSE file in root directory.
// Copyright(c) 2016 JetsonHacks. All Rights Reserved.

// Workarounds for Intel Realsense R200 on NVIDIA Jetson TK1
// Call r200JTK1Setup on program startup


#ifndef SETUPR200_H
#define SETUPR200_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libusb.h>

void resetDevice ( void ) ;
void clearInterfaceHalt ( libusb_device_handle *dev_handle, int bInterfaceNumber, int bEndpointAddress) ;
void videoStreamClearHalts ( void ) ;

// 
// There are a couple of issues running the Intel Realsense R200 camera with a NVIDIA Jetson TK1 Dev Kit
// First, after booting the machine with the camera plugged in, the camera will timeout when data is requested.
// As a workaround, create a file named /tmp/reset-realsense when the TK1 boots. One example of creating the file:
// $ touch /tmp/reset-realsense
// Call r200JTK1Setup(), which will attempt to reset the camera. For best results, the camera
// should be directly connected to the Jetson USB 3.0 A connector, as different USB 3.0 hubs (even powered ones) may cause issues.
//
// The second issue is that after running a program that uses the camera, the second time running the program
// does not work, typically with error messages having to do with the LIBUSB PIPE having issues. This is probably
// caused by the camera firmware/libusb/kernel not shutting down the first connection properly.
// In order to clear the LIBUSB pipe issue,  videoStreamClearHalts are be called.
//
// Note that neither of these workarounds can be considered robust (or work 100% of the time), but they may prove helpful
// in situations where replugging the camera after each use is not practical.
//
void r200JTK1Setup ( void ) ;

#ifdef __cplusplus
}
#endif

#endif // !def(SETUPR200_H)
