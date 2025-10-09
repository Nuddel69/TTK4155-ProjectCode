/*
 * fonts.h
 *
 * Library of fonts
 * Large: 8x8, normal: 5x7 and small: 4x7
 * Taken from Course resources on blackboard
 */
#ifndef FONTS_H_
#define FONTS_H_

#include <avr/pgmspace.h>

extern const unsigned char PROGMEM font8[95][8];
extern const unsigned char PROGMEM font5[95][5];
extern const unsigned char PROGMEM font4[95][4];

#endif /* FONTS_H_ */