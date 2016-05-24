
// License: MIT. See LICENSE file in root directory.
// Copyright(c) 2016 JetsonHacks. All Rights Reserved.

// Workarounds for Intel Realsense R200 on NVIDIA Jetson TK1

#include <unistd.h>

#define VENDOR_ID   0x8086
#define PRODUCT_ID  0x0a80

//
// Reset the R200 device
// When the Jetson TK1 boots with the camera plugged in and is accessed for the first time
// the camera will timeout. Applying libusb_reset_device works around this issue. This should
// only be called one time after booting, and the clear interface halts must *NOT* be called
//
// Another workaround is to do an unplug/replug of the camera into the USB port

void resetDevice ( void ) {
    libusb_context          *context = NULL ;
    libusb_device_handle    *dev_handle = NULL ;
    libusb_device           **devs ;
    int                     result ;
    ssize_t                 count ; // Number of devices in list
    int ret ;

    result = libusb_init(&context);
    if (result != 0) {
	fprintf(stderr,"Unable to initialize libusb in resetDevice!\n");
	return ;
    }
    count = libusb_get_device_list(context, &devs);
    dev_handle = libusb_open_device_with_vid_pid(context,VENDOR_ID,PRODUCT_ID);
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
    if (dev_handle) {

        int bpoint = 0 ;
        int success = 0 ;
        do {
            success = libusb_reset_device(dev_handle) ;
            ret = success ;
            ++ bpoint ;
            if (bpoint > 100) {
                success = 1 ;
            }

        } while (success < 0 ) ;   
        if (success) { 
	    // success was set to 1 after multiple tries to reset the device failed
            fprintf(stderr,"Unable to reset camera! %d %s\n", ret,libusb_error_name(ret));
        } 
        libusb_close(dev_handle) ;
    }
    libusb_exit(context);
}

void clearInterfaceHalt ( libusb_device_handle *dev_handle, int bInterfaceNumber, int bEndpointAddress) {
    int result ;
    if (libusb_kernel_driver_active(dev_handle,bInterfaceNumber)) {
        libusb_detach_kernel_driver(dev_handle,bInterfaceNumber) ;
    }

    result = libusb_claim_interface(dev_handle,bInterfaceNumber) ;
    if (result != 0) {
        fprintf(stderr,"Could not claim interface bInterfaceNumber %d! %d %s\n",bInterfaceNumber,result,libusb_error_name(result));
    }
    result = libusb_clear_halt(dev_handle,bEndpointAddress) ;
    if (result != 0) {
        fprintf(stderr,"Could not clear halt on interface bInterfaceNumber: %d! %d %s\n",bInterfaceNumber,result,libusb_error_name(result));
    }
    result = libusb_release_interface(dev_handle,bInterfaceNumber) ;
    if (result != 0) {
       fprintf(stderr,"Could not release interface bInterfaceNumber: %d! %d %s\n",bInterfaceNumber,result,libusb_error_name(result));
    }
    libusb_attach_kernel_driver(dev_handle,bInterfaceNumber) ;
}

//
// One of the issues encountered after running a program using the R200 camera
// is that the video streams are halted, and return libusb -EPIPE when accessed subsequently.
// Here we issue libusb_clear_halt to the video streams to kick start them. libusb_reset_device
// must *NOT* be called prior to this, otherwise the streams will timeout when started
//
void videoStreamClearHalts ( void ) {
    libusb_context          *context = NULL ;
    libusb_device_handle    *dev_handle = NULL ;
    libusb_device           **devs ;
    int                     result ;
    ssize_t                 count ; // Number of devices in list
    int ret ;

    result = libusb_init(&context);
    if (result != 0) {
	fprintf(stderr,"Unable to initialize libusb in clearHalts!\n");
	return ;
    }
    count = libusb_get_device_list(context, &devs);
    dev_handle = libusb_open_device_with_vid_pid(context,VENDOR_ID,PRODUCT_ID);
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
    if (dev_handle) {
        // Clear interface halts that might be on the video streams
        // Infrared cameras
        clearInterfaceHalt(dev_handle,1,0x86) ;
        // Depth camera stream
        clearInterfaceHalt(dev_handle,3,0x85) ;
        // Interface Number 5 is the color camera
        clearInterfaceHalt(dev_handle,5,0x87) ;
        libusb_close(dev_handle) ;
     }
    libusb_exit(context);
}

// 
// There are a couple of issues running the Intel Realsense R200 camera with a NVIDIA Jetson TK1 Dev Kit
// First, after booting the machine with the camera plugged in, the camera will timeout when data is requested.
// As a workaround, create a file named /tmp/reset-realsense when the TK1 boots. This can be done:
// $ touch /tmp/reset-realsense
// Call the following routine, which will attempt to reset the camera. For best results, the camera
// should be directly connected to the Jetson USB 3.0 A connector, as different hubs can cause issues.
//
// The second issue is that after running a program that uses the camera, the second time running the program
// does not work, typically with error messages having to do with the LIBUSB PIPE having issues. This is probably
// caused by the camera firmware/libusb/kernel not shutting down the first connection properly.
// In order to clear the LIBUSB pipe issue,  videoStreamClearHalts may be called.
//
// Note that neither of these workarounds can be considered robust (or work 100% of the time), but they may prove helpful
// in situations where replugging the camera after each use is not practical.
//

void r200JTK1Setup ( void ) {
    if( access( "/tmp/reset-realsense", F_OK ) != -1 ) {
        // file exists
        unlink("/tmp/reset-realsense") ;
        resetDevice();
    } else {
        // file doesn't exist ; means we've already run the program once since reboot
        // Clear interface halts that may have accumulated on video streams
        videoStreamClearHalts();
    }
}


