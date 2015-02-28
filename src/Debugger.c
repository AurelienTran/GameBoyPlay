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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <Memory.h>
#include <Z80.h>
#include <Log.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

/** Get the size of an array */
#define ARRAY_SIZE(x)               (sizeof(x) / sizeof(x[0]))

/** Buffer size for user input */
#define DEBUGGER_BUFFER_SIZE        256

/** Max number of breakpoint */
#define DEBUGGER_BREAKPOINT_COUNT   16

/** Max number of watchpoint */
#define DEBUGGER_WATCHPOINT_COUNT   16


/******************************************************/
/* Type                                               */
/******************************************************/

/**
 * Callback type to Execute a Debugger command
 * @param arg command argument string
 */
typedef void (*Debugger_Callback_t)(char *arg);

/**
 * Debugger command type
 */
typedef struct tagDebugger_Command_t
{
    char const * Name;              /**< Name */
    char const * Argument;          /**< Argument format */ 
    char const * Help;              /**< Help string */
    Debugger_Callback_t Callback;   /**< Callback */
} Debugger_Command_t;

/**
 * Debugger state type
 */
typedef enum tagDebugger_State_e
{
    DEBUGGER_STATE_RUN,     /**< Run wihtout interruption */
    DEBUGGER_STATE_BREAK,   /**< Pause the program for debug */
    DEBUGGER_STATE_EXIT     /**< Exit the debugger */
} Debugger_State_e;

/**
 * Debugger info type
 */
typedef struct tagDebugger_Info_t
{
    Debugger_State_e State;
    int BreakpointCount;
    int WatchpointCount;
    uint16_t BreakpointList[DEBUGGER_BREAKPOINT_COUNT];
    uint16_t WatchpointList[DEBUGGER_WATCHPOINT_COUNT];
} Debugger_Info_t;


/******************************************************/
/* Prototype                                          */
/******************************************************/

/**
 * Run Debugging shell
 */
void Debugger_RunShell(void);

/* Command callback */
static void Debugger_CommandRun(char *arg);
static void Debugger_CommandStep(char *arg);
static void Debugger_CommandReset(char *arg);
static void Debugger_CommandBreak(char *arg);
static void Debugger_CommandWatch(char *arg);
static void Debugger_CommandClear(char *arg);
static void Debugger_CommandMem(char *arg);
static void Debugger_CommandCpu(char *arg);
static void Debugger_CommandExit(char *arg);
static void Debugger_CommandHelp(char *arg);


/******************************************************/
/* Variable                                           */
/******************************************************/

/**
 * Debugger Info
 */
static Debugger_Info_t Debugger_Info;

/**
 * Debugger Command
 */
Debugger_Command_t const Debugger_Command[] =
{
    /* Program Execution */
    {"run", "",                 "Run the program to be debugged.",      Debugger_CommandRun},
    {"step", "[step #]",        "Go to next instruction.",              Debugger_CommandStep},
    {"continue", "",            "Continue normal execution.",           Debugger_CommandRun},
    {"reset", "",               "Reset the program.",                   Debugger_CommandReset},

    /* Breakpoint and Watchpoint */
    {"break", "<addr>",         "Set a new breakpoint.",                Debugger_CommandBreak},
    {"watch", "<addr>",         "Set a new watchpoint.",                Debugger_CommandWatch},
    {"clear", "",               "Remove all breakpoint.",               Debugger_CommandClear},

    /* Memory display */
    {"mem", "<addr> [size]",    "Print memory area. (default: size=1)", Debugger_CommandMem},
    {"cpu", "",                 "Print CPU register.",                  Debugger_CommandCpu},

    /* Misc */
    {"exit", "",                "Close the application.",               Debugger_CommandExit},
    {"help", "",                "Print this help.",                     Debugger_CommandHelp}
};


/******************************************************/
/* Function                                           */
/******************************************************/

void Debugger_RunShell(void)
{
    char buffer[DEBUGGER_BUFFER_SIZE];

    printf("print 'help' to list all availlable command.\n");

    /* Print prompt */
    while(Debugger_Info.State != DEBUGGER_STATE_EXIT)
    {
        printf("dbg> ");
    }
}


/******************************************************/
/* Command function                                   */
/******************************************************/

