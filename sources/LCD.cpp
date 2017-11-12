#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "LCD.h"

LCD::LCD() {}

void LCD::begin( uint8_t cols, uint8_t lines, uint8_t dotsize )
{
    if ( lines > 1 )
    {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _cols = cols;

    // for some 1 line displays you can select a 10 pixel high font
    // ------------------------------------------------------------
    if ( (dotsize != LCD_5x8DOTS) && (lines == 1) )
    {
        _displayfunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way before 4.5V so we'll wait 
    // 50
    // ---------------------------------------------------------------------------
    usleep(100L * 1000L);

    //put the LCD into 4 bit or 8 bit mode
    // -------------------------------------
    if ( !(_displayfunction & LCD_8BITMODE) )
    {
        // this is according to the hitachi HD44780 datasheet
        // figure 24, pg 46

        // we start in 8bit mode, try to set 4 bit mode
        send( 0x03, FOUR_BITS );
        usleep(4500); // wait min 4.1ms

        // second try
        send( 0x03, FOUR_BITS );
        usleep(4500); // wait min 4.1ms

        // third go!
        send( 0x03, FOUR_BITS );
        usleep(150);

        // finally, set to 4-bit interface
        send( 0x02, FOUR_BITS );
    }
    else
    {
        // this is according to the hitachi HD44780 datasheet
        // page 45 figure 23

        // Send function set command sequence
        command( LCD_FUNCTIONSET | _displayfunction );
        usleep(4500);

        // second try
        command( LCD_FUNCTIONSET | _displayfunction );
        usleep(150);

        // third go
        command( LCD_FUNCTIONSET | _displayfunction );
    }

    // finally, set # lines, font size, etc.
    command( LCD_FUNCTIONSET | _displayfunction );

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear the LCD
    clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);

    backlight();

}

// Common LCD Commands
// ---------------------------------------------------------------------------

void LCD::clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    usleep(HOME_CLEAR_EXEC);    // this command is time consuming
}

void LCD::home()
{
    command(LCD_RETURNHOME); // set cursor position to zero
    usleep(HOME_CLEAR_EXEC); // this command is time consuming
}

void LCD::setCursor( uint8_t col, uint8_t row )
{
    const uint8_t row_offsetsDef[] = {0x00, 0x40, 0x14, 0x54};      // For regular LCDs
    const uint8_t row_offsetsLarge[] = {0x00, 0x40, 0x10, 0x50};    // For 16x4 LCDs

    if ( row >= _numlines )
        row = _numlines - 1;                                        // rows start at 0

    // 16x4 LCDs have special memory map layout
    // ----------------------------------------
    if ( _cols == 16 && _numlines == 4 )
        command( LCD_SETDDRAMADDR | (col + row_offsetsLarge[row]) );
    else
        command( LCD_SETDDRAMADDR | (col + row_offsetsDef[row]) );

}

// Turn the display on/off

void LCD::noDisplay()
{
    _displaycontrol &= ~LCD_DISPLAYON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

void LCD::display()
{
    _displaycontrol |= LCD_DISPLAYON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

// Turns the underline cursor on/off

void LCD::noCursor()
{
    _displaycontrol &= ~LCD_CURSORON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

void LCD::cursor()
{
    _displaycontrol |= LCD_CURSORON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

// Turns on/off the blinking cursor

void LCD::noBlink()
{
    _displaycontrol &= ~LCD_BLINKON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

void LCD::blink()
{
    _displaycontrol |= LCD_BLINKON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

// These commands scroll the display without changing the RAM

void LCD::scrollDisplayLeft(void)
{
    command( LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT );
}

void LCD::scrollDisplayRight(void)
{
    command( LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT );
}

// This is for text that flows Left to Right

void LCD::leftToRight(void)
{
    _displaymode |= LCD_ENTRYLEFT;
    command( LCD_ENTRYMODESET | _displaymode );
}

// This is for text that flows Right to Left

void LCD::rightToLeft(void)
{
    _displaymode &= ~LCD_ENTRYLEFT;
    command( LCD_ENTRYMODESET | _displaymode );
}

// This method moves the cursor one space to the right

void LCD::moveCursorRight(void)
{
    command( LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT );
}

// This method moves the cursor one space to the left

void LCD::moveCursorLeft(void)
{
    command( LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT );
}


// This will 'right justify' text from the cursor

void LCD::autoscroll(void)
{
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command( LCD_ENTRYMODESET | _displaymode );
}

// This will 'left justify' text from the cursor

void LCD::noAutoscroll(void)
{
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command( LCD_ENTRYMODESET | _displaymode );
}

// Write to CGRAM of new characters

void LCD::createChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations 0-7

    command(LCD_SETCGRAMADDR | (location << 3));
    usleep(30);

    for ( int i = 0; i < 8; i++ )
    {
        write( charmap[i] ); // call the virtual write method
        usleep(40);
    }
}

//
// Switch on the backlight

void LCD::backlight(void)
{
    setBacklight(BACKLIGHT_ON);
}

//
// Switch off the backlight

void LCD::noBacklight(void)
{
    setBacklight(BACKLIGHT_OFF);
}

//
// Switch fully on the LCD (backlight and LCD)

void LCD::on(void)
{
    display();
    backlight();
}

//
// Switch fully off the LCD (backlight and LCD) 

void LCD::off(void)
{
    noBacklight();
    noDisplay();
}

// General LCD commands - generic methods used by the rest of the commands
// ---------------------------------------------------------------------------

void LCD::command(uint8_t value)
{
    send( value, COMMAND );
}

size_t LCD::write(uint8_t value)
{
    send( value, DATA );
    return 1; // assume OK
}

void LCD::print(char * str)
{
    int i;
    char * p = str;
    for ( i = 0; i < strlen(str); i++ )
    {
        write(*p);
        p++;
    }
}
