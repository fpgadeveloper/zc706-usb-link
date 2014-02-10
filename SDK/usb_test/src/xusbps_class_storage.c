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
 * @file xusbps_class_storage.c
 *
 * This file contains the implementation of the storage class code for the
 * example.
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

#include "xusbps.h"		/* USB controller driver */

#include "xusbps_ch9_storage.h"
#include "xusbps_class_storage.h"
#include "xil_printf.h"

#define CLASS_STORAGE_DEBUG

#ifdef CLASS_STORAGE_DEBUG
#define printf xil_printf
#endif

/************************** Constant Definitions *****************************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/* Pre-manufactured response to the SCSI Inquirey command.
 */
static SCSI_INQUIRY scsiInquiry = {
	0x00,
	0x80,
	0x00,
	0x01,
	0x1f,
	0x00,
	0x00,
	0x00,
	{"Xilinx  "},		/* Vendor ID:  must be  8 characters long. */
	{"PS USB VirtDisk"},	/* Product ID: must be 16 characters long. */
	{"1.00"}		/* Revision:   must be  4 characters long. */
};

/*****************************************************************************/
/**
* This function handles Reduced Block Command (RBC) requests from the host.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	EpNum is the number of the endpoint on which the RBC was received.
* @param	BufferPtr is the data buffer containing the RBC or data.
* @param	BufferLen is the length of the data buffer.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XUsbPs_HandleStorageReq(XUsbPs *InstancePtr, u8 EpNum,
				u8 *BufferPtr, u32 BufferLen)
{
	USB_CBW	*CBW;
	u32	Offset;
	/* Buffer for virtual flash disk space. */
	static u8 VirtFlash[VFLASH_SIZE];
	static u8 *VirtFlashWritePointer = VirtFlash;
	/* Static variables used for data transfers.*/
	static int	rxBytesLeft;
	static USB_CBW	lastCBW;
	int i;
	int toprint;

	/* Local transmit buffer for simple replies. */
	static u8	txBuffer[128];

	/* Current SCSI machine state. */
	static int	phase = USB_EP_STATE_COMMAND;

	toprint = (BufferLen > 10)? 10:BufferLen;
	xil_printf("Buffer: ");
	for(i=0;i<toprint;i++)
		xil_printf("%X ",BufferPtr[i]);
	xil_printf(" Length: %d\n\r",BufferLen);

	for(i=0;i<BufferLen;i++)
		txBuffer[i] = BufferPtr[i]+10;
	XUsbPs_EpBufferSend(InstancePtr, 1, txBuffer,BufferLen);