/**
 * Run the program until breakpoint/watchpoint
 */
static void Debugger_CommandStep(char *arg)
{
    /* @todo get the number of step to execute */
    (void) arg;
    int step = 1;

    for(int i=0; i<step; i++)
    {
        Z80_Step();

        if(Debugger_Info.State == DEBUGGER_STATE_BREAK)
        {
            break;
        }
    }
}

/**
 * Run the program until breakpoint/watchpoint
 */
static void Debugger_CommandRun(char *arg)
{
    /* Unused param */
    (void) arg;

    while(Debugger_Info.State == DEBUGGER_STATE_RUN)
    {
        Z80_Step();
    }
}

/**
 * Reset the program
 */
static void Debugger_CommandReset(char *arg)
{
    /* Unused param */
    (void) arg;

    /* @todo find a better way to do it */
    Memory_Initialize();
    Z80_Initialize();
    Memory_LoadFile("rom/bootstrap.bin", 0);
}


/**
 * Set break point
 */
static void Debugger_CommandBreak(char *arg)
{
    /* @todo get the breakpoint addresss */
    (void) arg;
    uint16_t addr = 0;

    /* Register breakpoint */
    if(Debugger_Info.BreakpointCount == DEBUGGER_BREAKPOINT_COUNT)
    {
        printf("Too many breakpoint set.\n");
    }
    Debugger_Info.BreakpointList[Debugger_Info.BreakpointCount] = addr;
    Debugger_Info.BreakpointCount ++;

    /* Print breakpoint list */
    printf("Breakpoint:\n");
    for(int i=0; i<Debugger_Info.BreakpointCount; i++)
    {
        printf(" - %d: 0x%x\n", i, Debugger_Info.BreakpointList[i]);
    }
}


/**
 * Set watch point
 */
static void Debugger_CommandWatch(char *arg)
{
    /* @todo get the watchpoint addresss */
    (void) arg;
    uint16_t addr = 0;

    /* Register watchpoint */
    if(Debugger_Info.WatchpointCount == DEBUGGER_WATCHPOINT_COUNT)
    {
        printf("Too many watchpoint set.\n");
    }
    Debugger_Info.WatchpointList[Debugger_Info.WatchpointCount] = addr;
    Debugger_Info.WatchpointCount ++;

    /* Print watchpoint list */
    printf("Watchpoint:\n");
    for(int i=0; i<Debugger_Info.WatchpointCount; i++)
    {
        printf(" - %d: 0x%x\n", i, Debugger_Info.WatchpointList[i]);
    }
}


/**
 * Clear all watch/break point
 */
static void Debugger_CommandClear(char *arg)
{
    /* Unused parameter */
    (void) arg;

    Debugger_Info.WatchpointCount = 0;
    Debugger_Info.BreakpointCount = 0;

    /* Print watchpoint list */
    printf("All watchpoint and breakpoint removed.\n");
}


/**
 * Print Memory
 */
static void Debugger_CommandMem(char *arg)
{
    /* @todo get the memory addresss and size */
    (void) arg;
    uint16_t addr = 0;
    int size = 1;

    /* Print Memory area */
    printf("Memory:\n");
    for(int i=0; i<size; i++)
    {
        printf(" - 0x%04x: 0x%02x\n", addr + i, Memory_Read(addr + i));
    }
}

/**
 * Print CPU
 */
static void Debugger_CommandCpu(char *arg)
{
    /* Unused parameter */
    (void) arg;

    /* Print CPU */
    Z80_Print();
}

/**
 * Print help
 */
static void Debugger_CommandHelp(char *arg)
{
    /* Unused parameter */
    (void) arg;

    /* Print help */
    printf("Help:\n");
    for(int i=0; i<(int)ARRAY_SIZE(Debugger_Command); i++)
    {
        Debugger_Command_t const * pCmd = &Debugger_Command[i];
        printf("%10s %10s: %s\n", pCmd->Name, pCmd->Argument, pCmd->Help);
    }
}

/**
 * Exit the debugger
 */
static void Debugger_CommandExit(char *arg)
{
    /* Unused parameter */
    (void) arg;

    /* Make the program exit */
    Debugger_Info.State = DEBUGGER_STATE_EXIT;
}

