# setupR200-Jetson
Workarounds for adding the Intel Realsense R200 camera to the NVIDIA Jetson Development Kit.

These workarounds are not 100 percent dependable, but can be useful if physical access to the USB plug is limited.

There are a couple of issues running the Intel Realsense R200 camera with a NVIDIA Jetson Dev Kit.
First, after booting the machine with the camera plugged in, the camera may timeout when data is requested if the camera is plugged into a USB hub. In some cases, this may be overcome by doing a soft device reset on the camera. 
As a workaround, create a file named /tmp/reset-realsense when the Jetson boots. One example for creating the file:

$ touch /tmp/reset-realsense

Call r200JetsonSetup(), which will then attempt to reset the camera. For best results, the camera
should be directly connected to the Jetson USB 3.0 A connector, as different USB 3.0 hubs (even powered ones) may cause issues.

The second issue is that after running a program that uses the camera, running the program a second time
does not work, typically with error messages having to do with the LIBUSB PIPE. This is probably
caused by the camera firmware/libusb/kernel not shutting down correctly on the first run, but the issue has not
been discovered yet.
In order to clear the LIBUSB pipe issue, videoStreamsClearHalts is called. 

NOTE: Calling videoStreamClearHalts() after calling resetDevice() will cause the camera device to timeout instead of 
providing a video stream. resetDevice() should only be called the first time the camera is used after booting. Physically 
unplugging/replugging the device obviates the need for resetDevice().

In the librealsense library, the files may be placed in the 'src' folder.

Note that neither of these workarounds can be considered robust (or work 100% of the time), but they may prove helpful
in situations where replugging the camera after each use is not practical.

