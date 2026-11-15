#include "led_hal.h"
#include "board.h"

void HAL_setLEDState(uint16_t ledIdx, bool state)
{
    uint32_t gpio   = ledIdx == 0 ? myBoardLED0_GPIO : myBoardLED1_GPIO;
    uint32_t value  = state ? 0 : 1;
    
    GPIO_writePin(gpio, value);
}