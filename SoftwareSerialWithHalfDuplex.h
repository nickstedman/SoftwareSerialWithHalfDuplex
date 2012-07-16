/*
SoftwareSerialWithHalfDuplex.h (formerly SoftwareSerial.h) - 
Multi-instance software serial with half duplex library for Arduino/Wiring

By default the library works the same as the SoftwareSerial library, 
but by adding a couple of additional arguments it can be configured for 
half-duplex. In that case, the transmit pin is set by default to an input, 
with the pull-up set. When transmitting, the pin temporarily switches to 
an output until the byte is sent, then flips back to input. When a module 
is receiving it should not be able to transmit, and vice-versa. 
This library probably won't work as is if you need inverted-logic.

This is a first draft of the library and test programs. It appears to work, 
but has only been tested on a limited basis. The library also works with 
Robotis Bioloid AX-12 motors. Seems fairly reliable up to 57600 baud. 
As with all serial neither error checking, nor addressing are implemented, 
so it is likely that you will need to do this yourself. Also, you can make 
use of other protocols such as i2c. I am looking for any feedback, advice 
and help at this stage. Changes from SoftwareSerial have been noted with a 
comment of "//NS" for your review. Only a few were required.
Contact me at n.stedman@steddyrobots.com, or on the arduino forum.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SoftwareSerialWithHalfDuplex_h
#define SoftwareSerialWithHalfDuplex_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#define _SS_MAX_RX_BUFF 64 // RX buffer size
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class SoftwareSerialWithHalfDuplex : public Stream
{
private:
  // per object data
  uint8_t _receivePin;
  uint8_t _receiveBitMask;
  volatile uint8_t *_receivePortRegister;
  uint8_t _transmitPin;								//NS Added
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;
  
  uint16_t _rx_delay_centering;
  uint16_t _rx_delay_intrabit;
  uint16_t _rx_delay_stopbit;
  uint16_t _tx_delay;

  uint16_t _buffer_overflow:1;
  uint16_t _inverse_logic:1;
  uint16_t _full_duplex:1;							//NS Added

  // static data
  static char _receive_buffer[_SS_MAX_RX_BUFF]; 
  static volatile uint8_t _receive_buffer_tail;
  static volatile uint8_t _receive_buffer_head;
  static SoftwareSerialWithHalfDuplex *active_object;

  // private methods
  void recv();
  uint8_t rx_pin_read();
  void tx_pin_write(uint8_t pin_state);
  void setTX(uint8_t transmitPin);
  void setRX(uint8_t receivePin);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  SoftwareSerialWithHalfDuplex(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false, bool full_duplex = true);
  ~SoftwareSerialWithHalfDuplex();
  void begin(long speed);
  bool listen();
  void end();
  bool isListening() { return this == active_object; }
  bool overflow() { bool ret = _buffer_overflow; _buffer_overflow = false; return ret; }
  int peek();

  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  
  using Print::write;

  // public only for easy access by interrupt handlers
  static inline void handle_interrupt();
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
