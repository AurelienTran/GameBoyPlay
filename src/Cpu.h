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

#ifndef _CPU_H_
#define _CPU_H_


/******************************************************/
/* Include                                            */
/******************************************************/

#include <stdint.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

/**
 * Get CPU internal 16 bit register pointer
 * @param reg The 16 bit register Cpu_RegName_e id
 */
#define CPU_REG16(reg)  (&Cpu_Info.Reg[reg])

/**
 * Get CPU internal 8 bit register pointer
 * @param reg The 8 bit register Cpu_RegName_e id
 */
#define CPU_REG8(reg)   (&Cpu_Info.Reg[(reg) / 2].Byte[(reg) % 2])


/******************************************************/
/* Type                                               */
/******************************************************/

/** CPU Register name */
typedef enum tagCpu_RegName_e
{
    /* 16 bit Register Name */
    CPU_R_AF = 0,   /**< Accumulator and Flag register */
    CPU_R_BC,       /**< All purpose register */
    CPU_R_DE,       /**< All purpose register */
    CPU_R_HL,       /**< All purpose register */
    CPU_R_SP,       /**< Stack Pointer register */
    CPU_R_PC,       /**< Program Pointer register */
    CPU_REG_NUM,    /**< Number of internal register */

    /* 8 bit Register Name (little endian) */
    CPU_R_F = 0,    /**< Flag register */
    CPU_R_A,        /**< Accumulator register */
    CPU_R_C,        /**< All purpose register */
    CPU_R_B,        /**< All purpose register */
    CPU_R_E,        /**< All purpose register */
    CPU_R_D,        /**< All purpose register */
    CPU_R_L,        /**< All purpose register */
    CPU_R_H         /**< All purpose register */
} Cpu_RegName_e;

/** CPU Flag name */
typedef enum tagCpu_FlagName_e
{
    CPU_F_Z   = 0x80,   /**< Zero Flag bitmap */
    CPU_F_N   = 0x40,   /**< Substract Flag bitmap */
    CPU_F_H   = 0x20,   /**< Half Carry Flag bitmap */
    CPU_F_C   = 0x10,   /**< Carry Flag bitmap */
    CPU_F_NO  = 0x00,   /**< No flag bitmap */
    CPU_F_ALL = 0xF0    /**< All flag (Zero, Substract, Half Carry, Carry) bitmap */
} Cpu_FlagName_e;

/** All purpose 8 bit register type */
typedef union tagCpu_Reg8_t
{
    uint8_t UByte;  /**< Unsigned 8 bit */
    int8_t  SByte;  /**< Signed 8 bit */
} Cpu_Reg8_t;

/** All purpose 16 bit register type */
typedef union tagCpu_Reg16_t
{
    uint16_t   UWord;     /**< Unsigned 16 bit access */
    int16_t    SWord;     /**< Signed 16 bit access */
    Cpu_Reg8_t Byte[2];   /**< 8 bit access */
} Cpu_Reg16_t;

/** CPU Info */
typedef struct tagCpu_Info_t
{
    Cpu_Reg16_t Reg[CPU_REG_NUM];   /**< Internal Register */
} Cpu_Info_t;


/******************************************************/
/* Prototype                                          */
/******************************************************/

/**
 * Initialize CPU
 */
extern void Cpu_Initialize(void);

/**
 * Process 1 CPU instruction
 * @return The number of cycle used for the instruction
 */
extern uint32_t Cpu_Step(void);

/**
 * Print CPU internal state for debug purpose
 */
extern void Cpu_Print(void);

/**
 * Get PC register
 * @return CPU PC register
 */
inline uint16_t Cpu_GetProgramCounter(void);


/******************************************************/
/* Variable                                           */
/******************************************************/

/** CPU Info */
extern Cpu_Info_t Cpu_Info;


/******************************************************/
/* Function                                           */
/******************************************************/

inline uint16_t Cpu_GetProgramCounter(void)
{
    return CPU_REG16(CPU_R_PC)->UWord;
}


#endif /* _CPU_H_ */

