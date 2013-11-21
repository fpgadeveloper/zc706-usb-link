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
 * @file xusbps_ch9.h
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

#ifndef XUSBPS_CH9_H
#define XUSBPS_CH9_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xusbps_hw.h"
#include "xil_types.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

/*
 * Simulation type switch, default type is storage.
 */

/**
 * @name Request types
 * @{
 */
#define XUSBPS_REQ_TYPE_MASK	0x60	/**< Mask for request opcode */

#define XUSBPS_CMD_STDREQ	0x00	/**< */
#define XUSBPS_CMD_CLASSREQ	0x20	/**< */
#define XUSBPS_CMD_VENDREQ	0x40	/**< */

#define XUSBPS_REQ_REPLY_LEN	1024	/**< Max size of reply buffer. */
/* @} */

/**
 * @name Request Values
 * @{
 */
#define XUSBPS_REQ_GET_STATUS		0x00
#define XUSBPS_REQ_CLEAR_FEATURE	0x01
#define XUSBPS_REQ_SET_FEATURE		0x03
#define XUSBPS_REQ_SET_ADDRESS		0x05
#define XUSBPS_REQ_GET_DESCRIPTOR	0x06
#define XUSBPS_REQ_SET_DESCRIPTOR	0x07
#define XUSBPS_REQ_GET_CONFIGURATION	0x08
#define XUSBPS_REQ_SET_CONFIGURATION	0x09
#define XUSBPS_REQ_GET_INTERFACE	0x0a
#define XUSBPS_REQ_SET_INTERFACE	0x0b
#define XUSBPS_REQ_SYNC_FRAME		0x0c
#define XUSBPS_ENDPOINT_HALT		0x00
#define XUSBPS_DEVICE_REMOTE_WAKEUP 0x1
/* @} */

/**
 * @name Descriptor Types
 * @{
 */
#define XUSBPS_TYPE_DEVICE_DESC		0x01
#define XUSBPS_TYPE_CONFIG_DESC		0x02
#define XUSBPS_TYPE_STRING_DESC		0x03
#define XUSBPS_TYPE_IF_CFG_DESC		0x04
#define XUSBPS_TYPE_ENDPOINT_CFG_DESC	0x05
#define XUSBPS_TYPE_DEVICE_QUALIFIER	0x06
#define XUSBPS_TYPE_HID_DESC			0x21

#define XUSBPS_TYPE_REPORT_DESC		0x22
/* @} */


/**
 * @name USB Device States
 * @{
 */
#define XUSBPS_DEVICE_ATTACHED		0x00
#define XUSBPS_DEVICE_POWERED		0x01
#define XUSBPS_DEVICE_DEFAULT		0x02
#define XUSBPS_DEVICE_ADDRESSED	0x03
#define XUSBPS_DEVICE_CONFIGURED	0x04
#define XUSBPS_DEVICE_SUSPENDED	0x05
/* @} */

/**
 * @name Status type
 * @{
 */
#define XUSBPS_STATUS_MASK			0x3
#define XUSBPS_STATUS_DEVICE		0x0
#define XUSBPS_STATUS_INTERFACE	0x1
#define XUSBPS_STATUS_ENDPOINT		0x2
/* @} */

/**
 * @name EP Types
 * @{
 */
#define XUSBPS_EP_CONTROL		0
#define XUSBPS_EP_ISOCHRONOUS		1
#define XUSBPS_EP_BULK			2
#define XUSBPS_EP_INTERRUPT		3
/* @} */


/**
 * @name Device Classes
 * @{
 */
#define XUSBPS_CLASS_HID		0x03
#define XUSBPS_CLASS_STORAGE		0x08
#define XUSBPS_CLASS_VENDOR		0xFF
/* @} */


/**************************** Type Definitions *******************************/

typedef struct {
	u8  CurrentConfig;	/* Configuration used by Ch9 code. */
} XUsbPs_Local;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

int XUsbPs_Ch9HandleSetupPacket(XUsbPs *InstancePtr,
				 XUsbPs_SetupData *SetupData);


#ifdef __cplusplus
}
#endif

#endif /* XUSBPS_CH9_H */
