# setupR200-TK1
Workarounds for adding the Intel Realsense R200 camera to the NVIDIA Jetson TK1 Development Kit.


There are a couple of issues running the Intel Realsense R200 camera with a NVIDIA Jetson TK1 Dev Kit
First, after booting the machine with the camera plugged in, the camera will timeout when data is requested.
As a workaround, create a file named /tmp/reset-realsense when the TK1 boots. One example for creating the file:

$ touch /tmp/reset-realsense

Call r200JTK1Setup(), which will then attempt to reset the camera in this case. For best results, the camera
should be directly connected to the Jetson USB 3.0 A connector, as different USB 3.0 hubs (even powered ones) may cause issues.

The second issue is that after running a program that uses the camera, running the program a second time
does not work, typically with error messages having to do with the LIBUSB PIPE. This is probably
caused by the camera firmware/libusb/kernel not shutting down correctly on the first run, but the issue has not
been found yet.
In order to clear the LIBUSB pipe issue, videoStreamClearHalts is called. 

NOTE: Calling videoStreamClearHalts() after calling resetDevice() will cause the device to timeout instead of 
providing a video stream. resetDevice() should only be called the first time the camera is used after booting. Physically 
unplugging/replugging the device obviates the need for resetDevice().

In the librealsense library, the files may be placed in the 'src' folder.

Note that neither of these workarounds can be considered robust (or work 100% of the time), but they may prove helpful
in situations where replugging the camera after each use is not practical.

