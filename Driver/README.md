USB Driver Installation
=======================

This example project uses the Python library PyUsb which requires the installation of either
libusb 0.1, libusb 1.0 or OpenUSB. Windows users (like myself) must install the libusb-win32
driver, which is a Windows port of the driver. More information about libusb-win32, including
installation can be found here:

http://sourceforge.net/apps/trac/libusb-win32/wiki 

These instructions are for Windows. If you are not using Windows, you will have
to look up the instructions for your particular case on the libusb online documentation.

### How to install libusb-win32

1. Download the "libusb-win32-bin-1.2.6.0.zip" file or the latest
   version from:
   http://sourceforge.net/projects/libusb-win32/files/
2. Extract the files to a folder on your hard drive.
3. Power up the ZC706.
4. Program the bitstream for the USB example project.
5. Run the SDK project.
6. Connect the USB connector J2 to your computer using a USB cable.
7. In Windows 7, run the "bin/inf-wizard.exe" file that you just
   extracted to your hard drive.
8. You will first see an information window, click Next.
9. In the Device Selection window, you should see a device with
   the following parameters:
   Vendor ID: 0x0D7D
   Product ID: 0x0100
   Description: ESB USB Flash Drive Disk Emulation
   The description may instead be "USB Mass Storage Device", but the
   important parameters to recognize are the Vendor and Product IDs. 
10. Select THAT device and make sure NOT to select the wrong device!
    Selecting the wrong device will change your existing driver for
    something else, which can be VERY difficult to fix for say a
    keyboard or mouse. Click Next.
11. You should see the Device Configuration window, make sure the
    device is the right one and click Next. The Manufacturer name
    should be "Phison Electronics Corp.".
12. It will now open a Save As window and you should select a folder
    to place the .inf file for your device. Select an appropriate
    location on your hard disk (I put it in a folder called
    Driver in the same place I keep my USB project files).
13. The wizard will then create a windows driver installation
    package. Click the button Install Now.
14. You should get a warning message from Windows saying that the
    publisher of the driver software cannot be verified. Click
    "Install this driver software anyway".
15. The driver should be installed, now click Done.
16. Restart the computer. This does not seem to be necessary but I have
    had the BSOD once and it did not happen again after reboot.
17. In Windows 7, go to the Device Manager.
18. You should see a device called "EPB USB Flash Drive Disk Emulation"
    or "USB Mass Storage Device" under the group "libusb-win32-devices".
    If you see this, you have correctly installed the driver.

### How to remove the driver

If you want to remove the driver:
1. Power up the ZC706.
2. Program the bitstream for the USB example project.
3. Run the SDK project.
4. Connect the USB connector J2 to your computer using a USB cable.
5. In Windows 7, go to the Device Manager.
6. You should see a device called "EPB USB Flash Drive Disk Emulation"
   under the group "libusb-win32-devices".
7. Right click on the device and select "Uninstall".
8. You will get a warning message, tick the box labelled "Delete the
   driver software for this device" and click OK.
   If you do not delete the driver, it will be installed automatically
   the next time you plug in the device.
9. The device will disappear from the list.
10. Check that the driver has been uninstalled correctly by disconnecting
    the USB cable and re-connecting it.


### License

Feel free to modify the project or use it as a base for your application.

### Fork and share

If you port this project to another hardware platform, please send me the
code or push it onto GitHub and send me the link so I can post it on my
website. The more people that benefit, the better.

### About the author

I'm an FPGA consultant and I provide FPGA design services and training to
innovative companies around the world. I believe in sharing knowledge and
I regularly contribute to the open source community.

Jeff Johnson
http://www.fpgadeveloper.com