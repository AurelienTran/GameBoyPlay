/**
 * PlayEmulator - Simple Gameboy emulator written in C.
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

#ifndef _MEMORY_H_
#define _MEMORY_H_


/******************************************************/
/* Include                                            */
/******************************************************/

#include <stdint.h>


/******************************************************/
/* Macro                                              */
/******************************************************/


/******************************************************/
/* Type                                               */
/******************************************************/


/******************************************************/
/* Prototype                                          */
/******************************************************/

/**
 * Initialize Memory
 */
extern void Memory_Initialize(void);

/**
 * Load binary file
 * @param file The binary file name
 */
extern void Memory_LoadFile(char const * file, uint16_t addr);

/**
 * Write Memory Address
 * @param addr The address to write
 * @param data The data to write to addr
 */
extern void Memory_Write(uint16_t addr, uint8_t data);

/**
 * Read Memory Address
 * @param addr The address to read
 */
extern uint8_t Memory_Read(uint16_t addr);


/******************************************************/
/* Variable                                           */
/******************************************************/


/******************************************************/
/* Function                                           */
/******************************************************/


#endif /* _MEMORY_H_ */

