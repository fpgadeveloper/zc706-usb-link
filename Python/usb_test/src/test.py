'''
Created on Nov 21, 2013

@author: Jeffrey
'''

import usb.core
import usb.util
import numpy
import time

# Base address of FMC107
FMC107_BASEADDR = 0x79000000

# Registers of the FMC107 peripheral
FMC107_ADCCTRL =   FMC107_BASEADDR + (0 * 4)
FMC107_PLLCTRL =   FMC107_BASEADDR + (1 * 4)
FMC107_ADCCTRLAB = FMC107_BASEADDR + (2 * 4)
FMC107_ADCCTRLCD = FMC107_BASEADDR + (3 * 4)
FMC107_ADCCTRLEF = FMC107_BASEADDR + (4 * 4)
FMC107_ADCCTRLGH = FMC107_BASEADDR + (5 * 4)

# Mux port selections

IICSW_USRCLK_SFP    = 0x01
IICSW_HDMI          = 0x02
IICSW_EEPROM        = 0x04
IICSW_PORT_EXPANDER = 0x08
IICSW_IIC_RTC       = 0x10
IICSW_FMC_HPC       = 0x20
IICSW_FMC_LPC       = 0x40
IICSW_PMBUS         = 0x80

# I2C addresses

IIC_EEPROM_ADDR       = 0x54
IIC_SCLK_RATE        = 100000
IIC_MUX_ADDRESS      = 0x74
IIC_PMBUS_ADDRESS    = 0x65
IIC_PORT_EXP_ADDRESS = 0x21
IIC_VMON1_ADDRESS    = 0x48
IIC_VMON2_ADDRESS    = 0x4A

# SLCR registers

SLCR_LOCK        = 0xF8000004 #< SLCR Write Protection Lock
SLCR_UNLOCK      = 0xF8000008 #< SLCR Write Protection Unlock
FPGA_RST_CTRL    = 0xF8000240 #< FPGA software reset control
FPGA0_CLK_CTRL   = 0xF8000170 #< FPGA clock control
FPGA1_CLK_CTRL   = 0xF8000180 #< FPGA clock control
FPGA2_CLK_CTRL   = 0xF8000190 #< FPGA clock control
FPGA3_CLK_CTRL   = 0xF80001A0 #< FPGA clock control

# FCLK resets
FPGA0_OUT_RST = 0x0001
FPGA1_OUT_RST = 0x0002
FPGA2_OUT_RST = 0x0004
FPGA3_OUT_RST = 0x0008

SLCR_LOCK_VAL    = 0x767B
SLCR_UNLOCK_VAL  = 0xDF0D

# Commands
CMD_MEM_RD  = 0x00010001
CMD_MEM_WR  = 0x00010002
CMD_I2C_RD  = 0x00020001
CMD_I2C_WR  = 0x00020002
CMD_SPI_RD  = 0x00030001
CMD_SPI_WR  = 0x00030002
CMD_USB_RD  = 0x00040001
CMD_USB_WR  = 0x00040002
CMD_SET_PLL = 0x000A0001
CMD_DMA_TRI = 0x000B0001

# Responses
REP_ACK     = 0x12345678
REP_ERR     = 0x87654321


def writeMem(eptx,eprx,addr,data):
    # send the write command
    data = numpy.array(data,dtype='uint32')
    msgarray = numpy.array([CMD_MEM_WR,addr],dtype='uint32')
    print('msgarray:',msgarray)
    msg = numpy.concatenate((msgarray,data))
    print('msg:',msg)
    eptx.write(msg.view(dtype='uint8'))
    # read the response
    buf = eprx.read(eprx.wMaxPacketSize,timeout=500)
    reply = numpy.array(buf).view(dtype='uint32')
    # check the reply
    if reply[0] == REP_ACK and reply[1] == addr:
        return(True)
    else:
        return(False)
    
