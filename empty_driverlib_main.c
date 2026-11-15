//#############################################################################
//
// FILE:   empty_driverlib_main.c
//
//! \addtogroup driver_example_list
//! <h1>Empty Project Example</h1> 
//!
//! This example is an empty project setup for Driverlib development.
//!
//
//#############################################################################
//
//
// 
// C2000Ware v26.02.00.00
//
// Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"

#include "led_hal.h"

#include <string.h>
#include <ctype.h>

#define RING_BUFFER_SIZE 16

char        g_RingBuffer[RING_BUFFER_SIZE];
uint8_t     g_RingBufferHead = 0;

bool        g_LedBlinkState[2];

typedef enum
{
    LED_OFF,
    LED_ON,
    LED_BLINK
} LEDState;

void LED_setState(uint16_t ledIdx, LEDState state);

void APP_parseCommand(char* commandBuffer);

char CONSOLE_getCharacterAt(int8_t position);
void CONSOLE_writeCharacter(char character);
void CONSOLE_eraseLastCharacter();
void CONSOLE_copyCommand(char* cmdBuffer);
void CONSOLE_clearBuffer();

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();
    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();
    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();
    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();
    //
    // PinMux and Peripheral Initialization
    //
    Board_init();
    //
    // C2000Ware Library initialization
    //
    C2000Ware_libraries_init();
    //
    // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
    //
    EINT;
    ERTM;

    g_LedBlinkState[0] = false;
    g_LedBlinkState[1] = false;

    CONSOLE_clearBuffer();

    while(1)
    {
        char value = (char)SCI_readCharBlockingFIFO(mySCI0_BASE);

        if (value == 8)
        {
            CONSOLE_eraseLastCharacter();
        }
        else if (value == 13)
        {
            CONSOLE_writeCharacter('\0');
            
            char cmdBuffer[RING_BUFFER_SIZE];
            CONSOLE_copyCommand(cmdBuffer);

            APP_parseCommand(cmdBuffer);

            CONSOLE_clearBuffer();
        }
        else 
        {
            CONSOLE_writeCharacter(tolower(value));
        }

    }
}

void APP_parseCommand(char* commandBuffer)
{
    // Command structure: "led 0/1 on/off/blink"
    
    char* cmdPtr = strstr(commandBuffer, "led");
    uint8_t ledIdx = 0;
    LEDState ledAction = LED_OFF;

    if (cmdPtr)
    {
        cmdPtr += 3;

        if (*cmdPtr != ' ')
        {
            return;
        }
        
        cmdPtr++;

        if (isdigit(*cmdPtr))
        {
            uint8_t idx = (*cmdPtr) - '0';

            if (idx > 1)
            {
                return;
            }

            ledIdx = idx;
        }

        cmdPtr++;

        if (*cmdPtr != ' ')
        {
            
            return;
        }

        cmdPtr++;

        char actionBuffer[5];
        strncpy(actionBuffer, cmdPtr, 5);
        if (strstr(actionBuffer, "on"))
        {
            ledAction = LED_ON;      
        }
        else if(strstr(actionBuffer, "off"))
        {
            ledAction = LED_OFF;            
        }
        else if (strstr(actionBuffer, "blink"))
        {
            ledAction = LED_BLINK;
        }
        else
        {
            return;
        }

        LED_setState(ledIdx, ledAction);
    }
}

char CONSOLE_getCharacterAt(int8_t position)
{
    int8_t  position_sign   = position >= 0 ? 1 : -1;
    uint8_t position_mod    = position_sign > 0 ? position : -position;
    uint8_t position_buffer = position_mod % RING_BUFFER_SIZE;

    if (position_sign > 0)
    {
        return g_RingBuffer[position_buffer];
    }
    else 
    {
        return g_RingBuffer[RING_BUFFER_SIZE - position_buffer];
    }
}

void CONSOLE_writeCharacter(char character)
{
    uint8_t position_buffer = g_RingBufferHead % RING_BUFFER_SIZE;

    g_RingBuffer[position_buffer] = character;

    g_RingBufferHead++;
}

void CONSOLE_eraseLastCharacter()
{
    if (g_RingBufferHead > 0)
    {
        g_RingBufferHead--;
    }
    else
    {
        g_RingBufferHead = RING_BUFFER_SIZE - 1;
    }
}

void CONSOLE_copyCommand(char* cmdBuffer)
{
    uint8_t i = 0;
    for(i = 0; i < RING_BUFFER_SIZE; i++)
    {
        cmdBuffer[i] = CONSOLE_getCharacterAt(g_RingBufferHead - RING_BUFFER_SIZE + i);
    }
}

void CONSOLE_clearBuffer() 
{
    uint8_t i = 0;
    for(i = 0; i < RING_BUFFER_SIZE; i++)
    {
        g_RingBuffer[i] = 1;
    }
}

__interrupt void INT_LED_BLINK_TIMER_ISR(void)
{
    if (g_LedBlinkState[0])
    {
        GPIO_togglePin(myBoardLED0_GPIO);
    }

    if (g_LedBlinkState[1])
    {
        GPIO_togglePin(myBoardLED1_GPIO);
    }

    Interrupt_clearACKGroup(INT_LED_BLINK_TIMER_INTERRUPT_ACK_GROUP);
}

void LED_setState(uint16_t ledIdx, LEDState state)
{
    switch (state) 
    {
    case LED_OFF:
    case LED_ON:
    {
        uint32_t gpio   = ledIdx == 0 ? myBoardLED0_GPIO : myBoardLED1_GPIO;
        uint32_t value  = state == LED_ON ? 0 : 1;
        
        g_LedBlinkState[ledIdx] = false;

        GPIO_writePin(gpio, value);
        
        return;
    }
    case LED_BLINK:
    {        
        g_LedBlinkState[ledIdx] = true;

        return;
    }
    }
}
//
// End of File
//
