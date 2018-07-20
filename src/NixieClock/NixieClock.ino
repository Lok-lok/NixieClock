#include "nixie_tube.h"
#include "GPS.h"
#include <DS3231.h>
#include <IRremote.h> 

#define MAX_WORLD_LINE_DIVERGENCE 2
#define WORLD_LINE_DIVERGENCE_FLASHING_TIME 1000
#define RANDOM_CONTENTS_FLASHING_TIME 3000
#define FLASHING_TIME_GAP 10

// define status
enum Status{
  INITIAL,
  CLOCK_TIME,
  GPS_TIME,
  CLOCK_DATE,
  TEMPERATURE,
  LOCATION,
  WORLD_LINE_DIVERGENCE,
  FLUSH,
  STEINS_GATE_DIVERGENCE,
  HOLD,
};

// Display parameter
uint8_t DISPLAY_TUBE_PIN[] = {3, 4, 5};
uint8_t DISPLAY_DATA_PIN[] = {6, 7, 8, 9};
uint8_t DISPLAY_DIGIT_PIN[] = {10, 1, 2, 3, 4, 5, 6, 7, 8, 9};
#define DISPLAY_EN 2
#define DISPLAY_LEFT_COMMA_INDEX 0
#define DISPLAY_RIGHT_COMMA_INDEX 11

// GPS parameter
#define GPS_TXD 11
#define GPS_RXD 12
#define TIMEZONE 0 // Please adjust the timezone when it is during DST

// IR parameter
#define IR_PIN 10
#define CLOCK_TIME_SIGNAL_ONE 0xE318261B
#define CLOCK_TIME_SIGNAL_TWO 0xFFA25D
#define GPS_TIME_SIGNAL_ONE 0x511DBB
#define GPS_TIME_SIGNAL_TWO 0xFF629D
#define CLOCK_DATE_SIGNAL_ONE 0xEE886D7F
#define CLOCK_DATE_SIGNAL_TWO 0xFFE21D
#define TEMPERATURE_SIGNAL_ONE 0x52A3D41F
#define TEMPERATURE_SIGNAL_TWO 0xFF22DD
#define LOCATION_SIGNAL_ONE 0xD7E84B1B
#define LOCATION_SIGNAL_TWO 0xFF02FD
#define WORLD_LINE_DIVERGENCE_SIGNAL_ONE 0x20FE4DBB
#define WORLD_LINE_DIVERGENCE_SIGNAL_TWO 0xFFC23D
#define FLUSH_SIGNAL_ONE 0xF076C13B
#define FLUSH_SIGNAL_TWO 0xFFE01F
#define STEINS_GATE_DIVERGENCE_SIGNAL_ONE 0x488F3CBB
#define STEINS_GATE_DIVERGENCE_SIGNAL_TWO 0xFF38C7
#define INITIAL_SIGNAL_ONE 0xC101E57B
#define INITIAL_SIGNAL_TWO 0xFF6897

// Initialization
NixieTubeComplexOfEight tubes(DISPLAY_EN, DISPLAY_TUBE_PIN, DISPLAY_DATA_PIN, DISPLAY_DIGIT_PIN, DISPLAY_LEFT_COMMA_INDEX, DISPLAY_RIGHT_COMMA_INDEX);
DS3231  rtc(SDA, SCL); //A4, A5 for UNO
GPS gps(GPS_TXD, GPS_RXD);
IRrecv ir(IR_PIN); 
decode_results results;

Status lasting_status, current_status;

void random_display(){
    int i, tube_amount = 12;
    if (tubes.get_right_comma() >= SIXTEEN) tube_amount--;
    if (tubes.get_left_comma() >= SIXTEEN) tube_amount--;
    char contents[EIGHT];
    for (i = 0; i < EIGHT; i++){
        int r = random(0, tube_amount);
        if (r < TEN) contents[i] = (char)(r + CHAR_ZERO);
        else if (r == TEN) contents[i] = LEFT_COMMA;
        else contents[i] = RIGHT_COMMA;
    }
    tubes.all_display(contents);
}

void random_world_line_divergence_display(){
    int i;
    char contents[EIGHT];
    for (i = 2; i < EIGHT; i++){
        contents[i] = (char)(CHAR_ZERO + random(0, TEN));
    }
    if (tubes.get_right_comma() < SIXTEEN) contents[1] = RIGHT_COMMA;
    else if (tubes.get_left_comma() < SIXTEEN) contents[1] = LEFT_COMMA;
    else contents[1] = EMPTY;
    contents[0] = (char)(CHAR_ZERO + random(0, MAX_WORLD_LINE_DIVERGENCE));
    tubes.all_display(contents);
}

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

