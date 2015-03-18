/**
 * GameBoyPlay - Simple Gameboy emulator written in C.
 * Copyright (C) 2015 - Aurelien Tran <aurelien.tran@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */


/******************************************************/
/* Include                                            */
/******************************************************/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <Memory.h>
#include <Debugger.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

/** 16 bit addressable memory table */
#define MEMORY_TABLE_SIZE   0x00010000


/******************************************************/
/* Type                                               */
/******************************************************/


/******************************************************/
/* Prototype                                          */
/******************************************************/


/******************************************************/
/* Variable                                           */
/******************************************************/

/** 16 bit memory table */
static uint8_t Memory_Table[MEMORY_TABLE_SIZE];


/******************************************************/
/* Function                                           */
/******************************************************/

/**
 * Initialize Memory
 */
void Memory_Initialize(void)
{
    for(int i=0; i<MEMORY_TABLE_SIZE; i++)
    {
        Memory_Table[i] = 0;
    }
}


void Memory_LoadFile(char const * file, uint16_t addr)
{
    /* Open file */
    FILE *pFile = fopen(file, "r");
    if(pFile == NULL)
    {
        DEBUGGER_ERROR("LoadFile Error: %s\n", strerror(errno));
        return;
    }

    /* Fill file data to the memory at the specified address */
    for(int i=addr; i<MEMORY_TABLE_SIZE; i++)
    {
        int c = fgetc(pFile);

        /* Handle end of file */
        if(c == EOF)
        {
            break;
        }

        Memory_Table[i] = c;
    }

    fclose(pFile);
}


void Memory_Write(uint16_t addr, uint8_t data)
{
    DEBUGGER_TRACE("Write 0x%04X: 0x%02X\n", addr, data);
    Memory_Table[addr] = data;

    /* Notify memory change for watchpoint */
    Debugger_NotifyMemoryWrite(addr, data);
}


uint8_t Memory_Read(uint16_t addr)
{
    DEBUGGER_TRACE("Read 0x%04X: 0x%02X\n", addr, Memory_Table[addr]);
    return Memory_Table[addr];
}


