#ifndef _LCD_H_
#define _LCD_H_

#include <inttypes.h>
#include <unistd.h>

inline static void waitUsec(uint16_t uSec) {
    usleep(uSec);
}

// LCD Commands
// ---------------------------------------------------------------------------
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
// ---------------------------------------------------------------------------
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off and cursor control
// ---------------------------------------------------------------------------
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for display/cursor shift
// ---------------------------------------------------------------------------
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for function set
// ---------------------------------------------------------------------------
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00


// Define COMMAND and DATA LCD Rs (used by send method).
// ---------------------------------------------------------------------------
#define COMMAND                 0
#define DATA                    1
#define FOUR_BITS               2


#define HOME_CLEAR_EXEC         2000

#define BACKLIGHT_OFF           0x00

#define BACKLIGHT_ON            0x08

typedef enum {
    POSITIVE, NEGATIVE
} t_backlighPol;

class LCD {
public:
    LCD();

    virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

    void clear();

    void home();

    void noDisplay();

    void display();

    void noBlink();

    void blink();

    void noCursor();

    void cursor();

    void scrollDisplayLeft();

    void scrollDisplayRight();

    void leftToRight();

    void rightToLeft();

    void moveCursorLeft();

    void moveCursorRight();

    void autoscroll();

    void noAutoscroll();

    void createChar(uint8_t location, uint8_t charmap[]);

    void setCursor(uint8_t col, uint8_t row);

    void backlight(void);

    void noBacklight(void);

    void on(void);

    void off(void);

    //
    // virtual class methods
    // --------------------------------------------------------------------------

    virtual void setBacklightPin(uint8_t value, t_backlighPol pol) {
    };

    virtual void setBacklight(uint8_t value) {
    };

    virtual size_t write(uint8_t value);
    
    void print(char * str);

protected:
    // Internal LCD variables to control the LCD shared between all derived
    // classes.
    uint8_t _displayfunction; // LCD_5x10DOTS or LCD_5x8DOTS, LCD_4BITMODE or 
    // LCD_8BITMODE, LCD_1LINE or LCD_2LINE
    uint8_t _displaycontrol; // LCD base control command LCD on/off, blink, cursor
    // all commands are "ored" to its contents.
    uint8_t _displaymode; // Text entry mode to the LCD
    uint8_t _numlines; // Number of lines of the LCD, initialized with begin()
    uint8_t _cols; // Number of columns in the LCD
    t_backlighPol _polarity; // Backlight polarity

private:
    void command(uint8_t value);

    virtual void send(uint8_t value, uint8_t mode) = 0;

};

#endif
