#ifndef I2CAccess_H
#define I2CAccess_H

#include "I2CTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

class I2CAccess {
public:
    I2CAccess();
    
    virtual void writeBuffer(int devAddr, I2C_BUFFER_P buffer, int size) = 0;
    virtual void writeByte(int devAddr, int val) = 0;
    virtual void writeNBytes(int devAddr, int val, int numBytes) = 0;

    virtual void writeBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int size) = 0;
    virtual void writeByte(int devAddr, int regAddr, int val) = 0;
    virtual void writeNBytes(int devAddr, int regAddr, int val, int numBytes) = 0;

    virtual void readBuffer(int devAddr, I2C_BUFFER_P buffer, int numBytes) = 0;
    virtual int readByte(int devAddr) = 0;

    virtual void readBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int numBytes) = 0;
    virtual int readByte(int devAddr, int regAddr) = 0;
    
    virtual bool isAccessOk() = 0;
    
    I2C_Result getLastResult();
    
protected:
    I2C_Result lastResult;
};

#ifdef __cplusplus
}
#endif

#endif /* I2CACCESS_H */

