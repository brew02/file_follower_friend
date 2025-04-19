#ifndef __TIME_H
#define __TIME_H

#include <stdint.h>

void delayMS(uint32_t val);
void resetTIM1Count();
void initTimers();

#endif