def readMem(eptx,eprx,addr,length):
    # send the read command
    msg = numpy.array([CMD_MEM_RD,addr,length],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    # read the response
    buf = eprx.read(eprx.wMaxPacketSize,timeout=500)
    reply = numpy.array(buf).view(dtype='uint32')
    # check the reply
    if reply[0] == REP_ACK and reply[1] == addr:
        return(reply[2:])
    else:
        return(None)

def readI2C(eptx,eprx,addr,cmd,n):
    # send the read command
    msg = numpy.array([CMD_I2C_RD,addr,n,cmd],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    # read the response
    buf = eprx.read(eprx.wMaxPacketSize,timeout=500)
    reply = numpy.array(buf).view(dtype='uint32')
    # check the reply
    if reply[0] == REP_ACK and reply[1] == addr:
        return(reply[2:])
    else:
        return(None)

def releaseReset(eptx,eprx):
    writeMem(eptx,eprx,SLCR_UNLOCK, [SLCR_UNLOCK_VAL])
    writeMem(eptx,eprx,FPGA_RST_CTRL, [0x00000000])
    writeMem(eptx,eprx,SLCR_LOCK, [SLCR_LOCK_VAL])
    
def muxSelect(eptx,eprx,sel):
    # GPIO Code to pull MUX out of reset.
    data = readMem(eptx,eprx,0xe000a204,1)
    data[0] |= 0x2000
    #time.sleep(1)
    writeMem(eptx,eprx,0xe000a204, data)
    data = readMem(eptx,eprx,0xe000a208,1)
    data[0] |= 0x2000
    writeMem(eptx,eprx,0xe000a208, data)
    data = readMem(eptx,eprx,0xe000a040,1)
    data[0] |= 0x2000
    writeMem(eptx,eprx,0xe000a040, data)
    #time.sleep(1)
    # set the I2C MUX
    rxdata = readI2C(eptx,eprx,IIC_MUX_ADDRESS,sel,1)
    if len(rxdata) == 1:
        if rxdata[0] == sel:
            return(True)
    return(False)

def enableVoltMonitor(eptx,eprx):
    # set MUX to LPC FMC
    if not muxSelect(eptx,eprx,IICSW_FMC_LPC):
        print('Failed to MUX select LPC FMC')
        return(False)
    # read control register
    rxdata = readI2C(eptx,eprx,IIC_VMON1_ADDRESS,0x18,1)
    # start monitoring
    if not writeI2C(eptx,eprx,IIC_VMON1_ADDRESS,[0x18,rxdata[0]|0x09]):
        return(False)

if __name__ == '__main__':
    print('hello')
    """
    dev = usb.core.find(find_all=True)
    # loop through devices, printing vendor and product ids in decimal and hex
    for cfg in dev:
        print('Decimal VendorID=' + str(cfg.idVendor) + ' & ProductID=' + str(cfg.idProduct))
        print('Hexadecimal VendorID=' + hex(cfg.idVendor) + ' & ProductID=' + hex(cfg.idProduct))
    
    """
    # find our device
    dev = usb.core.find(idVendor=0xd7d, idProduct=0x0100)
    
    # was it found?
    if dev is None:
        raise ValueError('Device not found')
    
    print('dev.bLength: ',dev.bLength)
    print('dev.bNumConfigurations: ',dev.bNumConfigurations)
    print('dev.bDeviceClass: ',dev.bDeviceClass)
    
    
    # set the active configuration. With no arguments, the first
    # configuration will be the active one
    dev.set_configuration()
    
    # get an endpoint instance
    cfg = dev.get_active_configuration()
    interface_number = cfg[(0,0)].bInterfaceNumber
    alternate_setting = usb.control.get_interface(dev,interface_number)
    intf = usb.util.find_descriptor(
        cfg, bInterfaceNumber = interface_number,
        bAlternateSetting = alternate_setting
    )

    # find the OUT endpoint    
    eptx = usb.util.find_descriptor(
        intf,
        # match the first OUT endpoint
        custom_match = \
        lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_OUT
    )
    
    # find the IN endpoint    
    eprx = usb.util.find_descriptor(
        intf,
        # match the first OUT endpoint
        custom_match = \
        lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_IN
    )
    
    assert eptx is not None
    assert eprx is not None
    """
    print('ep.bDescriptorType: ',ep.bDescriptorType)
    print('ep.bEndpointAddress: ',ep.bEndpointAddress)
    print('ep.bInterval: ',ep.bInterval)
    print('ep.bLength: ',ep.bLength)
    print('ep.bRefresh: ',ep.bRefresh)
    print('ep.bSynchAddress: ',ep.bSynchAddress)
    print('ep.bmAttributes: ',ep.bmAttributes)
    print('ep.device: ',ep.device)
    print('ep.index: ',ep.index)
    print('ep.interface: ',ep.interface)
    print('ep.wMaxPacketSize: ',ep.wMaxPacketSize)
    """
    # write the data
    #ep.write('jest')
    """
    msg = numpy.array([CMD_MEM_RD,0x0,2],
                      dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    #eptx.write([x % 256 for x in range(513)])
    #buf = dev.read(1,10)
    buf = eprx.read(eprx.wMaxPacketSize)
    reply = numpy.array(buf).view(dtype='uint32')
    #print(reply)
    print([hex(r) for r in reply])
    """
    """
    # read
    msg = numpy.array([CMD_MEM_RD,0xe000a204,1],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    buf = eprx.read(eprx.wMaxPacketSize)
    reply = numpy.array(buf).view(dtype='uint32')
    print([hex(r) for r in reply])
    # read
    msg = numpy.array([CMD_MEM_RD,0xe000a204,1],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    buf = eprx.read(eprx.wMaxPacketSize)
    reply = numpy.array(buf).view(dtype='uint32')
    print([hex(r) for r in reply])
    # read
    msg = numpy.array([CMD_MEM_RD,0xe000a204,1],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    buf = eprx.read(eprx.wMaxPacketSize)
    reply = numpy.array(buf).view(dtype='uint32')
    print([hex(r) for r in reply])
    # write
    msg = numpy.array([CMD_MEM_WR,0xe000a204]+[0x2080],dtype='uint32')
    eptx.write(msg.view(dtype='uint8'))
    buf = eprx.read(eprx.wMaxPacketSize)
    reply = numpy.array(buf).view(dtype='uint32')
    print([hex(r) for r in reply])
    """
    enableVoltMonitor(eptx,eprx)
    '''
    data = readMem(eptx,eprx,0x80000000,1)
    print([hex(d) for d in data])
    writeMem(eptx,eprx,0x80000000,[0xDEADBEEF])
    data = readMem(eptx,eprx,0x80000000,1)
    print([hex(d) for d in data])
    '''
    