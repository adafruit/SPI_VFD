/*!
 * @file SPI_VFD.h
 */
#ifndef SPI_VFD_h
#define SPI_VFD_h

#include "Print.h"
#include <inttypes.h>

// commands
#define VFD_CLEARDISPLAY 0x01 //!< Clear display, set cursor position to zero
#define VFD_RETURNHOME 0x02   //!< Set cursor position to zero
#define VFD_ENTRYMODESET 0x04 //!< Sets the entry mode
#define VFD_DISPLAYCONTROL                                                     \
  0x08 //!< Controls display; does stuff like turning it off and on
#define VFD_CURSORSHIFT 0x10 //!< Lets you move the cursor
#define VFD_FUNCTIONSET 0x30 //!< Used to send the function to set the display
#define VFD_SETCGRAMADDR                                                       \
  0x40 //!< Used to set the CGRAM (character generator RAM)
#define VFD_SETDDRAMADDR 0x80 //!< Used to set the DDRAM (display data RAM)

// flags for display entry mode
#define VFD_ENTRYRIGHT 0x00 //!< Used to set the text to flow from right to left
#define VFD_ENTRYLEFT 0x02  //!< Used to set the text to flow from left to right
#define VFD_ENTRYSHIFTINCREMENT                                                \
  0x01 //!< Used to 'right justify' text from the cursor
#define VFD_ENTRYSHIFTDECREMENT                                                \
  0x00 //!< Used to 'left justify' text from the cursor

// flags for display on/off control
#define VFD_DISPLAYON 0x04  //!< Turns the display on
#define VFD_DISPLAYOFF 0x00 //!< Turns the display off
#define VFD_CURSORON 0x02   //!< Turns the cursor on
#define VFD_CURSOROFF 0x00  //!< Turns the cursor off
#define VFD_BLINKON 0x01    //!< Turns the blinking cursor on
#define VFD_BLINKOFF 0x00   //!< Turns the blinking cursor off

// flags for display/cursor shift
#define VFD_DISPLAYMOVE 0x08 //!< Flag for moving the display
#define VFD_CURSORMOVE 0x00  //!< Flag for moving the cursor
#define VFD_MOVERIGHT 0x04   //!< Flag for moving right
#define VFD_MOVELEFT 0x00    //!< Flag for moving left

// flags for function set
#define VFD_2LINE 0x08         //!< 8 bit mode
#define VFD_1LINE 0x00         //!< 4 bit mode
#define VFD_BRIGHTNESS25 0x03  //!< 25% brightness
#define VFD_BRIGHTNESS50 0x02  //!< 50% brightness
#define VFD_BRIGHTNESS75 0x01  //!< 75% brightness
#define VFD_BRIGHTNESS100 0x00 //!< 100% brightness

#define VFD_SPICOMMAND 0xF8 //!< Send command
#define VFD_SPIDATA 0xFA    //!< Send data

/*!
 * @brief Stores the state and function for the SPI VFD device
 */
class SPI_VFD : public Print {
public:
  /*!
   * @brief SPI_VFD constructor
   * @param data Data pin
   * @param clock Clock pin
   * @param strobe Strobe pin
   * @param brightness Desired brightness (default 100%)
   */
  SPI_VFD(uint8_t data, uint8_t clock, uint8_t strobe,
          uint8_t brightness = VFD_BRIGHTNESS100);

  /*!
   * @brief SPI_VFD constructor
   * @param data Data pin
   * @param clock Clock pin
   * @param strobe Strobe pin
   * @param brightness Desired brightness (default 100%)
   */
  void init(uint8_t data, uint8_t clock, uint8_t strobe, uint8_t brightness);
  /*!
   * @brief Starts connection with display
   * @param cols Number of columns in display
   * @param rows Number of rows in display
   * @param brightness Desired brightness (default 100%)
   */
  void begin(uint8_t cols, uint8_t rows,
             uint8_t brightness = VFD_BRIGHTNESS100);

  /*!
   * @brief High-level command to clear the display
   */
  void clear();
  /*!
   * @brief High-level command to set the cursor position to zero
   */
  void home();

  /*!
   * @brief Sets the brightness
   * @param brightness Desired brightness level
   */
  void setBrightness(uint8_t brightness);
  /*!
   * @brief Gets the brightness
   * @return Returns the brightness level
   */
  uint8_t getBrightness();
  /*!
   * @brief High-level command to turn the display off
   */
  void noDisplay();
  /*!
   * @brief High-level command to turn the display on
   */
  void display();
  /*!
   * @brief High-level command to turn the blinking cursor off
   */
  void noBlink();
  /*!
   * @brief High-level command to turn the blinking cursor on
   */
  void blink();
  /*!
   * @brief High-level command to turn the underline cursor off
   */
  void noCursor();
  /*!
   * @brief High-level command to turn the underline cursor on
   */
  void cursor();
  /*!
   * @brief High-level command to scroll the display left without changing the
   * RAM
   */
  void scrollDisplayLeft();
  /*!
   * @brief High-level command to scroll the display right without changing the
   * RAM
   */
  void scrollDisplayRight();
  /*!
   * @brief High-level command to make text flow left to right
   */
  void leftToRight();
  /*!
   * @brief High-level command to make text flow right to left
   */
  void rightToLeft();
  /*!
   * @brief High-level command to 'right justify' text from the cursor
   */
  void autoscroll();
  /*!
   * @brief High-level command to 'left justify' text from the cursor
   */
  void noAutoscroll();

  /*!
   * @brief High-level command that creates a custom character in CGRAM
   * @param location Location in cgram to fill
   * @param charmap[] Character map to use
   */
  void createChar(uint8_t, uint8_t[]);
  /*!
   * @brief High-level command that sets the location of the cursor
   * @param col Column to set the cursor in
   * @param row Row to set the cursor in
   */
  void setCursor(uint8_t, uint8_t);
#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  /*!
   * @brief Mid-level command that sends data to the display
   * @param value Data to send to the display
   */
  virtual void write(uint8_t);
#endif
  /*!
   * @brief Sends command to display
   * @param value Command to send
   */
  void command(uint8_t);

private:
  inline void send(uint8_t data);

  uint8_t _clock, _data, _strobe; // SPI interface

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines, _currline;
};

#endif
