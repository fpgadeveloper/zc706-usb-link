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
 *
 * @file xusbps_ch9_storage.h
 *
 * This file contains definitions used in the chapter 9 code.
 *
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.00a wgr  10/10/10 First release
 * </pre>
 *
 ******************************************************************************/

#ifndef XUSBPS_CLASS_STORAGE_H
#define XUSBPS_CLASS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xusbps_ch9_storage.h"

/************************** Constant Definitions *****************************/

/* Mass storage opcodes.
 */
#define USB_RBC_FORMAT			0x04
#define USB_RBC_INQUIRY			0x12
#define USB_RBC_MODE_SEL		0x15
#define USB_RBC_MODE_SENSE		0x1a
#define USB_RBC_READ			0x28
#define USB_RBC_READ_CAP		0x25
#define USB_RBC_VERIFY			0x2f
#define USB_RBC_WRITE			0x2a
#define USB_RBC_STARTSTOP_UNIT		0x1b
#define USB_RBC_TEST_UNIT_READY		0x00
#define USB_RBC_MEDIUM_REMOVAL		0x1e
#define USB_UFI_GET_CAP_LIST		0x23


/* Virtual Flash memory related definitions.
 */
#define VFLASH_SIZE		0x100000	/* 1MB space */
#define VFLASH_BLOCK_SIZE	0x200
#define VFLASH_NUM_BLOCKS	(VFLASH_SIZE/VFLASH_BLOCK_SIZE)


/* Class request opcodes.
 */
#define XUSBPS_CLASSREQ_MASS_STORAGE_RESET	0xFF
#define XUSBPS_CLASSREQ_GET_MAX_LUN		0xFE


/* SCSI machine states
 */
#define USB_EP_STATE_COMMAND		0
#define USB_EP_STATE_DATA		1
#define USB_EP_STATE_STATUS		2


/**************************** Type Definitions *******************************/

/* The following structures define USB storage class requests. The details of
 * the contents of those structures are not important in the context of this
 * example.
 */
typedef struct {
	u32 dCBWSignature;
	u32 dCBWTag;
	u32 dCBWDataTransferLength;
	u8  bmCBWFlags;
	u8  cCBWLUN;
	u8  bCBWCBLength;
	u8  CBWCB[16];
} __attribute__((__packed__))USB_CBW;

typedef	struct {
	u8 deviceType;
	u8 rmb;
	u8 version;
	u8 blah;
	u8 additionalLength;
	u8 sccs;
	u8 info0;
	u8 info1;
	u8 vendorID[8];
	u8 productID[16];
	u8 revision[4];
} __attribute__((__packed__))SCSI_INQUIRY;

typedef struct {
	u8  reserved[3];
	u8  listLength;
	u32 numBlocks;
	u8  descCode;
	u8  blockLengthMSB;
	u16 blockLength;
} __attribute__((__packed__))SCSI_CAP_LIST;

typedef struct {
	u32 numBlocks;
	u32 blockSize;
} __attribute__((__packed__))SCSI_READ_CAPACITY;

typedef struct {
	u8  opCode;
	u8  reserved1;
	u32 block;
	u8  reserved2;
	u16 length;
	u8  control;
} __attribute__((__packed__))SCSI_READ_WRITE;

typedef struct {
	u8  opCode;
	u8  immed;
	u8  reserved1;
	u8  reserved2;
	u8  start;
	u8  control;
} __attribute__((__packed__))SCSI_START_STOP;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void XUsbPs_HandleStorageReq(XUsbPs *InstancePtr, u8 EpNum,
				u8 *BufferPtr, u32 BufferLen);
void XUsbPs_ClassReq(XUsbPs *InstancePtr, XUsbPs_SetupData *SetupData);

#ifdef __cplusplus
}
#endif

#endif /* XUSBPS_CLASS_STORAGE_H */
