#include <stdlib.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>

#include "I2CAccessSystem.h"

//#define I2C_DEFAULT_CHANNEL_NUM 0
#define I2C_DEV_PATH  "/dev/i2c-%d"

I2CAccessSystem::I2CAccessSystem(int chanNum)
    : I2CAccess() {
    channelNumber = chanNum;
    i2cChannelFD = -1;
}

bool I2CAccessSystem::isAccessOk() {
    return checkAndSetupChannel() == I2C_OK;
}

I2C_Result I2CAccessSystem::checkAndSetupChannel()
{
    if (i2cChannelFD > 0) {
        return I2C_OK;
    }
    
    int status;
    char pathname[255];

    // define the path to open
    status = snprintf(pathname, sizeof (pathname), I2C_DEV_PATH, channelNumber);

    // check the filename
    if (status < 0 || status >= sizeof (pathname)) {
        return I2C_BAD_ACCESS;
    }

    // create a file descriptor for the I2C bus
    i2cChannelFD = open(pathname, O_RDWR);

    // check the device handle
    if (i2cChannelFD <= 0) {
        return I2C_BAD_ACCESS;
    }

    return I2C_OK;
}

I2C_Result I2CAccessSystem::checkAndSetupDev(int devAddr) {
    I2C_Result res = checkAndSetupChannel();
    if (res != I2C_OK) {
        return res;
    }

    // set to 7-bit addr
    if (ioctl(i2cChannelFD, I2C_TENBIT, 0) < 0) {
        return I2C_BAD_DEV;
    }

    // set the address
    if (ioctl(i2cChannelFD, I2C_SLAVE, devAddr) < 0) {
        return I2C_BAD_DEV;
    }

    return I2C_OK;
}

void I2CAccessSystem::writeBuffer(int devAddr, I2C_BUFFER_P buffer, int size) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
        if (write(i2cChannelFD, buffer, size) != size) {
            res = I2C_BAD_WRITE;
        }
    }
    
    lastResult = res;
}

void I2CAccessSystem::writeByte(int devAddr, int val) {
    I2C_BUFFER buffer;
    buffer[0] = val & 0xff;
    return writeBuffer(devAddr, buffer, 1);
}

void I2CAccessSystem::writeNBytes(int devAddr, int val, int numBytes) {
    I2C_BUFFER buffer;
    int index;
    for (index = 0; index < numBytes; index++) {
        buffer[index] = (uint8_t) ((val >> (8 * index)) & 0xff);
    }

    return writeBuffer(devAddr, buffer, numBytes);
}

void I2CAccessSystem::writeBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int size) {
    int fullSize;
    I2C_BUFFER_P bufferNew;

    // allocate the new buffer
    fullSize = size + 1; // adding addr to buffer
    bufferNew = (I2C_BUFFER_P) malloc(fullSize);

    // add the address to the data buffer
    bufferNew[0] = regAddr & 0xff;
    memcpy(&bufferNew[1], &buffer[0], size);

    // perform the write
    writeBuffer(devAddr, bufferNew, fullSize);

    // free the allocated memory
    free(bufferNew);
}

void I2CAccessSystem::writeByte(int devAddr, int regAddr, int val) {
    I2C_BUFFER buffer;
    buffer[0] = val & 0xff;
    return writeBuffer(devAddr, regAddr, buffer, 1);
}

void I2CAccessSystem::writeNBytes(int devAddr, int regAddr, int val, int numBytes) {
    I2C_BUFFER buffer;
    int index;
    for (index = 0; index < numBytes; index++) {
        buffer[index] = (uint8_t) ((val >> (8 * index)) & 0xff);
    }

    return writeBuffer(devAddr, regAddr, buffer, numBytes);
}

void I2CAccessSystem::readBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int numBytes) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
        I2C_BUFFER readAddrBuf;
        readAddrBuf[0] = regAddr & 0xff;

        // Write the address
        if (write(i2cChannelFD, readAddrBuf, 1) != 1) {
            res = I2C_BAD_READ_ADDR;
        } else {
            // Read the data 
            if ((numBytes > 0) && (buffer != NULL)) {
                if (read(i2cChannelFD, buffer, numBytes) != numBytes) {
                    res = I2C_BAD_READ;
                }
            }
        }
    }
    
    lastResult = res;
}

int I2CAccessSystem::readByte(int devAddr, int regAddr) {
    I2C_BUFFER buffer;
    readBuffer(devAddr, regAddr, buffer, 1);
    if (lastResult == I2C_OK){
        return (int)(buffer[0]);
    }
    return 0;
}


void I2CAccessSystem::readBuffer(int devAddr, I2C_BUFFER_P buffer, int numBytes) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
        // Read the data 
        if ((numBytes > 0) && (buffer != NULL)) {
            if (read(i2cChannelFD, buffer, numBytes) != numBytes) {
                res = I2C_BAD_READ;
            }
        }
    }
    
    lastResult = res;
}

int I2CAccessSystem::readByte(int devAddr) {
    I2C_BUFFER buffer;
    readBuffer(devAddr, buffer, 1);
    if (lastResult == I2C_OK){
        return (int)(buffer[0]);
    }
    return 0;
}
