#include <stdint.h>

#ifndef __NIXIE_CLOCK_DEF_H__
#define __NIXIE_CLOCK_DEF_H__

#define ZERO 0
#define TWO 2
#define THREE 3
#define FOUR 4
#define EIGHT 8
#define TEN 10
#define SIXTEEN 16

#define CHAR_ZERO 48
#define CHAR_NINE 57

// may need default value
struct TimeAndDate{
  uint8_t hour = UINT8_MAX;
  uint8_t minute = UINT8_MAX;
  uint8_t second = UINT8_MAX;
  uint8_t day = UINT8_MAX;
  uint8_t month = UINT8_MAX;
  uint8_t year = UINT8_MAX;
};

enum Hemisphere{
    UNKNOWN_HEMISPHERE,
    EAST,
    SOUTH,
    WEST,
    NORTH,
};

// may need default value
struct Location{
    enum Hemisphere n_s = UNKNOWN_HEMISPHERE;
    enum Hemisphere w_e = UNKNOWN_HEMISPHERE;
    float latitude = -1;
    float longitude = -1;
};

#endif
