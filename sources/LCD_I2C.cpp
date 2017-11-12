#include <inttypes.h>
#include "I2CIO.h"
#include "LCD_I2C.h"

// CONSTANT  definitions
// ---------------------------------------------------------------------------

// flags for backlight control
#define LCD_NOBACKLIGHT 0x00

#define LCD_BACKLIGHT   0x08


// Default library configuration parameters used by class constructor with
// only the I2C address field.
// ---------------------------------------------------------------------------
#define EN 6  // Enable bit

#define RW 5  // Read/Write bit

#define RS 4  // Register select bit

#define D4 0
#define D5 1
#define D6 2
#define D7 3


// CONSTRUCTORS
// ---------------------------------------------------------------------------

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev) {
    theI2CDevice = _i2cdev;
    config(EN, RW, RS, D4, D5, D6, D7);
}

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev, uint8_t backlighPin,
        t_backlighPol pol = POSITIVE) {
    theI2CDevice = _i2cdev;
    config(EN, RW, RS, D4, D5, D6, D7);
    setBacklightPin(backlighPin, pol);
}

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw,
        uint8_t Rs) {
    theI2CDevice = _i2cdev;
    config(En, Rw, Rs, D4, D5, D6, D7);
}

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw,
        uint8_t Rs, uint8_t backlighPin,
        t_backlighPol pol = POSITIVE) {
    theI2CDevice = _i2cdev;
    config(En, Rw, Rs, D4, D5, D6, D7);
    setBacklightPin(backlighPin, pol);
}

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw,
        uint8_t Rs, uint8_t d4, uint8_t d5,
        uint8_t d6, uint8_t d7) {
    theI2CDevice = _i2cdev;
    config(En, Rw, Rs, d4, d5, d6, d7);
}

LCD_I2C::LCD_I2C(I2CDevice * _i2cdev, uint8_t En, uint8_t Rw,
        uint8_t Rs, uint8_t d4, uint8_t d5,
        uint8_t d6, uint8_t d7, uint8_t backlighPin,
        t_backlighPol pol = POSITIVE) {
    theI2CDevice = _i2cdev;
    config(En, Rw, Rs, d4, d5, d6, d7);
    setBacklightPin(backlighPin, pol);
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin

void LCD_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {

    init(); // Initialise the I2C expander interface
    LCD::begin(cols, lines, dotsize);
}


// User commands - users can expand this section
//----------------------------------------------------------------------------
// Turn the (optional) backlight off/on

//
// setBacklightPin

void LCD_I2C::setBacklightPin(uint8_t value, t_backlighPol pol = POSITIVE) {
    _backlightPinMask = (1 << value);
    _polarity = pol;
    setBacklight(BACKLIGHT_OFF);
}

//
// setBacklight

void LCD_I2C::setBacklight(uint8_t value) {
    // Check if backlight is available
    // ----------------------------------------------------
    if (_backlightPinMask != 0x0) {
        // Check for polarity to configure mask accordingly
        // ----------------------------------------------------------
        if (((_polarity == POSITIVE) && (value > 0)) ||
                ((_polarity == NEGATIVE) && (value == 0))) {
            _backlightStsMask = _backlightPinMask & LCD_BACKLIGHT;
        } else {
            _backlightStsMask = _backlightPinMask & LCD_NOBACKLIGHT;
        }
        _i2cio.write(_backlightStsMask);
    }
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init

int LCD_I2C::init() {
    int status = 0;

    // initialize the backpack IO expander
    // and display functions.
    // ------------------------------------------------------------------------
    if (_i2cio.begin(theI2CDevice) == 1) {
        _i2cio.portMode(OUTPUT); // Set the entire IO extender to OUTPUT
        _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
        status = 1;
        _i2cio.write(0); // Set the entire port to LOW
    }
    return ( status);
}

//
// config

void LCD_I2C::config(uint8_t En, uint8_t Rw, uint8_t Rs,
        uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {

    _backlightPinMask = 0;
    _backlightStsMask = LCD_NOBACKLIGHT;
    _polarity = POSITIVE;

    _En = (1 << En);
    _Rw = (1 << Rw);
    _Rs = (1 << Rs);

    // Initialise pin mapping
    _data_pins[0] = (1 << d4);
    _data_pins[1] = (1 << d5);
    _data_pins[2] = (1 << d6);
    _data_pins[3] = (1 << d7);
}



// low level data pushing commands
//----------------------------------------------------------------------------

//
// send - write either command or data

void LCD_I2C::send(uint8_t value, uint8_t mode) {
    // No need to use the delay routines since the time taken to write takes
    // longer that what is needed both for toggling and enable pin an to execute
    // the command.

    if (mode == FOUR_BITS) {
        write4bits((value & 0x0F), COMMAND);
    } else {
        write4bits((value >> 4), mode);
        write4bits((value & 0x0F), mode);
    }
}

//
// write4bits

void LCD_I2C::write4bits(uint8_t value, uint8_t mode) {
    uint8_t pinMapValue = 0;

    // Map the value to LCD pin mapping
    // --------------------------------
    for (uint8_t i = 0; i < 4; i++) {
        if ((value & 0x1) == 1) {
            pinMapValue |= _data_pins[i];
        }
        value = (value >> 1);
    }

    // Is it a command or data
    // -----------------------
    if (mode == DATA) {
        mode = _Rs;
    }

    pinMapValue |= mode | _backlightStsMask;
    pulseEnable(pinMapValue);
}

//
// pulseEnable

void LCD_I2C::pulseEnable(uint8_t data) {
    _i2cio.write(data | _En); // En HIGH
    _i2cio.write(data & ~_En); // En LOW
}