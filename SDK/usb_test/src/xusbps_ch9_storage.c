/******************************************************************************
*
* (c) Copyright 2010-12 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
 * @file xusbps_ch9_storage.c
 *
 * This file contains the implementation of the storage specific chapter 9 code
 * for the example.
 *
 *<pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- ---------------------------------------------------------
 * 1.00a wgr  10/10/10 First release
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/

#include <string.h>

#include "xparameters.h"	/* XPAR parameters */
#include "xusbps.h"		/* USB controller driver */

#include "xusbps_ch9.h"
#include "xusbps_ch9_storage.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	u16 idVendor;
	u16 idProduct;
	u16 bcdDevice;
	u8  iManufacturer;
	u8  iProduct;
	u8  iSerialNumber;
	u8  bNumConfigurations;
}__attribute__((__packed__))USB_STD_DEV_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 wTotalLength;
	u8  bNumInterfaces;
	u8  bConfigurationValue;
	u8  iConfiguration;
	u8  bmAttributes;
	u8  bMaxPower;
}__attribute__((__packed__))USB_STD_CFG_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bInterfaceNumber;
	u8  bAlternateSetting;
	u8  bNumEndPoints;
	u8  bInterfaceClass;
	u8  bInterfaceSubClass;
	u8  bInterfaceProtocol;
	u8  iInterface;
}__attribute__((__packed__))USB_STD_IF_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bEndpointAddress;
	u8  bmAttributes;
	u16 wMaxPacketSize;
	u8  bInterval;
}__attribute__((__packed__))USB_STD_EP_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 wLANGID[1];
}__attribute__((__packed__))USB_STD_STRING_DESC;

typedef struct {
	USB_STD_CFG_DESC stdCfg;
	USB_STD_IF_DESC ifCfg;
	USB_STD_EP_DESC epCfg1;
	USB_STD_EP_DESC epCfg2;
}__attribute__((__packed__))USB_CONFIG;

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#define USB_ENDPOINT0_MAXP		0x40

#define USB_BULKIN_EP			1
#define USB_BULKOUT_EP			1

#define USB_DEVICE_DESC			0x01
#define USB_CONFIG_DESC			0x02
#define USB_STRING_DESC			0x03
#define USB_INTERFACE_CFG_DESC		0x04
#define USB_ENDPOINT_CFG_DESC		0x05


/*****************************************************************************/
/**
*
* This function returns the device descriptor for the device.
*
* @param	BufPtr is pointer to the buffer that is to be filled
*		with the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupDevDescReply(u8 *BufPtr, u32 BufLen)
{
	USB_STD_DEV_DESC deviceDesc = {
		sizeof(USB_STD_DEV_DESC),	/* bLength */
		USB_DEVICE_DESC,		/* bDescriptorType */
		be2les(0x0200),			/* bcdUSB 2.0 */
		0x00,				/* bDeviceClass */
		0x00,				/* bDeviceSubClass */
		0x00,				/* bDeviceProtocol */
		USB_ENDPOINT0_MAXP,		/* bMaxPackedSize0 */
		be2les(0x0d7d),			/* idVendor */
		be2les(0x0100),			/* idProduct */
		be2les(0x0100),			/* bcdDevice */
		0x01,				/* iManufacturer */
		0x02,				/* iProduct */
		0x03,				/* iSerialNumber */
		0x01				/* bNumConfigurations */
	};

	/* Check buffer pointer is there and buffer is big enough. */
	if (!BufPtr) {
		return 0;
	}

	if (BufLen < sizeof(USB_STD_DEV_DESC)) {
		return 0;
	}

	memcpy(BufPtr, &deviceDesc, sizeof(USB_STD_DEV_DESC));

	return sizeof(USB_STD_DEV_DESC);
}


