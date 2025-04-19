#ifndef __BITS_H
#define __BITS_H

#include <stdint.h>

#define BITSET(value, bit) (value |= (1 << bit))
#define BITCLEAR(value, bit) (value &= ~(1 << bit))
#define BITFLIP(value, bit) (value ^= (1 << bit))
#define BITCHECK(value, bit) ((value >> bit) & 1)

#endif