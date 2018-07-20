// Author: Lok

#include "GPS.h"
#include <SoftwareSerial.h>

// The period of sending information is one second.
#define SAFETY_TIME_IN_US 1000000
#define WAITING_TIME_PERIOD_IN_US 5
#define PREFIX_SIZE 5

char GPGGA[] = {'G', 'P', 'G', 'G', 'A'};
char GPRMC[] = {'G', 'P', 'R', 'M', 'C'};

// does not check duplicate '.'
bool areDigits(char chars[]){
    int i;
    for (i = 0; i < sizeof(chars) / sizeof(char); i++){
        if ((chars[i] < CHAR_ZERO || chars[i] > CHAR_NINE) && chars[i] != '.') return false;
    }
    return true;
}

int parseToInt(char chars[]){
    int i, sum = 0;
    for (i = 0; i < sizeof(chars)/sizeof(char); i++){
        sum *= 10;
        sum += (chars[i] - CHAR_ZERO);
    }
    return sum;
}

float parseToFloat(char chars[], int count){
    float sum = 0, decimal_sum = 0;
    int i, point_pos = 0;
    while (point_pos < count && chars[point_pos] != '.') point_pos++;
    for (i = 0; i < point_pos; i++){
        sum *= 10;
        sum += (chars[i] - CHAR_ZERO);
    }
    for (i = count - 1; i > point_pos; i--){
        decimal_sum += (chars[i] - CHAR_ZERO);
        decimal_sum /= 10;
    }
    return (sum + decimal_sum) / 100;
}

GPS::GPS(){
    is_software_serial = false;
    TX = UINT8_MAX;
    RX = UINT8_MAX;
    SWSerial = NULL;
    Serial.begin(9600);
}

GPS::GPS(uint8_t TXD, uint8_t RXD){
    is_software_serial = true;
    TX = TXD;
    RX = RXD;
    SWSerial = new SoftwareSerial(TXD, RXD);
    SWSerial -> begin(9600);
}

GPS::~GPS(){
    if (is_software_serial)
        delete(SWSerial);
    else
        Serial.end();
}

bool GPS::waitForNextCharAvailable(){
    int i = 0;
    if (is_software_serial){
        while (!SWSerial -> available() && i < SAFETY_TIME_IN_US)
            delayMicroseconds(WAITING_TIME_PERIOD_IN_US);
            i += WAITING_TIME_PERIOD_IN_US;
    } else{
        while (!Serial.available() && i < SAFETY_TIME_IN_US)
            delayMicroseconds(WAITING_TIME_PERIOD_IN_US);
            i += WAITING_TIME_PERIOD_IN_US;
    }
    return i < SAFETY_TIME_IN_US;
}

char GPS::getNextChar(){
    if (is_software_serial) return SWSerial -> read();
    return Serial.read();
}

bool GPS::correctPrefix(char prefix[]){
    int i;
    char c;
    bool b = true;
    for (i = 0; i < PREFIX_SIZE && b && waitForNextCharAvailable(); i++){
        c = getNextChar();
        b = c == prefix[i];
    }
    if (i < PREFIX_SIZE){
        b = false;
    }
    return b;
}

bool GPS::skipCommas(int number){
    int count = 0;
    while(count < number && waitForNextCharAvailable()){
        char c = getNextChar();
        if (c == ',') count++;
    }
    return count == number;
}

SixChars GPS::getSixChars(){
    SixChars ret = {{-1, -1, -1, -1, -1, -1}};
    int i;
    for(i = 0; i < 6 && waitForNextCharAvailable(); i++){
        ret.six_chars[i] = getNextChar();
    }
    return ret;
}

float GPS::getLatitudeOrLongitude(){
    int i, count;
    char chars[100];
    for (i = 0; i < 100; i++){
        chars[i] = -1;
    }
    if(waitForNextCharAvailable()){
        char c = getNextChar();
        int i = 0;
        while (i < 100 && c != ',' && waitForNextCharAvailable()){
            chars[i] = c;
            c = getNextChar();
            i++;
        }
        if (c != ',' && i < 100) chars[0] = -1;
    }
    count = 0;
    while (count < sizeof(chars)/sizeof(char) && chars[count] >= 0) count++;
    if (count == 0) return -1;
    char *ret = (char*)malloc(sizeof(char) * i);
    for(i = 0; i < count; i++){
        ret[i] = chars[i];
    }
    if(!areDigits(ret)) return -1;
    free(ret);
    return parseToFloat(ret, count);
}

uint8_t GPS::getTXDPin(){
    if (is_software_serial)
        return TX;
    return UINT8_MAX;
}

uint8_t GPS::getRXDPin(){
    if (is_software_serial)
        return RX;
    return UINT8_MAX;
}

