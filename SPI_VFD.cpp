#include "SPI_VFD.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "WProgram.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// SPI_VFD constructor is called).

SPI_VFD::SPI_VFD(uint8_t data, uint8_t clock, uint8_t chipselect)
{
  init(data, clock, chipselect);
}

SPI_VFD::SPI_VFD(uint8_t data, uint8_t clock)
{
  init(data, clock, 0);
}

void SPI_VFD::init(uint8_t data, uint8_t clock, uint8_t chipselect)
{
  _clock = clock;
  _data = data;
  _chipselect = chipselect;
  
  pinMode(_clock, OUTPUT);
  pinMode(_data, OUTPUT);
  
  if (_chipselect) {
     pinMode(_chipselect, OUTPUT);
  }

  _displayfunction = LCD_8BITMODE; // its actually SPI but its 8 bit SPI
  begin(20, 2);  
}

void SPI_VFD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  clear();
  home();

  // set up the display size
  command(LCD_FUNCTIONSET | _displayfunction);

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

  command(LCD_SETDDRAMADDR);  // go to address 0

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF; 
  display();

  command(LCD_SETDDRAMADDR);  // go to address 0
}

/********** high level commands, for the user! */
void SPI_VFD::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void SPI_VFD::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void SPI_VFD::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void SPI_VFD::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void SPI_VFD::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void SPI_VFD::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void SPI_VFD::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void SPI_VFD::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void SPI_VFD::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void SPI_VFD::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void SPI_VFD::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void SPI_VFD::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void SPI_VFD::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void SPI_VFD::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void SPI_VFD::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void SPI_VFD::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

void SPI_VFD::command(uint8_t value) {
  if (_chipselect) 
    digitalWrite(_chipselect, LOW);
  send(LCD_SPICOMMAND);
  send(value);
  if (_chipselect) 
    digitalWrite(_chipselect, HIGH);

  /*
  Serial.print(LCD_SPICOMMAND, HEX);
  Serial.print('\t');
  Serial.println(value, HEX);
  */
}

void SPI_VFD::write(uint8_t value) {
  if (_chipselect) 
    digitalWrite(_chipselect, LOW);
  send(LCD_SPIDATA);
  send(value);
  if (_chipselect) 
    digitalWrite(_chipselect, HIGH);

  /*
  Serial.print(LCD_SPIDATA, HEX);
  Serial.print('\t');
  Serial.println(value, HEX);
  */
}

/************ low level data pushing commands **********/

// write spi data
void SPI_VFD::send(uint8_t c) {
  //shiftOut(_data, _clock, MSBFIRST, value);

  //volatile uint8_t *sclkportreg = portOutputRegister(sclkport);
  //volatile uint8_t *sidportreg = portOutputRegister(sidport);

  int8_t i;

  //*sclkportreg |= sclkpin;
  digitalWrite(_clock, HIGH);

  for (i=7; i>=0; i--) {
    //*sclkportreg &= ~sclkpin;
    //SCLK_PORT &= ~_BV(SCLK);
    digitalWrite(_clock, LOW);
    
    if (c & _BV(i)) {
      //*sidportreg |= sidpin;
      //SID_PORT |= _BV(SID);
      digitalWrite(_data, HIGH);
    } else {
      //*sidportreg &= ~sidpin;
      //SID_PORT &= ~_BV(SID);
      digitalWrite(_data, LOW);
    }
    
    //*sclkportreg |= sclkpin;
    //SCLK_PORT |= _BV(SCLK);
    digitalWrite(_clock, HIGH);
  }
  
}
