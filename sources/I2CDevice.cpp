#include "I2CDevice.h"
#include "I2CAccessSystem.h"

I2CDevice::I2CDevice(I2CAccess * i2cAcc, int devAddr)
{
    i2cAccess = i2cAcc;
    deviceAddr = devAddr;
    lastResult = I2C_OK;
}

void I2CDevice::writeBuffer(I2C_BUFFER_P buffer, int size) {
    i2cAccess->writeBuffer(deviceAddr, buffer, size);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::writeByte(int val) {
    i2cAccess->writeByte(deviceAddr, val);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::writeNBytes(int val, int numBytes) {
    i2cAccess->writeNBytes(deviceAddr, val, numBytes);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::writeBuffer(int regAddr, I2C_BUFFER_P buffer, int size) {
    i2cAccess->writeBuffer(deviceAddr, regAddr, buffer, size);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::writeByte(int regAddr, int val) {
    i2cAccess->writeByte(deviceAddr, regAddr, val);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::writeNBytes(int regAddr, int val, int numBytes) {
    i2cAccess->writeNBytes(deviceAddr, regAddr, val, numBytes);
    lastResult = i2cAccess->getLastResult();
}

void I2CDevice::readBuffer(int regAddr, I2C_BUFFER_P buffer, int numBytes){
    i2cAccess->readBuffer(deviceAddr, regAddr, buffer, numBytes);
    lastResult = i2cAccess->getLastResult();
}

int I2CDevice::readByte(int regAddr) {
    int val = i2cAccess->readByte(deviceAddr, regAddr);
    lastResult = i2cAccess->getLastResult();
    return val;
}

void I2CDevice::readBuffer(I2C_BUFFER_P buffer, int numBytes){
    i2cAccess->readBuffer(deviceAddr, buffer, numBytes);
    lastResult = i2cAccess->getLastResult();
}

int I2CDevice::readByte() {
    int val = i2cAccess->readByte(deviceAddr);
    lastResult = i2cAccess->getLastResult();
    return val;
}

int I2CDevice::getDevAddr() {
    return deviceAddr;
}

I2C_Result I2CDevice::getLastResult() {
    return lastResult;
}
