// Author: Lok

// Please remember to put GPS.h and nixie_tube.h in the same folder or in the libraries

// Ignore day of the week

#include "GPS.h"
#include "nixie_tube.h"
#include <DS3231.h>

// GPS parameter
#define TXD 11
#define RXD 12

// Display parameter
uint8_t DISPLAY_TUBE_PIN[] = {3, 4, 5};
uint8_t DISPLAY_DATA_PIN[] = {6, 7, 8, 9};
uint8_t DISPLAY_DIGIT_PIN[] = {10, 1, 2, 3, 4, 5, 6, 7, 8, 9};
#define DISPLAY_EN 2
#define DISPLAY_LEFT_COMMA_INDEX 0
#define DISPLAY_RIGHT_COMMA_INDEX 11

#define TZ 0 // Please adjust the timezone when it is during DST

GPS gps(TXD, RXD);
NixieTubeComplexOfEight tubes(DISPLAY_EN, DISPLAY_TUBE_PIN, DISPLAY_DATA_PIN, DISPLAY_DIGIT_PIN, DISPLAY_LEFT_COMMA_INDEX, DISPLAY_RIGHT_COMMA_INDEX);
DS3231 rtc(SDA, SCL);

TimeAndDate get_TimeAndDate_from_DS_time(Time DS_time){
  TimeAndDate time_and_date;
  time_and_date.hour = DS_time.hour;
  time_and_date.minute = DS_time.min;
  time_and_date.second = DS_time.sec;
  time_and_date.day = DS_time.date;
  time_and_date.month = DS_time.mon;
  time_and_date.year = DS_time.year % 100;
  return time_and_date;
}

bool success = false;

void setup() {
  int i;
  Serial.begin(9600);
  rtc.begin();
}

void loop() {
  if (success){
    // Output through serial
    Serial.print(rtc.getDateStr());
    Serial.print(" ");
    Serial.println(rtc.getTimeStr());

    // Output to nixie tubes
    tubes.time_display(get_TimeAndDate_from_DS_time(rtc.getTime()));
    delay(1000);
  }else{
    TimeAndDate time = gps.getTimeAndDate();
    if (time.hour > 24 || time.minute > 60 || time.second > 60){
      // Output through serial
      Serial.println("Time Not Available");
      
      // Output to nixie tubes
      uint8_t sec = rtc.getTime().sec;
      if (time.second % TWO == ZERO && tubes.get_left_comma() < SIXTEEN)
        tubes.all_display(LEFT_COMMA);
      else if (time.second % TWO != ZERO && tubes.get_right_comma() < SIXTEEN)
        tubes.all_display(RIGHT_COMMA);
      delay(500);
    }else{
      time.hour += TZ;
      if (time.hour >= 24){
        time.hour -= 24;
        time.day++;
        if ((time.month == 2 && ((time.year % FOUR != ZERO && time.day > 28) || time.day > 29)) ||
          ((time.month == 4 || time.month == 6 || time.month == 9 || time.month == 11) && time.day > 30)
          || time.day > 31){
          time.day = 1;
          time.month++;
        }
        if (time.month > 12){
          time.month = 1;
          time.year++;
        }
      }
      rtc.setTime(time.hour, time.minute, time.second);
      rtc.setDate(time.day, time.month, (int)time.year + 2000);
      // Output through serial
      Serial.print("Time Set at ");
      Serial.print(time.hour);
      Serial.print(":");
      Serial.print(time.minute);
      Serial.print(":");
      Serial.println(time.second);
      success = true;
    }
  }
}