//	/* COMMAND phase. */
//	if (USB_EP_STATE_COMMAND == phase) {
//		CBW = (USB_CBW *) BufferPtr;
//
//		switch (CBW->CBWCB[0]) {
//		case USB_RBC_INQUIRY:
//#ifdef CLASS_STORAGE_DEBUG
// 			printf("SCSI: INQUIRY\n");
//#endif
//			XUsbPs_EpBufferSend(InstancePtr, 1,
//						(void *) &scsiInquiry,
//						sizeof(scsiInquiry));
//			/* Send Success Status 	 */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength = 0;
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (void *) CBW, 13);
//			break;
//
//
//		case USB_UFI_GET_CAP_LIST:
//		{
//			SCSI_CAP_LIST	*CapList;
//
//			CapList = (SCSI_CAP_LIST *) txBuffer;
//#ifdef CLASS_STORAGE_DEBUG
// 			printf("SCSI: CAPLIST\n");
//#endif
//			CapList->listLength	= 8;
//			CapList->descCode	= 3;
//			CapList->numBlocks	= htonl(VFLASH_NUM_BLOCKS);
//			CapList->blockLength	= htons(VFLASH_BLOCK_SIZE);
//			XUsbPs_EpBufferSend(InstancePtr, 1, txBuffer,
//						      sizeof(SCSI_CAP_LIST));
//			/* Send Success Status
//			 */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength =
//				be2le(be2le(CBW->dCBWDataTransferLength) -
//						      sizeof(SCSI_CAP_LIST));
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *) CBW, 13);
//			break;
//		}
//
//		case USB_RBC_READ_CAP:
//		{
//			SCSI_READ_CAPACITY	*Cap;
//
//			Cap = (SCSI_READ_CAPACITY *) txBuffer;
//#ifdef CLASS_STORAGE_DEBUG
// 			printf("SCSI: READCAP\n");
//#endif
//			Cap->numBlocks = htonl(VFLASH_NUM_BLOCKS - 1);
//			Cap->blockSize = htonl(VFLASH_BLOCK_SIZE);
//			XUsbPs_EpBufferSend(InstancePtr, 1, txBuffer,
//					      sizeof(SCSI_READ_CAPACITY));
//			/* Send Success Status  */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength = 0;
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *) CBW, 13);
//			break;
//		}
//
//		case USB_RBC_READ:
//			Offset = htonl(((SCSI_READ_WRITE *) CBW->CBWCB)->
//				       block) * VFLASH_BLOCK_SIZE;
//#ifdef CLASS_STORAGE_DEBUG
//			printf("SCSI: READ Offset 0x%08x\n", (int) Offset);
//#endif
//			XUsbPs_EpBufferSend(InstancePtr, 1, &VirtFlash[Offset],
//				      htons(((SCSI_READ_WRITE *) CBW->CBWCB)->
//					    length) * VFLASH_BLOCK_SIZE);
//			/* Send Success Status */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength = 0;
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *) CBW, 13);
//			break;
//
//		case USB_RBC_MODE_SENSE:
//#ifdef CLASS_STORAGE_DEBUG
// 			printf("SCSI: MODE SENSE\n");
//#endif
//			XUsbPs_EpBufferSend(InstancePtr, 1,
//				      (u8 *) "\003\000\000\000", 4);
//
//			/* Send Success Status */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength =
//				be2le(be2le(CBW->dCBWDataTransferLength) - 4);
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *) CBW, 13);
//			break;
//
//
//		case USB_RBC_TEST_UNIT_READY:
//		case USB_RBC_MEDIUM_REMOVAL:
//		case USB_RBC_VERIFY:
//#ifdef CLASS_STORAGE_DEBUG
// 			printf("SCSI: TEST UNIT READY\n");
//#endif
//			/* Send Success Status */
//			CBW->dCBWSignature = 0x55534253;
//			CBW->dCBWDataTransferLength = 0;
//			CBW->bmCBWFlags = 0;
//
//			XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *) CBW, 13);
//			break;
//
//
//		case USB_RBC_WRITE:
//			Offset = htonl(((SCSI_READ_WRITE *) CBW->CBWCB)->
//				       block) * VFLASH_BLOCK_SIZE;
//#ifdef CLASS_STORAGE_DEBUG
//			printf("SCSI: WRITE Offset 0x%08x\n", (int) Offset);
//#endif
//			VirtFlashWritePointer = &VirtFlash[Offset];
//			/* Save the CBW for the DATA and STATUS phases. */
//			lastCBW = *CBW;
//			rxBytesLeft =
//				htons(((SCSI_READ_WRITE *) CBW->CBWCB)->length)
//							* VFLASH_BLOCK_SIZE;
//
//			phase = USB_EP_STATE_DATA;
//			break;
//
//
//		case USB_RBC_STARTSTOP_UNIT:
//		{
//			u8 immed;
//
//			immed = ((SCSI_START_STOP *) CBW->CBWCB)->immed;
//#ifdef CLASS_STORAGE_DEBUG
//			printf("SCSI: START/STOP unit: immed %02x\n", immed);
//#endif
//			/* If the immediate bit is 0 we are supposed to send
//			 * a success status.
//			 */
//			if (0 == (immed & 0x01)) {
//				/* Send Success Status */
//				CBW->dCBWSignature = 0x55534253;
//				CBW->dCBWDataTransferLength = 0;
//				CBW->bmCBWFlags = 0;
//
//				XUsbPs_EpBufferSend(InstancePtr, 1,
//							(u8 *) CBW, 13);
//			}
//			break;
//		}
//
//
//		/* Commands that we do not support for this example. */
//		case 0x04:	/* Format Unit */
//		case 0x15:	/* Mode Select */
//		case 0x5e:	/* Persistent Reserve In */
//		case 0x5f:	/* Persistent Reserve Out */
//		case 0x17:	/* Release */
//		case 0x03:	/* Request Sense */
//		case 0x16:	/* Reserve */
//		case 0x35:	/* Sync Cache */
//		case 0x3b:	/* Write Buffer */
//#ifdef CLASS_STORAGE_DEBUG
//			printf("SCSI: Got unhandled command %02x\n", CBW->CBWCB[0]);
//#endif
//		default:
//			break;
//		}
//	}
//	/* DATA phase.
//	 */
//	else if (USB_EP_STATE_DATA == phase) {
//		switch (lastCBW.CBWCB[0]) {
//		case USB_RBC_WRITE:
//			/* Copy the data we just read into the VirtFlash buffer. */
//			memcpy(VirtFlashWritePointer, BufferPtr, BufferLen);
//			VirtFlashWritePointer += BufferLen;
//
//			rxBytesLeft -= BufferLen;
//
//			if (rxBytesLeft <= 0) {
//				/* Send Success Status */
//				lastCBW.dCBWSignature = 0x55534253;
//				lastCBW.dCBWDataTransferLength = 0;
//				lastCBW.bmCBWFlags = 0;
//
//				XUsbPs_EpBufferSend(InstancePtr, 1,
//						      (void *) &lastCBW, 13);
//
//				phase = USB_EP_STATE_COMMAND;
//			}
//			break;
//		}
//	}
}


/*****************************************************************************/
/**
* This function handles a Storage Class Setup request from the host.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	SetupData is the setup data structure containing the setup
*		request.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XUsbPs_ClassReq(XUsbPs *InstancePtr, XUsbPs_SetupData *SetupData)
{
	static u8 MaxLUN = 0;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(SetupData   != NULL);


	switch (SetupData->bRequest) {

	case XUSBPS_CLASSREQ_MASS_STORAGE_RESET:
		XUsbPs_EpBufferSend(InstancePtr, 0, NULL, 0);
		break;

	case XUSBPS_CLASSREQ_GET_MAX_LUN:
		XUsbPs_EpBufferSend(InstancePtr, 0, &MaxLUN, 1);
		break;

	default:
		XUsbPs_EpStall(InstancePtr, 0, XUSBPS_EP_DIRECTION_IN);
		break;
	}
}


