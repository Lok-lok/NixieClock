// Author: Lok

#include "nixie_tube.h"
#include <Arduino.h>

void NixieTubeComplexOfEight::new_common_instance(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN]){
    int i;
    
    EN_pin = EN;
    pinMode(EN, OUTPUT);
    digitalWrite(EN, LOW);
    for (i = 0; i < THREE; i++){
        tube_pins[i] = tubes[i];
        pinMode(tube_pins[i], OUTPUT);
    }
    for (i = 0; i < FOUR; i++){
        display_pins[i] = display[i];
        pinMode(display_pins[i], OUTPUT);
    }
    
    left_comma = UINT8_MAX;
    right_comma = UINT8_MAX;
    for (i = 0; i < TEN; i++)
        digits[i] = digit_indices[i];
    
    for (i = 0; i < EIGHT; i++)
      last_display[i] = UINT8_MAX; // which means unknown.
}

uint8_t NixieTubeComplexOfEight::find_off_index(){
    int i;
    uint8_t index = UINT8_MAX;
    bool used[SIXTEEN];
    for (i = 0; i < SIXTEEN; i++) used[i] = false;
    
    if (left_comma < SIXTEEN) used[left_comma] = true;
    if (right_comma < SIXTEEN) used[right_comma] = true;
    for (i = 0; i < TEN; i++) used[digits[i]] = true;
    
    for (i = 15; i >= ZERO && index >= SIXTEEN; i--) 
        if (!used[i]) index = i;
    return index;
}

NixieTubeComplexOfEight::NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN], uint8_t left_comma_index, uint8_t right_comma_index){
    int i;
    
    new_common_instance(EN, tubes, display, digit_indices);
    
    left_comma = left_comma_index;
    right_comma = right_comma_index;

    off = find_off_index();
}

NixieTubeComplexOfEight::NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN], uint8_t comma_index){
    int i;
    
    new_common_instance(EN, tubes, display, digit_indices);
    
    left_comma = comma_index;

    off = find_off_index();
}

NixieTubeComplexOfEight::NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN]){
    int i;

    new_common_instance(EN, tubes, display, digit_indices);

    off = find_off_index();
}

NixieTubeComplexOfEight::~NixieTubeComplexOfEight(){
    int i;
    all_off();
    digitalWrite(EN_pin, LOW);
    for (i = 0; i < THREE; i++){
        digitalWrite(tube_pins[i], LOW);
    }
    for (i = 0; i < FOUR; i++){
        digitalWrite(display_pins[i], LOW);
    }
}

char* NixieTubeComplexOfEight::get_display(){
    return last_display;
}

void NixieTubeComplexOfEight::tube_display(uint8_t number, uint8_t tube){
    int i;
    uint8_t display = digits[number % 10];
    for (i = 0; i < FOUR; i++)
        digitalWrite(display_pins[i], (display >> i) % TWO);
    for (i = 0; i < THREE; i++)
        digitalWrite(tube_pins[i], (tube >> i) % TWO);
    delayMicroseconds(PROPOGATIONAL_DELAY);
    digitalWrite(EN_pin, HIGH);
    delayMicroseconds(PROPOGATIONAL_DELAY);
    digitalWrite(EN_pin, LOW);
    last_display[tube] = (char)(number % 10 + CHAR_ZERO);
}

void NixieTubeComplexOfEight::tube_display(char content, uint8_t tube){
    if (last_display[tube] != content){
        int i;
        uint8_t display = UINT8_MAX;
        if (CHAR_ZERO <= content && content <= CHAR_NINE) display = digits[content - CHAR_ZERO];
        else if (content == LEFT_COMMA && left_comma < SIXTEEN) display = left_comma;
        else if (content == RIGHT_COMMA && right_comma < SIXTEEN) display = right_comma;
        if (display >= SIXTEEN) display = off;
        for (i = 0; i < FOUR; i++)
            digitalWrite(display_pins[i], (display >> i) % TWO);
        for (i = 0; i < THREE; i++)
            digitalWrite(tube_pins[i], (tube >> i) % TWO);
        delayMicroseconds(PROPOGATIONAL_DELAY);
        digitalWrite(EN_pin, HIGH);
        delayMicroseconds(PROPOGATIONAL_DELAY);
        digitalWrite(EN_pin, LOW);
        last_display[tube] = content;
    }
}

void NixieTubeComplexOfEight::tube_off(uint8_t tube){
    int i;
    for (i = 0; i < FOUR; i++)
        digitalWrite(display_pins[i], (off >> i) % TWO);
    for (i = 0; i < THREE; i++)
        digitalWrite(tube_pins[i], (tube >> i) % TWO);
    delayMicroseconds(PROPOGATIONAL_DELAY);
    digitalWrite(EN_pin, HIGH);
    delayMicroseconds(PROPOGATIONAL_DELAY);
    digitalWrite(EN_pin, LOW);
    last_display[tube] = EMPTY;
}

void NixieTubeComplexOfEight::all_display(char contents[EIGHT]){
    int i;
    for (i = 0; i < EIGHT; i++)
        tube_display(contents[i], 7 - i); // reverse indices
}