TimeAndDate GPS::getTime(){
    TimeAndDate time;
    char c;
    while (waitForNextCharAvailable()){
        c = getNextChar();
        if(c == '$' && correctPrefix(GPRMC)){
            int i;
            char time_chars[6];
            if(!skipCommas(1)) break;
            SixChars temp = getSixChars();
            for (i = 0; i < sizeof(time_chars) / sizeof(char); i++) time_chars[i] = temp.six_chars[i];
            if(time_chars[5] < 0) break;
            if(!areDigits(time_chars)) break;
            char temp1[] = {time_chars[0], time_chars[1]};
            time.hour = (uint8_t)parseToInt(temp1);
            char temp2[] = {time_chars[2], time_chars[3]};
            time.minute = (uint8_t)parseToInt(temp2);
            char temp3[] = {time_chars[4], time_chars[5]};
            time.second = (uint8_t)parseToInt(temp3);
            break;
        }
    }
    return time;
}

TimeAndDate GPS::getDate(){
    TimeAndDate date;
    char c;
    while (waitForNextCharAvailable()){
        c = getNextChar();
        if(c == '$' && correctPrefix(GPRMC)){
            int i;
            char date_chars[6];
            if(!skipCommas(9)) break;
            SixChars temp = getSixChars();
            for (i = 0; i < sizeof(date_chars) / sizeof(char); i++) date_chars[i] = temp.six_chars[i];
            if(date_chars[5] < 0) break;
            if(!areDigits(date_chars)) break;
            char temp1[] = {date_chars[0], date_chars[1]};
            date.day = (uint8_t)parseToInt(temp1);
            char temp2[] = {date_chars[2], date_chars[3]};
            date.month = (uint8_t)parseToInt(temp2);
            char temp3[] = {date_chars[4], date_chars[5]};
            date.year = (uint8_t)parseToInt(temp3);
            break;
        }
    }
    return date;
}

TimeAndDate GPS::getTimeAndDate(){
    TimeAndDate time_and_date;
    char c;
    while (waitForNextCharAvailable()){
        c = getNextChar();
        if(c == '$' && correctPrefix(GPRMC)){
            int i;
            char time_chars[6], date_chars[6];
            if(!skipCommas(1)) break;
            SixChars temp = getSixChars();
            for (i = 0; i < sizeof(time_chars) / sizeof(char); i++) time_chars[i] = temp.six_chars[i];
            if(time_chars[5] < 0) break;
            if(!areDigits(time_chars)) break;
            char temp1[] = {time_chars[0], time_chars[1]};
            time_and_date.hour = (uint8_t)parseToInt(temp1);
            char temp2[] = {time_chars[2], time_chars[3]};
            time_and_date.minute = (uint8_t)parseToInt(temp2);
            char temp3[] = {time_chars[4], time_chars[5]};
            time_and_date.second = (uint8_t)parseToInt(temp3);
            if(!skipCommas(8)) break;
            temp = getSixChars();
            for (i = 0; i < sizeof(date_chars) / sizeof(char); i++) date_chars[i] = temp.six_chars[i];
            if(date_chars[5] < 0) break;
            if(!areDigits(date_chars)) break;
            char temp4[] = {date_chars[0], date_chars[1]};
            time_and_date.day = (uint8_t)parseToInt(temp4);
            char temp5[] = {date_chars[2], date_chars[3]};
            time_and_date.month = (uint8_t)parseToInt(temp5);
            char temp6[] = {date_chars[4], date_chars[5]};
            time_and_date.year = (uint8_t)parseToInt(temp6);
            break;
        }
    }
    return time_and_date;
}

Location GPS::getLocation(){
    Location loc;
    char c;
    while (waitForNextCharAvailable()){
        c = getNextChar();
        if(c == '$' && correctPrefix(GPRMC)){
            bool available, is_north, is_west;
            char n_s, w_e;
            if(!skipCommas(2)) break;
            if(!waitForNextCharAvailable()) break;
            available = getNextChar() == 'A';
            if(!available) break;
            
            if(!skipCommas(1)) break;
            loc.latitude = getLatitudeOrLongitude();
            
            if(!waitForNextCharAvailable()) break;
            n_s = getNextChar();
            if (n_s == 'N') loc.n_s = NORTH;
            else if (n_s = 'S') loc.n_s = SOUTH;
            else break;
            
            if(!skipCommas(1)) break;
            loc.longitude = getLatitudeOrLongitude();
            
            if(!waitForNextCharAvailable()) break;
            w_e = getNextChar();
            if (w_e == 'W') loc.w_e = WEST;
            else if (w_e = 'E') loc.w_e = EAST;
            else break;
            
            break;
        }
    }
    return loc;
}
