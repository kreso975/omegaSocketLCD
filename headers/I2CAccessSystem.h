#ifndef I2CAccessSystem_H
#define I2CAccessSystem_H

#include "I2CAccess.h"

#ifdef __cplusplus
extern "C" {
#endif 

class I2CAccessSystem : public I2CAccess {
public:
    I2CAccessSystem(int chanNum);

    void writeBuffer(int devAddr, I2C_BUFFER_P buffer, int size);
    void writeByte(int devAddr, int val);
    void writeNBytes(int devAddr, int val, int numBytes);

    void writeBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int size);
    void writeByte(int devAddr, int regAddr, int val);
    void writeNBytes(int devAddr, int regAddr, int val, int numBytes);

    void readBuffer(int devAddr, I2C_BUFFER_P buffer, int numBytes);
    int readByte(int devAddr);

    void readBuffer(int devAddr, int regAddr, I2C_BUFFER_P buffer, int numBytes);
    int readByte(int devAddr, int regAddr);
    
    bool isAccessOk();
private:
    int i2cChannelFD;
    int channelNumber;

    I2C_Result checkAndSetupChannel();
    I2C_Result checkAndSetupDev(int devAddr);
};

#ifdef __cplusplus
}
#endif 
#endif
