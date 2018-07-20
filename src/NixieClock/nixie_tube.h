// Author: Lok

#include "def.h"

#ifndef _nixie_tube_H_
#define _nixie_tube_H_

#define LEFT_COMMA ','
#define RIGHT_COMMA '.'
#define EMPTY ' '

#define PROPOGATIONAL_DELAY 10

class NixieTubeComplexOfEight{
    private:
        uint8_t EN_pin;
        uint8_t tube_pins[THREE];
        uint8_t display_pins[FOUR];
        
        uint8_t left_comma;
        uint8_t right_comma;
        uint8_t digits[TEN];
        uint8_t off;
        
        char last_display[EIGHT];
        
        void new_common_instance(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN]);
        uint8_t find_off_index();
        
    public:
        NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN], uint8_t left_comma_index, uint8_t right_comma_index);
        NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN], uint8_t comma_index);
        NixieTubeComplexOfEight(uint8_t EN, uint8_t tubes[THREE], uint8_t display[FOUR], uint8_t digit_indices[TEN]);
        ~NixieTubeComplexOfEight();
        
        //Kernel methods
        char* get_display();
        void tube_display(uint8_t number, uint8_t tube);
        void tube_display(char content, uint8_t tube);
        void tube_off(uint8_t tube);
        
        void all_display(char contents[EIGHT]);
        void all_display(char content);
        void all_off();
        
        void time_display(TimeAndDate time);
        void date_display(TimeAndDate date);
        void location_display(Location loc);
        void temperature_display(float temp);
        
        uint8_t get_left_comma();
        uint8_t get_right_comma();
};

#endif
