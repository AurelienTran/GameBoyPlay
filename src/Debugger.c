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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Memory.h>
#include <Cpu.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

/** Get the size of an array */
#define ARRAY_SIZE(x)               (sizeof(x) / sizeof(x[0]))

/** Buffer size for user input */
#define DEBUGGER_BUFFER_SIZE        256

/** Max argument number */
#define DEBUGGER_ARG_COUNT          4

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
typedef void (*Debugger_Callback_t)(int argc, char const * argv[]);

/**
 * Debugger command type
 */
typedef struct tagDebugger_Command_t
{
    char const * Name;              /**< Name */
    char const * Shortcut;          /**< Shortcut */
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
    DEBUGGER_STATE_EXIT     /**< Quit the debugger */
} Debugger_State_e;

/**
 * Debugger info type
 */
typedef struct tagDebugger_Info_t
{
    Debugger_State_e State;                             /**< Debugger state */
    int BreakListCount;                                 /**< Breakpoint set count */
    int WatchListCount;                                 /**< Watchpoint set count */
    uint16_t BreakListAddr[DEBUGGER_BREAKPOINT_COUNT];  /**< Breakpoint list */
    uint16_t WatchListAddr[DEBUGGER_WATCHPOINT_COUNT];  /**< Watchpoint list */
    uint16_t WatchListSize[DEBUGGER_WATCHPOINT_COUNT];  /**< Watchpoint list */
} Debugger_Info_t;


/******************************************************/
/* Prototype                                          */
/******************************************************/

/* Command callback */
static void Debugger_CommandRun(int argc, char const * argv[]);
static void Debugger_CommandStep(int argc, char const * argv[]);
static void Debugger_CommandReset(int argc, char const * argv[]);
static void Debugger_CommandBreak(int argc, char const * argv[]);
static void Debugger_CommandWatch(int argc, char const * argv[]);
static void Debugger_CommandClear(int argc, char const * argv[]);
static void Debugger_CommandMem(int argc, char const * argv[]);
static void Debugger_CommandCpu(int argc, char const * argv[]);
static void Debugger_CommandQuit(int argc, char const * argv[]);
static void Debugger_CommandHelp(int argc, char const * argv[]);

/* Shell utility */
static char * Debugger_GetUserInput(char * buffer);

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
    {"run", "r", "",                 "Run the program to be debugged.",         Debugger_CommandRun},
    {"step", "s", "[step #]",        "Go to next instruction.",                 Debugger_CommandStep},
    {"continue", "c", "",            "Continue normal execution.",              Debugger_CommandRun},
    {"reset", "rst", "",             "Reset the program.",                      Debugger_CommandReset},

    /* Breakpoint and Watchpoint */
    {"break", "b", "<addr>",         "Set a new breakpoint.",                   Debugger_CommandBreak},
    {"watch", "w", "<addr> [size]",  "Set a new watchpoint. (default: size=1)", Debugger_CommandWatch},
    {"clear", "clr", "",             "Remove all breakpoint.",                  Debugger_CommandClear},

    /* Memory display */
    {"mem", "m", "<addr> [size]",    "Print memory area. (default: size=1)",    Debugger_CommandMem},
    {"cpu", "cpu", "",               "Print CPU register.",                     Debugger_CommandCpu},

    /* Misc */
    {"help", "h", "",                "Print this help.",                        Debugger_CommandHelp},
    {"quit", "q", "",                "Close the application.",                  Debugger_CommandQuit}
};


/******************************************************/
/* Function                                           */
/******************************************************/

/**
 * Get user string input
 * @param buffer String of DEBUGGER_BUFFER_SIZE length
 * @return buffer if successful, otherwise NULL
 */
static char * Debugger_GetUserInput(char * buffer)
{
    for(;;)
    {
        /* Print prompt */
        printf("dbg> ");

        /* Get user input */
        if(fgets(buffer, DEBUGGER_BUFFER_SIZE, stdin) == NULL)
        {
            /* An error occur or EOF */
            return NULL;
        }

        /* Handle case where the input is too long */
        if(buffer[strlen(buffer) - 1] != '\n')
        {
            /* Get the rest of the input until EOL */
            int ch = getc(stdin);
            while((ch != '\n') && (ch != EOF))
            {
                ch = getc(stdin);
            }

            printf("Input too long.\n");
            continue;
        }

        /* Remove newline at the end of the line */
        buffer[strlen(buffer)-1] = '\0';

        return buffer;
    }
}