void updateStatusFromIR(){
  if (ir.decode(&results)){
    uint32_t result = results.value;
    ir.resume(); // Receive the next value
    delay(5);
    while(ir.decode(&results) && (result < 0x100000 || result == 0xFFFFFFFF)){
      result = results.value;
      ir.resume(); // Receive the next value
      delay(5);
    }
    if (result == CLOCK_TIME_SIGNAL_ONE || result == CLOCK_TIME_SIGNAL_TWO)
      lasting_status = current_status = CLOCK_TIME;
    else if (result == GPS_TIME_SIGNAL_ONE || result == GPS_TIME_SIGNAL_TWO)
      lasting_status = current_status = GPS_TIME;
    else if (result == CLOCK_DATE_SIGNAL_ONE || result == CLOCK_DATE_SIGNAL_TWO)
      lasting_status = current_status = CLOCK_DATE;
    else if (result == TEMPERATURE_SIGNAL_ONE || result == TEMPERATURE_SIGNAL_TWO)
      lasting_status = current_status = TEMPERATURE;
    else if (result == LOCATION_SIGNAL_ONE || result == LOCATION_SIGNAL_TWO)
      lasting_status = current_status = LOCATION;
    else if (result == WORLD_LINE_DIVERGENCE_SIGNAL_ONE || result == WORLD_LINE_DIVERGENCE_SIGNAL_TWO)
      lasting_status = current_status = WORLD_LINE_DIVERGENCE;
    else if (result == FLUSH_SIGNAL_ONE || result == FLUSH_SIGNAL_TWO)
      current_status = FLUSH;
    else if (result == STEINS_GATE_DIVERGENCE_SIGNAL_TWO || result == STEINS_GATE_DIVERGENCE_SIGNAL_TWO)
      current_status = STEINS_GATE_DIVERGENCE;
    else if (result == INITIAL_SIGNAL_ONE || result == INITIAL_SIGNAL_TWO)
      lasting_status = current_status = INITIAL;
  }
}

void setup() {
  int i;
  lasting_status = current_status = INITIAL;
  randomSeed(analogRead(A0));
  tubes.all_off(); // flush
  delay(100);
  tubes.all_display(',');
  rtc.begin();
  ir.enableIRIn();
  tubes.all_off();
}

void loop() {
  switch (current_status){
    case INITIAL:
    {
      tubes.all_off();
      delay(100);
      break;
    }  
    case CLOCK_TIME:
    {
      TimeAndDate time = get_TimeAndDate_from_DS_time(rtc.getTime());
      tubes.time_display(time);
      delay(100);
      break;
    }
    case GPS_TIME:
    {
      TimeAndDate time = gps.getTime();
      time.hour += TIMEZONE;
      if (time.hour >= 24)
        time.hour -= 24;
      tubes.time_display(time);
      break;
    }
    case CLOCK_DATE:
    {
      TimeAndDate date = get_TimeAndDate_from_DS_time(rtc.getTime());
      tubes.date_display(date);
      delay(100);
      break;
    }
    case TEMPERATURE:
    {
      tubes.temperature_display(rtc.getTemp());
      delay(100);
      break;
    }
    case LOCATION:
    {
      tubes.location_display(gps.getLocation());
      break;
    }
    case WORLD_LINE_DIVERGENCE:
    {
      int i;
      for (i = 0; i < WORLD_LINE_DIVERGENCE_FLASHING_TIME / FLASHING_TIME_GAP; i++){
        random_world_line_divergence_display();
        delay(FLASHING_TIME_GAP);
      }
      current_status = HOLD;
      break;
    }
    case FLUSH:
    {
      int i;
      for (i = 0; i < RANDOM_CONTENTS_FLASHING_TIME / FLASHING_TIME_GAP; i++){
        random_display();
        delay(FLASHING_TIME_GAP);
      }
      current_status = lasting_status;
      break;
    }
    case STEINS_GATE_DIVERGENCE:
    {
      int i;
      for (i = 0; i < WORLD_LINE_DIVERGENCE_FLASHING_TIME / FLASHING_TIME_GAP; i++){
        random_world_line_divergence_display();
        delay(FLASHING_TIME_GAP);
      }
      char contents[8] = {'1', '.', '0', '4', '8', '5', '9', '6'};
      tubes.all_display(contents);
      current_status = HOLD;
      break;
    }
    case HOLD:
    {
      delay(100);
      break;
    }
  }
  updateStatusFromIR();
}