void NixieTubeComplexOfEight::all_off(){
    int i;
    for (i = 0; i < EIGHT; i++){
        tube_off(i);
    }
}

void NixieTubeComplexOfEight::time_display(TimeAndDate time){
    char contents[EIGHT] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    if (time.hour > 24 || time.minute > 60 || time.second > 60){
        if (right_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = RIGHT_COMMA;
        }
        else if (left_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = LEFT_COMMA;
        }
    } else {
        contents[0] = (char)(CHAR_ZERO + (time.hour / TEN) % TEN);
        contents[1] = (char)(CHAR_ZERO + time.hour % TEN);
        contents[3] = (char)(CHAR_ZERO + (time.minute / TEN) % TEN);
        contents[4] = (char)(CHAR_ZERO + time.minute % TEN);
        contents[6] = (char)(CHAR_ZERO + (time.second / TEN) % TEN);
        contents[7] = (char)(CHAR_ZERO + time.second % TEN);
        if (time.second % TWO == ZERO && left_comma < SIXTEEN){
            contents[2] = LEFT_COMMA;
            contents[5] = LEFT_COMMA;
        } else if (time.second % TWO != ZERO && right_comma < SIXTEEN){
            contents[2] = RIGHT_COMMA;
            contents[5] = RIGHT_COMMA;
        }
    }
    all_display(contents);
}

void NixieTubeComplexOfEight::date_display(TimeAndDate date){
    char contents[EIGHT] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    if (date.day > 31 || date.month > 12 || date.year > 99){
        if (right_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = RIGHT_COMMA;
        } else if (left_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = LEFT_COMMA;
        }
    } else {
        contents[0] = (char)(CHAR_ZERO + (date.day / TEN) % TEN);
        contents[1] = (char)(CHAR_ZERO + date.day % TEN);
        contents[3] = (char)(CHAR_ZERO + (date.month / TEN) % TEN);
        contents[4] = (char)(CHAR_ZERO + date.month % TEN);
        contents[6] = (char)(CHAR_ZERO + (date.year / TEN) % TEN);
        contents[7] = (char)(CHAR_ZERO + date.year % TEN);
        
        if (right_comma < SIXTEEN){
            contents[2] = RIGHT_COMMA;
            contents[5] = RIGHT_COMMA;
        } else if (left_comma < SIXTEEN){
            contents[2] = LEFT_COMMA;
            contents[5] = LEFT_COMMA;
        } 
    }
    all_display(contents);
}

void NixieTubeComplexOfEight::location_display(Location loc){
    char contents[EIGHT] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    if (loc.latitude < ZERO || loc.longitude < ZERO || loc.n_s == UNKNOWN_HEMISPHERE || loc.w_e == UNKNOWN_HEMISPHERE){
        if (right_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = RIGHT_COMMA;
        } else if (left_comma < SIXTEEN){
            int i;
            for (i = 0; i < EIGHT; i++) contents[i] = LEFT_COMMA;
        }
    } else {
        contents[0] = (char)(CHAR_ZERO + ((uint8_t)loc.latitude / TEN) % TEN);
        contents[1] = (char)(CHAR_ZERO + (uint8_t)loc.latitude % TEN);
        contents[4] = (char)(CHAR_ZERO + ((uint8_t)loc.longitude / TEN / TEN) % TEN);
        contents[5] = (char)(CHAR_ZERO + ((uint8_t)loc.longitude / TEN) % TEN);
        contents[6] = (char)(CHAR_ZERO + (uint8_t)loc.longitude % TEN);
        if (loc.n_s == NORTH && right_comma < SIXTEEN) contents[2] = RIGHT_COMMA;
        else if (loc.n_s == SOUTH && left_comma < SIXTEEN) contents[2] = LEFT_COMMA;
        if (loc.w_e == EAST && right_comma < SIXTEEN) contents[7] = RIGHT_COMMA;
        else if (loc.w_e == WEST && left_comma < SIXTEEN) contents[7] = LEFT_COMMA;
    }
    all_display(contents);
}

void NixieTubeComplexOfEight::temperature_display(float temp){
    int i, times = 10000;
    char contents[EIGHT] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    for (i = 0; i < FOUR; i++){
        contents[i] = (char)(CHAR_ZERO + ((int)temp / times) % TEN);
        times /= TEN;
    }
    contents[4] = (char)(CHAR_ZERO + (int)temp % TEN);
    for (i = 0; i < FOUR && contents[i] == CHAR_ZERO; i++)
        contents[i] = EMPTY; // delete the leading zero's.
    times = TEN;
    for (i = 6; i < EIGHT; i++){
        contents[i] = (char)(CHAR_ZERO + (int)(temp * times) % TEN);
        times *= TEN;
    }
    if (right_comma < SIXTEEN)
        contents[5] = RIGHT_COMMA;
    else if (left_comma < SIXTEEN)
        contents[5] = LEFT_COMMA;
    all_display(contents);
}

uint8_t NixieTubeComplexOfEight::get_left_comma(){
    return left_comma;
}
uint8_t NixieTubeComplexOfEight::get_right_comma(){
    return right_comma;
}