void Debugger_RunShell(int argc, char const *argv[])
{
    char buffer_prev[DEBUGGER_BUFFER_SIZE] = "help";
    char buffer_curr[DEBUGGER_BUFFER_SIZE];
    char buffer[DEBUGGER_BUFFER_SIZE];

    /* Initialize */
    Debugger_CommandReset(0, NULL);

    printf("Print 'help' to list all availlable command.\n");

    while(Debugger_Info.State != DEBUGGER_STATE_EXIT)
    {
        Debugger_Info.State = DEBUGGER_STATE_RUN;

        /* Get user input */
        if(Debugger_GetUserInput(buffer) == NULL)
        {
            return;
        }

        /* Remember current string before handling it */
        strncpy(buffer_curr, buffer, DEBUGGER_BUFFER_SIZE);

        /* Use previous command if the current input have no command */
        char *pch = strtok(buffer, " ");
        if(pch == NULL)
        {
            strncpy(buffer,      buffer_prev, DEBUGGER_BUFFER_SIZE);
            strncpy(buffer_curr, buffer_prev, DEBUGGER_BUFFER_SIZE);
            pch = strtok(buffer, " ");
        }

        /* Cut the string in token */
        int com_argc = 0;
        char *com_argv[DEBUGGER_ARG_COUNT];
        while(pch != NULL)
        {    
            com_argv[com_argc] = pch;
            com_argc ++;

            if(com_argc > DEBUGGER_ARG_COUNT)
            {
                /* Ignore extra argument */
                break;
            }

            pch = strtok(NULL, " ");
        }

        /* Find the right command and execute it */
        int i, found = 0;
        for(i=0; i<(int)ARRAY_SIZE(Debugger_Command); i++)
        {
            if(strcmp(com_argv[0], Debugger_Command[i].Name) == 0)
            {
                found = 1;
                break;
            }
            if(Debugger_Command[i].Shortcut != NULL)
            {
                if(strcmp(com_argv[0], Debugger_Command[i].Shortcut) == 0)
                {
                    found = 1;
                    break;
                }
            }
        }

        /* Execute command */
        if(found == 1)
        {
            Debugger_Command[i].Callback(com_argc, (char const **)com_argv);
            strncpy(buffer_prev, buffer_curr, DEBUGGER_BUFFER_SIZE);
        }
        else
        {
            printf("Unknown command. Print 'help' to list availlable command.\n");
        }
    }
}


void Debugger_Log(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fflush(stdout);
}


void Debugger_NotifyPcAddress(uint16_t addr)
{
    for(int i=0; i<Debugger_Info.BreakListCount; i++)
    {
        if(addr == Debugger_Info.BreakListAddr[i])
        {
            Debugger_Info.State = DEBUGGER_STATE_BREAK;
            printf("Breakpoint: 0x%04X\n", addr);
            return;
        }
    }
}


/******************************************************/
/* Command function                                   */
/******************************************************/

/**
 * Run the program until breakpoint/watchpoint
 */
static void Debugger_CommandStep(int argc, char const * argv[])
{
    if(argc > 2)
    {
        printf("Wrong number of argument\n");
        return;
    }

    /* Get the number of step to execute */
    int step = 1;
    if(argc == 2)
    {
        step = strtol(argv[1], NULL, 0);
    }

    for(int i=0; i<step; i++)
    {
        Cpu_Step();

        if(Debugger_Info.State == DEBUGGER_STATE_BREAK)
        {
            break;
        }
    }

    /* Display CPU after stepping */
    Cpu_Print();
}

/**
 * Run the program until breakpoint/watchpoint
 */
static void Debugger_CommandRun(int argc, char const * argv[])
{
    /* Unused param */
    (void) argc;
	(void) argv;

    while(Debugger_Info.State == DEBUGGER_STATE_RUN)
    {
        Cpu_Step();
    }

    /* Display CPU after stepping */
    Cpu_Print();
}

/**
 * Reset the program
 */
static void Debugger_CommandReset(int argc, char const * argv[])
{
    /* Unused param */
    (void) argc;
	(void) argv;

    /* @todo find a better way to do it */
    Memory_Initialize();
    Cpu_Initialize();
    Memory_LoadFile("rom/bootstrap.bin", 0);
}


/**
 * Set break point
 */
