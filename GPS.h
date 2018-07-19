// Author: Lok

#ifndef _GPS_H_
#define _GPS_H_

#include "def.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

struct SixChars{ // a wrap of char array of six
  char six_chars[6];
};

class GPS{
    private:
        uint8_t TX;
        uint8_t RX;
        bool is_software_serial;
        SoftwareSerial *SWSerial;
        bool waitForNextCharAvailable();
        char getNextChar();
        bool correctPrefix(char prefix[]);
        bool skipCommas(int number);
        SixChars getSixChars();
        float getLatitudeOrLongitude();
        
    public:
        GPS();
        GPS(uint8_t TXD, uint8_t RXD);
        ~GPS();
        uint8_t getTXDPin();
        uint8_t getRXDPin();
        TimeAndDate getTime();
        TimeAndDate getDate();
        TimeAndDate getTimeAndDate();
        Location getLocation();
};

#endif
