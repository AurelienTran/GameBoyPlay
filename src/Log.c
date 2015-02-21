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


/******************************************************/
/* Include                                            */
/******************************************************/

#include <stdarg.h>
#include <stdio.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

#define LOG_ERROR(...)  Log_Printf("E", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)   Log_Printf("I", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)  Log_Printf("D", __FILE__, __LINE__, __VA_ARGS__)

/******************************************************/
/* Type                                               */
/******************************************************/


/******************************************************/
/* Prototype                                          */
/******************************************************/


/******************************************************/
/* Variable                                           */
/******************************************************/


/******************************************************/
/* Function                                           */
/******************************************************/

void Log_Printf(char *lvl, char *file, int line, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, "[%s][%s:%d] ", lvl, file, line);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

