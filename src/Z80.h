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

#ifndef _Z80_H_
#define _Z80_H_


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
 * Initialize Z80
 */
extern void Z80_Initialize(void);

/**
 * Initialize Z80
 * @return The number of cycle used for the instruction
 */
extern int Z80_Run(void);

/**
 * Print Z80 internal state for debug purpose
 */
extern void Z80_Print(void);


/******************************************************/
/* Variable                                           */
/******************************************************/


/******************************************************/
/* Function                                           */
/******************************************************/


#endif /* _Z80_H_ */

