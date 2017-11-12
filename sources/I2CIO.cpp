#include <inttypes.h>

#include "I2CIO.h"

I2CIO::I2CIO() {
    _dirMask = 0xFF; // mark all as INPUTs
    _shadow = 0x0; // no values set
    _initialised = false;
}

int I2CIO::begin(I2CDevice * _i2cdev) {
    theI2CDevice = _i2cdev;

    _shadow = theI2CDevice->readByte();
    _initialised = theI2CDevice->getLastResult() == I2C_OK;
    return ( _initialised);
}

void I2CIO::pinMode(uint8_t pin, uint8_t dir) {
    if (_initialised) {
        if (OUTPUT == dir) {
            _dirMask &= ~(1 << pin);
        } else {
            _dirMask |= (1 << pin);
        }
    }
}

void I2CIO::portMode(uint8_t dir) {

    if (_initialised) {
        if (dir == INPUT) {
            _dirMask = 0xFF;
        } else {
            _dirMask = 0x00;
        }
    }
}

uint8_t I2CIO::read(void) {
    uint8_t retVal = 0;

    if (_initialised) {
        retVal = (_dirMask & theI2CDevice->readByte());
    }
    return ( retVal);
}

int I2CIO::write(uint8_t value) {
    int status = 0;

    if (_initialised) {
        // Only write HIGH the values of the ports that have been initialised as
        // outputs updating the output shadow of the device
        _shadow = (value & ~(_dirMask));

        theI2CDevice->writeByte(_shadow);
        status = (theI2CDevice->getLastResult() == I2C_OK) ? 0 : 1;
    }
    return ( (status == 0));
}

uint8_t I2CIO::digitalRead(uint8_t pin) {
    uint8_t pinVal = 0;

    // Check if initialised and that the pin is within range of the device
    // -------------------------------------------------------------------
    if ((_initialised) && (pin <= 7)) {
        // Remove the values which are not inputs and get the value of the pin
        pinVal = this->read() & _dirMask;
        pinVal = (pinVal >> pin) & 0x01; // Get the pin value
    }
    return (pinVal);
}

int I2CIO::digitalWrite(uint8_t pin, uint8_t level) {
    uint8_t writeVal;
    int status = 0;

    // Check if initialised and that the pin is within range of the device
    // -------------------------------------------------------------------
    if ((_initialised) && (pin <= 7)) {
        // Only write to HIGH the port if the port has been configured as
        // an OUTPUT pin. Add the new state of the pin to the shadow
        writeVal = (1 << pin) & ~_dirMask;
        if (level == HIGH) {
            _shadow |= writeVal;

        } else {
            _shadow &= ~writeVal;
        }
        status = this->write(_shadow);
    }
    return ( status);
}