/*****************************************************************************/
/**
*
* This function returns the configuration descriptor for the device.
*
* @param	BufPtr is the pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupCfgDescReply(u8 *BufPtr, u32 BufLen)
{
	USB_CONFIG config = {
		/* Std Config */
		{sizeof(USB_STD_CFG_DESC),	/* bLength */
		 USB_CONFIG_DESC,		/* bDescriptorType */
		 be2les(sizeof(USB_CONFIG)),	/* wTotalLength */
		 0x01,				/* bNumInterfaces */
		 0x01,				/* bConfigurationValue */
		 0x04,				/* iConfiguration */
		 0xc0,				/* bmAttribute */
		 0x00},				/* bMaxPower  */

		/* Interface Config */
		{sizeof(USB_STD_IF_DESC),	/* bLength */
		 USB_INTERFACE_CFG_DESC,	/* bDescriptorType */
		 0x00,				/* bInterfaceNumber */
		 0x00,				/* bAlternateSetting */
		 0x02,				/* bNumEndPoints */
		 0x08,				/* bInterfaceClass */
		 0x06,				/* bInterfaceSubClass */
		 0x50,				/* bInterfaceProtocol */
		 0x05},				/* iInterface */

		/* Bulk Out Endpoint Config */
		{sizeof(USB_STD_EP_DESC),	/* bLength */
		 USB_ENDPOINT_CFG_DESC,		/* bDescriptorType */
		 0x00 | USB_BULKOUT_EP,		/* bEndpointAddress */
		 0x02,				/* bmAttribute  */
		 be2les(0x200),			/* wMaxPacketSize */
		 0x00},				/* bInterval */

		/* Bulk In Endpoint Config */
		{sizeof(USB_STD_EP_DESC),	/* bLength */
		 USB_ENDPOINT_CFG_DESC,		/* bDescriptorType */
		 0x80 | USB_BULKIN_EP,		/* bEndpointAddress */
		 0x02,				/* bmAttribute  */
		 be2les(0x200),			/* wMaxPacketSize */
		 0x00}				/* bInterval */
	};

	/* Check buffer pointer is OK and buffer is big enough. */
	if (!BufPtr) {
		return 0;
	}

	if (BufLen < sizeof(USB_STD_DEV_DESC)) {
		return 0;
	}

	memcpy(BufPtr, &config, sizeof(USB_CONFIG));

	return sizeof(USB_CONFIG);
}


/*****************************************************************************/
/**
*
* This function returns a string descriptor for the given index.
*
* @param	BufPtr is a  pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
* @param	Index is the index of the string for which the descriptor
*		is requested.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupStrDescReply(u8 *BufPtr, u32 BufLen, u8 Index)
{
	int i;

	static char *StringList[] = {
		"UNUSED",
		"Xilinx",
		"EPB USB Flash Drive Disk Emulation",
		"2A49876D9CC1AA4",
		"Default Configuration",
		"Default Interface",
	};
	char *String;
	u32 StringLen;
	u32 DescLen;
	u8 TmpBuf[128];

	USB_STD_STRING_DESC *StringDesc;

	if (!BufPtr) {
		return 0;
	}

	if (Index >= sizeof(StringList) / sizeof(char *)) {
		return 0;
	}

	String = StringList[Index];
	StringLen = strlen(String);

	StringDesc = (USB_STD_STRING_DESC *) TmpBuf;

	/* Index 0 is special as we can not represent the string required in
	 * the table above. Therefore we handle index 0 as a special case.
	 */
	if (0 == Index) {
		StringDesc->bLength = 4;
		StringDesc->bDescriptorType = USB_STRING_DESC;
		StringDesc->wLANGID[0] = be2les(0x0409);
	}
	/* All other strings can be pulled from the table above. */
	else {
		StringDesc->bLength = StringLen * 2 + 2;
		StringDesc->bDescriptorType = USB_STRING_DESC;

		for (i = 0; i < StringLen; i++) {
			StringDesc->wLANGID[i] = be2les((u16) String[i]);
		}
	}
	DescLen = StringDesc->bLength;

	/* Check if the provided buffer is big enough to hold the descriptor. */
	if (DescLen > BufLen) {
		return 0;
	}

	memcpy(BufPtr, StringDesc, DescLen);

	return DescLen;
}


/*****************************************************************************/
/**
* This function handles a "set configuration" request.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	ConfigIdx is the Index of the desired configuration.
*
* @return	None
*
******************************************************************************/
void XUsbPs_SetConfiguration(XUsbPs *InstancePtr, int ConfigIdx)
{
	Xil_AssertVoid(InstancePtr != NULL);

	/* We only have one configuration. Its index is 1. Ignore anything
	 * else.
	 */
	if (1 != ConfigIdx) {
		return;
	}

	XUsbPs_EpEnable(InstancePtr, 1, XUSBPS_EP_DIRECTION_OUT);
	XUsbPs_EpEnable(InstancePtr, 1, XUSBPS_EP_DIRECTION_IN);

	/* Set BULK mode for both directions.  */
	XUsbPs_SetBits(InstancePtr, XUSBPS_EPCR1_OFFSET,
						XUSBPS_EPCR_TXT_BULK_MASK |
						XUSBPS_EPCR_RXT_BULK_MASK |
						XUSBPS_EPCR_TXR_MASK |
						XUSBPS_EPCR_RXR_MASK);

	/* Prime the OUT endpoint. */
	XUsbPs_EpPrime(InstancePtr, 1, XUSBPS_EP_DIRECTION_OUT);
}
