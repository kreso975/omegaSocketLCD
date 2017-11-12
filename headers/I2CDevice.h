#ifndef I2CDevice_h
#define I2CDevice_h

#include "I2CTypes.h"
#include "I2CAccess.h"

class I2CDevice {
public:
    I2CDevice(I2CAccess * i2cAccess, int devAddr);

    void writeBuffer(I2C_BUFFER_P buffer, int size);
    void writeByte(int val);
    void writeNBytes(int val, int numBytes);

    void writeBuffer(int regAddr, I2C_BUFFER_P buffer, int size);
    void writeByte(int regAddr, int val);
    void writeNBytes(int regAddr, int val, int numBytes);

    void readBuffer(I2C_BUFFER_P buffer, int numBytes);
    int readByte();

    void readBuffer(int regAddr, I2C_BUFFER_P buffer, int numBytes);
    int readByte(int regAddr);
    
    int getDevAddr();
    
    I2C_Result getLastResult();
private:
    I2CAccess * i2cAccess;
    int deviceAddr;
    I2C_Result lastResult;
};

#endif
