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

#include <string.h>
#include <ctype.h>

#define RING_BUFFER_SIZE 16

char        g_RingBuffer[RING_BUFFER_SIZE];
uint8_t     g_RingBufferHead = 0;

char        g_CmdBuffer[RING_BUFFER_SIZE];

void LED_setState(uint8_t ledIdx, bool state)
{
    uint32_t gpio   = ledIdx == 0 ? myBoardLED0_GPIO : myBoardLED1_GPIO;
    uint32_t value  = state ? 0 : 1;
    
    GPIO_writePin(gpio, value);
}

char get_character_at_position(int8_t position)
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

char get_character(uint8_t offset)
{
    return get_character_at_position(g_RingBufferHead - offset);
}

void write_character(char character)
{
    uint8_t position_buffer = g_RingBufferHead % RING_BUFFER_SIZE;

    g_RingBuffer[position_buffer] = character;

    g_RingBufferHead++;
}

void copy_cmd_buffer()
{
    uint8_t i = 0;
    for(i = 0; i < RING_BUFFER_SIZE; i++)
    {
        g_CmdBuffer[i] = get_character_at_position(g_RingBufferHead - RING_BUFFER_SIZE + i);
    }
}

void clear_buffers() 
{
    uint8_t i = 0;
    for(i = 0; i < RING_BUFFER_SIZE; i++)
    {
        g_RingBuffer[i] = 1;
        g_CmdBuffer[i] = 1;
    }
}

void parse_command()
{
    // LED n ON/OFF
    char* cmd_ptr = strstr(g_CmdBuffer, "led");
    uint8_t led_idx = 0;
    bool led_action = false;

    if (cmd_ptr)
    {
        cmd_ptr += 3;

        if (*cmd_ptr != ' ')
        {
            
            return;
        }
        
        cmd_ptr++;

        if (isdigit(*cmd_ptr))
        {
            uint8_t idx = (*cmd_ptr) - '0';

            if (idx > 1)
            {
                return;
            }

            led_idx = idx;
        }

        cmd_ptr++;

        if (*cmd_ptr != ' ')
        {
            
            return;
        }

        cmd_ptr++;

        char action_buffer[3];
        strncpy(action_buffer, cmd_ptr, 3);
        if (strstr(action_buffer, "on"))
        {
            led_action = true;      
        }
        else if(strstr(action_buffer, "off"))
        {
            led_action = false;            
        }
        else
        {
            return;
        }

        LED_setState(led_idx, led_action);
    }
}

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

    clear_buffers();

    while(1)
    {
        // GPIO_writePin(myBoardLED0_GPIO, 0);
        // DEVICE_DELAY_US(1e6);
        // GPIO_writePin(myBoardLED0_GPIO, 1);
        // GPIO_togglePin(myBoardLED1_GPIO);
        // DEVICE_DELAY_US(1e6);

        char value = (char)SCI_readCharBlockingFIFO(mySCI0_BASE);

        if (value == 8)
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
        else if (value == 13)
        {
            write_character('\0');
            
            copy_cmd_buffer();

            parse_command();

            clear_buffers();
        }
        else 
        {
            write_character(tolower(value));
        }

    }
}
//
// End of File
//