static void Debugger_CommandBreak(int argc, char const * argv[])
{
    /* breakpoint presence */
    int found = 0;

    if(argc != 2)
    {
        printf("Wrong number of argument\n");
        return;
    }

    /* Get breakpoint addresss */
    uint16_t addr = (uint16_t)strtol(argv[1], NULL, 0);

    /* Check Table overflow */
    if(Debugger_Info.BreakListCount == DEBUGGER_BREAKPOINT_COUNT)
    {
        found = 1;
        printf("Cannot register more than %d breakpoint.\n", DEBUGGER_BREAKPOINT_COUNT);
    }

    /* Check for breakpoint presence */
    for(int i=0; i<Debugger_Info.BreakListCount; i++)
    {
        if(addr == Debugger_Info.BreakListAddr[i])
        {
            found = 1;
            break;
        }
    }
   
    /* Register breakpoint if not found */
    if(found == 0)
    {
        Debugger_Info.BreakListAddr[Debugger_Info.BreakListCount] = addr;
        Debugger_Info.BreakListCount ++;
    }

    /* Print breakpoint list */
    printf("Breakpoint list:\n");
    for(int i=0; i<Debugger_Info.BreakListCount; i++)
    {
        printf("#%d: 0x%04x\n", i, Debugger_Info.BreakListAddr[i]);
    }
}


/**
 * Set watch point
 */
static void Debugger_CommandWatch(int argc, char const * argv[])
{
    /* Watchpoint presence */
    int found = 0;

    if((argc != 2) && (argc != 3))
    {
        printf("Wrong number of argument\n");
        return;
    }

   /* Get watchpoint addresss */
    uint16_t addr = (uint16_t)strtol(argv[1], NULL, 0);

    /* Get watchpoint size area */
    uint16_t size = 1;
    if(argc == 3)
    {
        size = (uint16_t)strtol(argv[2], NULL, 0);
    }

    /* Check Table overflow */
    if(Debugger_Info.WatchListCount == DEBUGGER_WATCHPOINT_COUNT)
    {
        found = 1;
        printf("Cannot register more than %d watchpoint.\n", DEBUGGER_WATCHPOINT_COUNT);
    }

    /* Check for breakpoint presence */
    for(int i=0; i<Debugger_Info.WatchListCount; i++)
    {
        if((addr == Debugger_Info.WatchListAddr[i]) &&
           (size == Debugger_Info.WatchListSize[i]))
        {
            found = 1;
            break;
        }
    }

    /* Register watchpoint */
    if(found == 0)
    {
        Debugger_Info.WatchListAddr[Debugger_Info.WatchListCount] = addr;
        Debugger_Info.WatchListSize[Debugger_Info.WatchListCount] = size;
        Debugger_Info.WatchListCount ++;
    }

    /* Print watchpoint list */
    printf("Watchpoint list:\n");
    for(int i=0; i<Debugger_Info.WatchListCount; i++)
    {
        addr = Debugger_Info.WatchListAddr[i];
        size = Debugger_Info.WatchListSize[i];
        if(size == 1)
        {
            printf("#%d: 0x%04X\n", i, addr);
        }
        else
        {
            printf("#%d: 0x%04X-0x%04X\n", i, addr, addr + size - 1);
        }
    }
}


/**
 * Clear all watch/break point
 */
static void Debugger_CommandClear(int argc, char const * argv[])
{
    /* Unused parameter */
    (void) argc;
	(void) argv;

    Debugger_Info.WatchListCount = 0;
    Debugger_Info.BreakListCount = 0;

    /* Print watchpoint list */
    printf("All watchpoint and breakpoint removed.\n");
}


/**
 * Print Memory
 */
static void Debugger_CommandMem(int argc, char const * argv[])
{
    /* @todo get the memory addresss and size */
    (void) argc;
	(void) argv;
    uint16_t addr = 0;
    int size = 1;

    /* Print Memory area */
    printf("Memory:\n");
    for(int i=0; i<size; i++)
    {
        printf("#0x%04x: 0x%02x\n", addr + i, Memory_Read(addr + i));
    }
}

/**
 * Print CPU
 */
static void Debugger_CommandCpu(int argc, char const * argv[])
{
    /* Unused parameter */
    (void) argc;
	(void) argv;

    /* Print CPU */
    Cpu_Print();
}

/**
 * Print help
 */
static void Debugger_CommandHelp(int argc, char const * argv[])
{
    /* Unused parameter */
    (void) argc;
	(void) argv;

    /* Print help */
    printf("------------------------------------------------------------------------\n");
    printf(":     Name Argument       : Explanation                                :\n");
    printf("------------------------------------------------------------------------\n");
    for(int i=0; i<(int)ARRAY_SIZE(Debugger_Command); i++)
    {
        Debugger_Command_t const * pCmd = &Debugger_Command[i];
        printf(":%9s %-15s: %-43s:\n", pCmd->Name, pCmd->Argument, pCmd->Help);
    }
    printf("------------------------------------------------------------------------\n");
}

/**
 * Quit the debugger
 */
static void Debugger_CommandQuit(int argc, char const * argv[])
{
    /* Unused parameter */
    (void) argc;
	(void) argv;

    /* Make the program exit */
    Debugger_Info.State = DEBUGGER_STATE_EXIT;
}

