#ifndef _I2CIO_H_
#define _I2CIO_H_

#include <inttypes.h>

#include "I2CDevice.h"

#define _I2CIO_VERSION "1.0.0"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

class I2CIO {
public:
    I2CIO();

    int begin(I2CDevice * _i2cdev);

    void pinMode(uint8_t pin, uint8_t dir);

    void portMode(uint8_t dir);

    uint8_t read(void);

    uint8_t digitalRead(uint8_t pin);

    int write(uint8_t value);

    int digitalWrite(uint8_t pin, uint8_t level);



private:
    uint8_t _shadow; // Shadow output
    uint8_t _dirMask; // Direction mask
    bool _initialised; // Initialised object
    I2CDevice * theI2CDevice;

};

#endif