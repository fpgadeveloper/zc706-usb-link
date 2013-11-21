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
 * @file xusbps_ch9.c
 *
 * This file contains the implementation of the chapter 9 code for the example.
 *
 *<pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- ---------------------------------------------------------
 * 1.00a jz  10/10/10 First release
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/


#include "xparameters.h"	/* XPAR parameters */
#include "xusbps.h"		/* USB controller driver */
#include "xusbps_hw.h"		/* USB controller driver */

#include "xusbps_ch9.h"
#include "xil_printf.h"

/*default class is storage class */
#include "xusbps_class_storage.h"

#define CH9_DEBUG

#ifdef CH9_DEBUG
#include <stdio.h>
#define printf xil_printf
#endif

/************************** Constant Definitions *****************************/
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

static void XUsbPs_StdDevReq(XUsbPs *InstancePtr,
			      XUsbPs_SetupData *SetupData);

/************************** Variable Definitions *****************************/

static u8 Response = (u8)0x0;

/*****************************************************************************/
/**
* This function handles a Setup Data packet from the host.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	SetupData is the structure containing the setup request.
*
* @return
*		- XST_SUCCESS if the function is successful.
*		- XST_FAILURE if an Error occured.
*
* @note		None.
*
******************************************************************************/
int XUsbPs_Ch9HandleSetupPacket(XUsbPs *InstancePtr,
				 XUsbPs_SetupData *SetupData)
{

#ifdef CH9_DEBUG
	printf("Handle setup packet\n\r");
#endif

	switch (SetupData->bmRequestType & XUSBPS_REQ_TYPE_MASK) {
	case XUSBPS_CMD_STDREQ:
		XUsbPs_StdDevReq(InstancePtr, SetupData);
		break;

	case XUSBPS_CMD_CLASSREQ:
		XUsbPs_ClassReq(InstancePtr, SetupData);
		break;

	case XUSBPS_CMD_VENDREQ:

#ifdef CH9_DEBUG
		printf("vendor request %x\n\r", SetupData->bRequest);
#endif
		/* No support */
		/* Status = XUsbPs_HandleVendorReq(InstancePtr, SetupData); */
		break;

	default:
		/* Stall on Endpoint 0 */
#ifdef CH9_DEBUG
		printf("unknown class req, stall 0 in out\n\r");
#endif
		XUsbPs_EpStall(InstancePtr, 0, XUSBPS_EP_DIRECTION_IN |
						XUSBPS_EP_DIRECTION_OUT);
		break;
	}

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
* This function handles a standard device request.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	SetupData is a pointer to the data structure containing the
*		setup request.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void XUsbPs_StdDevReq(XUsbPs *InstancePtr,
			      XUsbPs_SetupData *SetupData)
{
	int Status;
	int Error = 0;

	XUsbPs_Local	*UsbLocalPtr;

	int ReplyLen;
	static u8  	Reply[XUSBPS_REQ_REPLY_LEN];

	/* Check that the requested reply length is not bigger than our reply
	 * buffer. This should never happen...
	 */
	if (SetupData->wLength > XUSBPS_REQ_REPLY_LEN) {
		return;
	}

	UsbLocalPtr = (XUsbPs_Local *) InstancePtr->UserDataPtr;

#ifdef CH9_DEBUG
	printf("std dev req %d\n\r", SetupData->bRequest);
#endif

	switch (SetupData->bRequest) {

	case XUSBPS_REQ_GET_STATUS:

		switch(SetupData->bmRequestType & XUSBPS_STATUS_MASK) {
		case XUSBPS_STATUS_DEVICE:
			/* It seems we do not have to worry about zeroing out the rest
			 * of the reply buffer even though we are only using the first
			 * two bytes.
			 */
			*((u16 *) &Reply[0]) = 0x0100; /* Self powered */
			break;

		case XUSBPS_STATUS_INTERFACE:
			*((u16 *) &Reply[0]) = 0x0;
			break;

		case XUSBPS_STATUS_ENDPOINT:
			{
			u32 Status;
			int EpNum = SetupData->wIndex;

			Status = XUsbPs_ReadReg(InstancePtr->Config.BaseAddress,
					XUSBPS_EPCRn_OFFSET(EpNum & 0xF));

			if(EpNum & 0x80) { /* In EP */
				if(Status & XUSBPS_EPCR_TXS_MASK) {
					*((u16 *) &Reply[0]) = 0x0100;
				}else {
					*((u16 *) &Reply[0]) = 0x0000;
				}
			} else {	/* Out EP */
				if(Status & XUSBPS_EPCR_RXS_MASK) {
					*((u16 *) &Reply[0]) = 0x0100;
				}else {
					*((u16 *) &Reply[0]) = 0x0000;
				}
			}
			break;
			}

		default:
			;
#ifdef CH9_DEBUG
			printf("unknown request for status %x\n\r", SetupData->bmRequestType);
#endif
		}
		XUsbPs_EpBufferSend(InstancePtr, 0, Reply, SetupData->wLength);
		break;

	case XUSBPS_REQ_SET_ADDRESS:

		/* With bit 24 set the address value is held in a shadow
		 * register until the status phase is acked. At which point it
		 * address value is written into the address register.
		 */
		XUsbPs_SetDeviceAddress(InstancePtr, SetupData->wValue);
#ifdef CH9_DEBUG
		printf("Set address %d\n\r", SetupData->wValue);
#endif
		/* There is no data phase so ack the transaction by sending a
		 * zero length packet.
		 */
		XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);
		break;

	case XUSBPS_REQ_GET_INTERFACE:
#ifdef CH9_DEBUG
		printf("Get interface %d/%d/%d\n\r",
			SetupData->wIndex, SetupData->wLength,
			InstancePtr->CurrentAltSetting);
#endif
		Response = (u8)InstancePtr->CurrentAltSetting;

		/* Ack the host */
		XUsbPs_EpBufferSend(InstancePtr, 0, &Response, 1);

		break;

	case XUSBPS_REQ_GET_DESCRIPTOR:
#ifdef CH9_DEBUG
		printf("Get desc %x/%d\n\r", (SetupData->wValue >> 8) & 0xff,
				SetupData->wLength);
#endif

		/* Get descriptor type. */
		switch ((SetupData->wValue >> 8) & 0xff) {

		case XUSBPS_TYPE_DEVICE_DESC:
		case XUSBPS_TYPE_DEVICE_QUALIFIER:

			/* Set up the reply buffer with the device descriptor
			 * data.
			 */
			ReplyLen = XUsbPs_Ch9SetupDevDescReply(
						Reply, XUSBPS_REQ_REPLY_LEN);

			ReplyLen = ReplyLen > SetupData->wLength ?
						SetupData->wLength : ReplyLen;

			if(((SetupData->wValue >> 8) & 0xff) ==
					XUSBPS_TYPE_DEVICE_QUALIFIER) {
				Reply[0] = (u8)ReplyLen;
				Reply[1] = (u8)0x6;
				Reply[2] = (u8)0x0;
				Reply[3] = (u8)0x2;
				Reply[4] = (u8)0xFF;
				Reply[5] = (u8)0x00;
				Reply[6] = (u8)0x0;
				Reply[7] = (u8)0x10;
				Reply[8] = (u8)0;
				Reply[9] = (u8)0x0;
			}
			Status = XUsbPs_EpBufferSend(InstancePtr, 0,
							Reply, ReplyLen);
			if (XST_SUCCESS != Status) {
				/* Failure case needs to be handled */
				for (;;);
			}
			break;

		case XUSBPS_TYPE_CONFIG_DESC:

			/* Set up the reply buffer with the configuration
			 * descriptor data.
			 */
			ReplyLen = XUsbPs_Ch9SetupCfgDescReply(
						Reply, XUSBPS_REQ_REPLY_LEN);

#ifdef CH9_DEBUG
			printf("get config %d/%d\n\r", ReplyLen, SetupData->wLength);
#endif

			ReplyLen = ReplyLen > SetupData->wLength ?
						SetupData->wLength : ReplyLen;

			Status = XUsbPs_EpBufferSend(InstancePtr, 0,
							Reply, ReplyLen);
			if (XST_SUCCESS != Status) {
				/* Failure case needs to be handled */
				for (;;);
			}
			break;


		case XUSBPS_TYPE_STRING_DESC:

			/* Set up the reply buffer with the string descriptor
			 * data.
			 */
			ReplyLen = XUsbPs_Ch9SetupStrDescReply(
						Reply, XUSBPS_REQ_REPLY_LEN,
						SetupData->wValue & 0xFF);

			ReplyLen = ReplyLen > SetupData->wLength ?
						SetupData->wLength : ReplyLen;

			Status = XUsbPs_EpBufferSend(InstancePtr, 0,
							Reply, ReplyLen);
			if (XST_SUCCESS != Status) {
				/* Failure case needs to be handled */
				for (;;);
			}
			break;

#ifdef MOUSE_SIMULATION
		case XUSBPS_TYPE_HID_DESC:

			/* Set up the reply buffer with the HID descriptor
			 * data.
			 */
			ReplyLen = XUsbPs_Ch9SetupHidDescReply(
						Reply, XUSBPS_REQ_REPLY_LEN);

			ReplyLen = ReplyLen > SetupData->wLength ?
						SetupData->wLength : ReplyLen;

			Status = XUsbPs_EpBufferSend(InstancePtr, 0,
							Reply, ReplyLen);
			if (XST_SUCCESS != Status) {
				/* Failure case needs to be handled */
				for (;;);
			}
			break;

		case XUSBPS_TYPE_REPORT_DESC:

			/* Set up the reply buffer with the report descriptor
			 * data.
			 */
			ReplyLen = XUsbPs_Ch9SetupReportDescReply(
						Reply, XUSBPS_REQ_REPLY_LEN);
#ifdef CH9_DEBUG
			printf("report desc len %d\n\r", ReplyLen);
#endif

			ReplyLen = ReplyLen > SetupData->wLength ?
						SetupData->wLength : ReplyLen;

			Status = XUsbPs_EpBufferSend(InstancePtr, 0,
							Reply, ReplyLen);
			if (XST_SUCCESS != Status) {
				/* Failure case needs to be handled */
				for (;;);
			}
			break;
#endif /* MOUSE_SIMULATION */

		default:
			Error = 1;
			break;
		}
		break;


	case XUSBPS_REQ_SET_CONFIGURATION:

		/*
		 * Only allow configuration index 1 as this is the only one we
		 * have.
		 */
		if ((SetupData->wValue & 0xff) != 1) {
			Error = 1;
			break;
		}

		UsbLocalPtr->CurrentConfig = SetupData->wValue & 0xff;


		/* Call the application specific configuration function to
		 * apply the configuration with the given configuration index.
		 */
		XUsbPs_SetConfiguration(InstancePtr,
						UsbLocalPtr->CurrentConfig);

		/* There is no data phase so ack the transaction by sending a
		 * zero length packet.
		 */
		XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);
		break;


	case XUSBPS_REQ_GET_CONFIGURATION:

		XUsbPs_EpBufferSend(InstancePtr, 0,
					&UsbLocalPtr->CurrentConfig, 1);
		break;


	case XUSBPS_REQ_CLEAR_FEATURE:
		switch(SetupData->bmRequestType & XUSBPS_STATUS_MASK) {
		case XUSBPS_STATUS_ENDPOINT:
			if(SetupData->wValue == XUSBPS_ENDPOINT_HALT) {
				int EpNum = SetupData->wIndex;

				if(EpNum & 0x80) {	/* In ep */
					XUsbPs_ClrBits(InstancePtr,
						XUSBPS_EPCRn_OFFSET(EpNum & 0xF),
						XUSBPS_EPCR_TXS_MASK);
				}else { /* Out ep */
					XUsbPs_ClrBits(InstancePtr,
						XUSBPS_EPCRn_OFFSET(EpNum),
						XUSBPS_EPCR_RXS_MASK);
				}
			}
			/* Ack the host ? */
			XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);
			break;

		default:
			Error = 1;
			break;
		}

		break;

	case XUSBPS_REQ_SET_FEATURE:
		switch(SetupData->bmRequestType & XUSBPS_STATUS_MASK) {
		case XUSBPS_STATUS_ENDPOINT:
			if(SetupData->wValue == XUSBPS_ENDPOINT_HALT) {
				int EpNum = SetupData->wIndex;

				if(EpNum & 0x80) {	/* In ep */
					XUsbPs_SetBits(InstancePtr,
						XUSBPS_EPCRn_OFFSET(EpNum & 0xF),
						XUSBPS_EPCR_TXS_MASK);

				}else { /* Out ep */
					XUsbPs_SetBits(InstancePtr,
						XUSBPS_EPCRn_OFFSET(EpNum),
						XUSBPS_EPCR_RXS_MASK);
				}
			}
			/* Ack the host ? */
			XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);

			break;

		default:
			/* We do not support any test mode */
			Error = 1;
			break;
		}

		break;


	/* For set interface, check the alt setting host wants */
	case XUSBPS_REQ_SET_INTERFACE:

#ifdef CH9_DEBUG
		printf("set interface %d/%d\n\r", SetupData->wValue, SetupData->wIndex);
#endif
		/* Not supported */
		/* XUsbPs_SetInterface(InstancePtr, SetupData->wValue, SetupData->wIndex); */

		/* Ack the host after device finishes the operation */
		Error = XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);
		if(Error) {
#ifdef CH9_DEBUG
			printf("EpBufferSend failed %d\n\r", Error);
#endif
		}
        break;

	default:
		Error = 1;
		break;
	}

	/* Set the send stall bit if there was an error */
	if (Error) {
#ifdef CH9_DEBUG
		printf("std dev req %d/%d error, stall 0 in out\n\r",
			SetupData->bRequest, (SetupData->wValue >> 8) & 0xff);
#endif
		XUsbPs_EpStall(InstancePtr, 0, XUSBPS_EP_DIRECTION_IN |
						XUSBPS_EP_DIRECTION_OUT);
	}
}

