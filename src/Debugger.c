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

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Memory.h>
#include <Debugger.h>
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

/** Memory print line count */
#define DEBUGGER_MEM_LINE_COUNT     4


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
    int MemoryAddress;                                  /**< Memory address to display */
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

/* Utility */
static char * Debugger_GetUserInput(char * buffer);
static bool Debugger_IsBreakpoint(uint16_t addr);
static void Debugger_PrintState(void);


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
    {"reset", "rst", "",             "Reset the program.",                      Debugger_CommandReset},

    /* Breakpoint and Watchpoint */
    {"break", "b", "<addr>",         "Set a new breakpoint.",                   Debugger_CommandBreak},
    {"watch", "w", "<addr> [size]",  "Set a new watchpoint. (default: size=1)", Debugger_CommandWatch},
    {"clear", "c", "",               "Remove all breakpoint.",                  Debugger_CommandClear},

    /* Memory */
    {"mem", "", "<addr> [size]",     "Print memory area. (default: size=1)",    Debugger_CommandMem},
    {"cpu", "", "",                  "Print CPU register.",                     Debugger_CommandCpu},

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


void Debugger_NotifyPcChange(uint16_t addr)
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


void Debugger_NotifyMemoryWrite(uint16_t addr, uint8_t data)
{
    for(int i=0; i<Debugger_Info.WatchListCount; i++)
    {
        uint16_t waddr = Debugger_Info.WatchListAddr[i];
        uint16_t wsize = Debugger_Info.WatchListSize[i];
        if((addr >= waddr) && (addr < waddr + wsize))
        {
            Debugger_Info.State = DEBUGGER_STATE_BREAK;
            printf("Watchpoint: 0x%04X = 0x%02X\n", addr, data);
            return;
        }
    }
}


static bool Debugger_IsBreakpoint(uint16_t addr)
{
    for(int i=0; i<Debugger_Info.BreakListCount; i++)
    {
        if(addr == Debugger_Info.BreakListAddr[i])
        {
            return true;
        }
    }

    return false;
}


/**
 * Print the following information:
 * ┌────────┬──────────────────────────────────────────────────┐
 * │ Memory │ 00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f │
 * ├────────┼──────────────────────────────────────────────────┤
 * │ 0x0000 │ 31 fe ff af 21 ff 9f 32  cb 7c 20 fb 21 26 ff 0e │
 * │ 0x0010 │ 11 3e 80 32 e2 0c 3e f3  e2 32 3e 77 77 3e fc e0 │
 * │ 0x0020 │ 47 11 04 01 21 10 80 1a  cd 95 00 cd 96 00 13 7b │
 * │ 0x0030 │ FE 34 20 F3 11 D8 00 06  08 1A 13 22 23 05 20 F9 │
 * └────────┴──────────────────────────────────────────────────┘
 * ┌──────────────┐ ┌──────────────────────────────────────────┐
 * │ CPU Register │ │ Program                                  │
 * ├────┬─────────┤ ├──────────┬────────────────────┬──────────┤
 * │ AF │ 0x3b80  │ │ o 0x0000 │ LD SP,0xfffe       │ 31 fe ff │
 * │ BC │ 0x8012  │ │   0x0003 │ XOR A              │ af       │
 * │ DE │ 0x2528  │ │   0x0004 │ LD HL,0x9fff       │ 21 ff 9f │
 * │ HL │ 0xff45  │ │ o 0x0007 │ LD (HL-),A         │ 32       │
 * │ SP │ 0xfffe  │ │   0x0008 │ BIT 7,H            │ cb 7c    │
 * │ PC │ 0x0000  │ │   0x000a │ JR NZ,-5           │ 20 fb    │
 * └────┴─────────┘ └──────────┴────────────────────┴──────────┘
 */
static void Debugger_PrintState(void)
{
    /* Print memory header */
    printf("┌────────┬──────────────────────────────────────────────────┐\n");
    printf("│ Memory │ 00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f │\n");
    printf("├────────┼──────────────────────────────────────────────────┤\n");

    /* Print memory */
    uint16_t mem_start = Debugger_Info.MemoryAddress & 0xFFF0;
    uint16_t mem_end   = mem_start + (0x0010 * DEBUGGER_MEM_LINE_COUNT);
    for(uint16_t i=mem_start; i<mem_end; i += 0x0010)
    {
        printf("│ 0x%04x │ ", i);
        for(uint16_t j=0x0000; j<0x0008; j++)
        {
            printf("%02x ", Memory_Read(i + j));
        }
        printf(" ");
        for(uint16_t j=0x0008; j<0x0010; j++)
        {
            printf("%02x ", Memory_Read(i + j));
        }
        printf("│\n");
    }

    /* Print memory footer and cpu/program header */
    printf("└────────┴──────────────────────────────────────────────────┘\n");
    printf("┌──────────────┐ ┌──────────────────────────────────────────┐\n");
    printf("│ CPU Register │ │ Program                                  │\n");
    printf("├────┬─────────┤ ├──────────┬────────────────────┬──────────┤\n");

    /* Print Cpu data and Program data */
    char const *cpu_reg[CPU_REG_NUM] = {"AF", "BC", "DE", "HL", "SP", "PC"};
    uint16_t    cpu_pc = CPU_REG16(CPU_R_PC)->UWord;
    for(int i=0; i<CPU_REG_NUM; i++)
    {
        /* CPU */
        printf("│ %2s │ 0x%04x  │ ", cpu_reg[i], CPU_REG16(i)->UWord);

        /* Program */
        char buffer[DEBUGGER_BUFFER_SIZE];
        int size;
        Cpu_GetOpcodeInfo(cpu_pc, buffer, &size);
        printf("│ %c ", Debugger_IsBreakpoint(cpu_pc) ? 'o':' ');
        printf("0x%04x │", cpu_pc);
        printf(" %-18s │\n", buffer);

        cpu_pc += size;
    }

    /* Print CPU and Program footer */
    printf("└────┴─────────┘ └──────────┴────────────────────┴──────────┘\n");
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
    Debugger_PrintState();
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
    Debugger_PrintState();
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
    if((argc != 2) && (argc != 3))
    {
        printf("Wrong number of argument\n");
        return;
    }

    /* Get the memory addresss and size */
    uint16_t addr = (uint16_t)strtol(argv[1], NULL, 0);

    Debugger_Info.MemoryAddress = addr;
    Debugger_PrintState();
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
    Debugger_PrintState();
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

