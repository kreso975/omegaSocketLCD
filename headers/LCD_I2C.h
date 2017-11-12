#ifndef LCD_I2C_h
#define LCD_I2C_h

#include <inttypes.h>

#include "I2CIO.h"
#include "LCD.h"
#include "I2CDevice.h"

class LCD_I2C : public LCD {
public:
    LCD_I2C(I2CDevice * _i2cdev);
    LCD_I2C(I2CDevice * _i2cdev, uint8_t backlighPin, t_backlighPol pol);
    LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw, uint8_t Rs);
    LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw, uint8_t Rs,
            uint8_t backlighPin, t_backlighPol pol);
    LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw, uint8_t Rs,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw, uint8_t Rs,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
            uint8_t backlighPin, t_backlighPol pol);
    virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

    virtual void send(uint8_t value, uint8_t mode);

    void setBacklightPin(uint8_t value, t_backlighPol pol);

    void setBacklight(uint8_t value);

private:
    int init();

    void config(uint8_t En, uint8_t Rw, uint8_t Rs,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

    void write4bits(uint8_t value, uint8_t mode);

    void pulseEnable(uint8_t);

    uint8_t _backlightPinMask; // Backlight IO pin mask
    uint8_t _backlightStsMask; // Backlight status mask
    I2CIO _i2cio; // I2CIO PCF8574* expansion module driver I2CLCDextraIO
    uint8_t _En; // LCD expander word for enable pin
    uint8_t _Rw; // LCD expander word for R/W pin
    uint8_t _Rs; // LCD expander word for Register Select pin
    uint8_t _data_pins[4]; // LCD data lines
    I2CDevice * theI2CDevice;

};

#endif
