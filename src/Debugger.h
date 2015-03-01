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

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

/******************************************************/
/* Include                                            */
/******************************************************/


/******************************************************/
/* Macro                                              */
/******************************************************/

/* Assert */
#define DEBUGGER_ASSERT(cond)

/* Panic */
#define DEBUGGER_PANIC(cond)

/* Level definition */
#define DEBUGGER_LEVEL_ERROR    0   /**< Abnormal event that make the program stop */
#define DEBUGGER_LEVEL_WARNING  1   /**< Abnomral event but program can go on */
#define DEBUGGER_LEVEL_INFO     2   /**< Normal event about program behavior */
#define DEBUGGER_LEVEL_TRACE    3   /**< Normal event about program behavior */

/* Set Default Level filter */
#ifndef DEBUGGER_LEVEL
#  define DEBUGGER_LEVEL          DEBUGGER_LEVEL_INFO
#endif

/* Filter Error level */
#if DEBUGGER_LEVEL < DEBUGGER_LEVEL_ERROR
#  define DEBUGGER_ERROR(...)
#else
#  define DEBUGGER_ERROR(...)     do{ Debugger_Log(__VA_ARGS__); } while(0)
#endif

/* Filter Warning level */
#if DEBUGGER_LEVEL < DEBUGGER_LEVEL_WARNING
#  define DEBUGGER_WARNING(...)
#else
#  define DEBUGGER_WARNING(...)   do{ Debugger_Log(__VA_ARGS__); } while(0)
#endif

/* Filter Info level */
#if DEBUGGER_LEVEL < DEBUGGER_LEVEL_INFO
#  define DEBUGGER_INFO(...)
#else
#  define DEBUGGER_INFO(...)      do{ Debugger_Log(__VA_ARGS__); } while(0)
#endif

/* Filter Trace level */
#if DEBUGGER_LEVEL < DEBUGGER_LEVEL_TRACE
#  define DEBUGGER_TRACE(...)
#else
#  define DEBUGGER_TRACE(...)     do{ Debugger_Log(__VA_ARGS__); } while(0)
#endif


/******************************************************/
/* Type                                               */
/******************************************************/


/******************************************************/
/* Prototype                                          */
/******************************************************/

/**
 * Run Debugging shell
 */
extern void Debugger_RunShell(int argc, char const *argv[]);

/**
 * Event logger
 * @param fmt String to print 
 */
extern void Debugger_Log(char const *fmt, ...);

/**
 * Notify PC address for breakpoint
 * @param addr The PC address
 */
extern void Debugger_NotifyPcAddress(uint16_t addr);


/******************************************************/
/* Variable                                           */
/******************************************************/


/******************************************************/
/* Function                                           */
/******************************************************/


#endif /* _DEBUGGER_H_ */

