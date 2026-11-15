#ifndef LED_HAL_H
#define LED_HAL_H

#include <stdbool.h>
#include <stdint.h>

void HAL_setLEDState(uint16_t ledIdx, bool state);

#endif