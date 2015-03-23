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
#include <string.h>
#include <Cpu.h>
#include <Memory.h>
#include <Debugger.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

/**
 * OpCode unused parameter
 */
#define CPU_NULL    0

/**
 * Concat 2 Byte data to 1 Word
 * @param data0 The first Byte
 * @param data1 The second Byte
 */
#define CONCAT(data0, data1) ((data1) << 8 | (data0))

/**
 * Set a Flag
 * @param flag The Cpu_FlagName_e flag
 */
#define CPU_FLAG_SET(flag) do{ CPU_REG8(CPU_R_F)->UByte |= (flag); } while(0)

/**
 * Clear a Flag
 * @param flag The Cpu_FlagName_e flag
 */
#define CPU_FLAG_CLEAR(flag) do{ CPU_REG8(CPU_R_F)->UByte &= ~(flag); } while(0)

/**
 * Check flag
 * @param mask The mask bitmap to apply to flag before comparison
 * @param compare The comparison bitmap to check
 */
#define CPU_FLAG_CHECK(mask, compare) ((CPU_REG8(CPU_R_F)->UByte & mask) == compare)


/******************************************************/
/* Type                                               */
/******************************************************/

/* Forward declaration for Cpu_Callback_t definition */
typedef struct tagCpu_OpCode_t Cpu_OpCode_t;

/**
 * Callback to Execute an OpCode
 * @param opcode The opcode to execute
 * @return The number of cycle used to execute the opcode
 */
typedef int (*Cpu_Callback_t)(Cpu_OpCode_t const * const opcode);

/** CPU OpCode information */
typedef struct tagCpu_OpCode_t
{
    uint32_t Value;     /**< OpCode Value */
    uint32_t Size;      /**< OpCode Byte size */
    char const * Name;  /**< OpCode Name */
    uint32_t Param0;    /**< OpCode Param 1 */
    uint32_t Param1;    /**< Opcode Param 2 */
    Cpu_Callback_t Callback; /**< OpCode execution callback */
} Cpu_OpCode_t;


/******************************************************/
/* Prototype                                          */
/******************************************************/

/**
 * Get PC data
 * @return data pointed by PC
 * @note Each call increment PC register
 */
static inline uint8_t Cpu_ReadPc(void);

/* Misc/Control Command */
static int Cpu_Execute_Unimplemented(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_PREFIX_CB(Cpu_OpCode_t const * const opcode);

/* Jump/Call Command */
static int Cpu_Execute_CALL_F_NN(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_JR_F_N(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RET(Cpu_OpCode_t const * const opcode);

/* 8 bit Load/Move/Store Command */
static int Cpu_Execute_LD_R_N(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_R_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_R_pRR(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_pR_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_pN_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_pNN_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LD_pRR_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LDD_pRR_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_LDI_pRR_R(Cpu_OpCode_t const * const opcode);

/* 16 bit Load/Move/Store Command */
static int Cpu_Execute_LD_RR_NN(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_PUSH_RR(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_POP_RR(Cpu_OpCode_t const * const opcode);

/* 8 bit Arithmetic/Logical Command */
static int Cpu_Execute_INC_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_DEC_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_XOR_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_CP_N(Cpu_OpCode_t const * const opcode);

/* 16 bit Arithmetic/Logical Command */
static int Cpu_Execute_INC_RR(Cpu_OpCode_t const * const opcode);

/* 8 bit Rotation/Shift/Bit Command */
static int Cpu_Execute_BIT_N_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_BIT_N_pRR(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_SET_N_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_SET_N_pRR(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RES_N_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RES_N_pRR(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RLA(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RL_R(Cpu_OpCode_t const * const opcode);
static int Cpu_Execute_RL_pRR(Cpu_OpCode_t const * const opcode);


/******************************************************/
/* Variable                                           */
/******************************************************/

/** CPU Info */
Cpu_Info_t Cpu_Info;

/** Callback table for each OpCode */
static Cpu_OpCode_t const Cpu_OpCode[] =
{
    {0x00, 1, "NOP",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x01, 3, "LD BC,0x%04X\n",             CPU_R_BC, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x02, 1, "LD (BC),A\n",                CPU_R_BC, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x03, 1, "INC BC\n",                   CPU_R_BC, CPU_NULL, Cpu_Execute_INC_RR},
    {0x04, 1, "INC B\n",                    CPU_R_B,  CPU_NULL, Cpu_Execute_INC_R},
    {0x05, 1, "DEC B\n",                    CPU_R_B,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x06, 2, "LD B,0x%02X\n",              CPU_R_B,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x07, 1, "RLCA",                       CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x08, 3, "LD (a16),SP",                CPU_NULL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0x09, 1, "ADD HL,BC",                  CPU_R_HL, CPU_R_BC, Cpu_Execute_Unimplemented},
    {0x0A, 1, "LD A,(BC)\n",                CPU_R_A,  CPU_R_BC, Cpu_Execute_LD_R_pRR},
    {0x0B, 1, "DEC BC",                     CPU_R_BC, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0C, 1, "INC C\n",                    CPU_R_C,  CPU_NULL, Cpu_Execute_INC_R},
    {0x0D, 1, "DEC C\n",                    CPU_R_C,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x0E, 2, "LD C,0x%02X\n",              CPU_R_C,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x0F, 1, "RRCA",                       CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x10, 2, "STOP",                       CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x11, 3, "LD DE,0x%04X\n",             CPU_R_DE, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x12, 1, "LD (DE),A\n",                CPU_R_DE, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x13, 1, "INC DE\n",                   CPU_R_DE, CPU_NULL, Cpu_Execute_INC_RR},
    {0x14, 1, "INC D\n",                    CPU_R_D,  CPU_NULL, Cpu_Execute_INC_R},
    {0x15, 1, "DEC D\n",                    CPU_R_D,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x16, 2, "LD D,0x%02X\n",              CPU_R_D,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x17, 1, "RLA\n",                      CPU_NULL, CPU_NULL, Cpu_Execute_RLA},
    {0x18, 2, "JR %d\n",                    CPU_F_NO, CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x19, 1, "ADD HL,DE",                  CPU_R_HL, CPU_R_DE, Cpu_Execute_Unimplemented},
    {0x1A, 1, "LD A,(DE)\n",                CPU_R_A,  CPU_R_DE, Cpu_Execute_LD_R_pRR},
    {0x1B, 1, "DEC DE",                     CPU_R_DE, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1C, 1, "INC E\n",                    CPU_R_E,  CPU_NULL, Cpu_Execute_INC_R},
    {0x1D, 1, "DEC E\n",                    CPU_R_E,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x1E, 2, "LD E,0x%02X\n",              CPU_R_E,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x1F, 1, "RRA",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x20, 2, "JR NZ,%d\n",                 CPU_F_Z,  CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x21, 3, "LD HL,0x%04X\n",             CPU_R_HL, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x22, 1, "LD (HL+),A\n",               CPU_R_HL, CPU_R_A,  Cpu_Execute_LDI_pRR_R},
    {0x23, 1, "INC HL\n",                   CPU_R_HL, CPU_NULL, Cpu_Execute_INC_RR},
    {0x24, 1, "INC H\n",                    CPU_R_H,  CPU_NULL, Cpu_Execute_INC_R},
    {0x25, 1, "DEC H\n",                    CPU_R_H,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x26, 2, "LD H,0x%02X\n",              CPU_R_H,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x27, 1, "DAA",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x28, 2, "JR Z,%d\n",                  CPU_F_Z,  CPU_F_Z,  Cpu_Execute_JR_F_N},
    {0x29, 1, "ADD HL,HL",                  CPU_R_HL, CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x2A, 1, "LD A,(HL+)",                 CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x2B, 1, "DEC HL",                     CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2C, 1, "INC L\n",                    CPU_R_L,  CPU_NULL, Cpu_Execute_INC_R},
    {0x2D, 1, "DEC L\n",                    CPU_R_L,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x2E, 2, "LD L,0x%02X\n",              CPU_R_L,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x2F, 1, "CPL",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x30, 2, "JR NC,%d\n",                 CPU_F_C,  CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x31, 3, "LD SP,0x%04X\n",             CPU_R_SP, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x32, 1, "LD (HL-),A\n",               CPU_R_HL, CPU_R_A,  Cpu_Execute_LDD_pRR_R},
    {0x33, 1, "INC SP\n",                   CPU_R_SP, CPU_NULL, Cpu_Execute_INC_RR},
    {0x34, 1, "INC (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x35, 1, "DEC (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x36, 2, "LD (HL),d8",                 CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x37, 1, "SCF",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x38, 2, "JR C,%d\n",                  CPU_F_C,  CPU_F_C,  Cpu_Execute_JR_F_N},
    {0x39, 1, "ADD HL,SP",                  CPU_R_HL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0x3A, 1, "LD A,(HL-)",                 CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x3B, 1, "DEC SP",                     CPU_R_SP, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3C, 1, "INC A\n",                    CPU_R_A,  CPU_NULL, Cpu_Execute_INC_R},
    {0x3D, 1, "DEC A\n",                    CPU_R_A,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x3E, 2, "LD A,0x%02X\n",              CPU_R_A,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x3F, 1, "CCF",                        CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x40, 1, "LD B,B\n",                   CPU_R_B,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x41, 1, "LD B,C\n",                   CPU_R_B,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x42, 1, "LD B,D\n",                   CPU_R_B,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x43, 1, "LD B,E\n",                   CPU_R_B,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x44, 1, "LD B,H\n",                   CPU_R_B,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x45, 1, "LD B,L\n",                   CPU_R_B,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x46, 1, "LD B,(HL)\n",                CPU_R_B,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x47, 1, "LD B,A\n",                   CPU_R_B,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x48, 1, "LD C,B\n",                   CPU_R_C,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x49, 1, "LD C,C\n",                   CPU_R_C,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x4A, 1, "LD C,D\n",                   CPU_R_C,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x4B, 1, "LD C,E\n",                   CPU_R_C,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x4C, 1, "LD C,H\n",                   CPU_R_C,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x4D, 1, "LD C,L\n",                   CPU_R_C,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x4E, 1, "LD C,(HL)\n",                CPU_R_C,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x4F, 1, "LD C,A\n",                   CPU_R_C,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x50, 1, "LD D,B\n",                   CPU_R_D,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x51, 1, "LD D,C\n",                   CPU_R_D,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x52, 1, "LD D,D\n",                   CPU_R_D,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x53, 1, "LD D,E\n",                   CPU_R_D,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x54, 1, "LD D,H\n",                   CPU_R_D,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x55, 1, "LD D,L\n",                   CPU_R_D,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x56, 1, "LD D,(HL)\n",                CPU_R_D,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x57, 1, "LD D,A\n",                   CPU_R_D,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x58, 1, "LD E,B\n",                   CPU_R_E,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x59, 1, "LD E,C\n",                   CPU_R_E,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x5A, 1, "LD E,D\n",                   CPU_R_E,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x5B, 1, "LD E,E\n",                   CPU_R_E,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x5C, 1, "LD E,H\n",                   CPU_R_E,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x5D, 1, "LD E,L\n",                   CPU_R_E,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x5E, 1, "LD E,(HL)\n",                CPU_R_E,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x5F, 1, "LD E,A\n",                   CPU_R_E,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x60, 1, "LD H,B\n",                   CPU_R_H,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x61, 1, "LD H,C\n",                   CPU_R_H,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x62, 1, "LD H,D\n",                   CPU_R_H,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x63, 1, "LD H,E\n",                   CPU_R_H,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x64, 1, "LD H,H\n",                   CPU_R_H,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x65, 1, "LD H,L\n",                   CPU_R_H,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x66, 1, "LD H,(HL)\n",                CPU_R_H,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x67, 1, "LD H,A\n",                   CPU_R_H,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x68, 1, "LD L,B\n",                   CPU_R_L,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x69, 1, "LD L,C\n",                   CPU_R_L,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x6A, 1, "LD L,D\n",                   CPU_R_L,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x6B, 1, "LD L,E\n",                   CPU_R_L,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x6C, 1, "LD L,H\n",                   CPU_R_L,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x6D, 1, "LD L,L\n",                   CPU_R_L,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x6E, 1, "LD L,(HL)\n",                CPU_R_L,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x6F, 1, "LD L,A\n",                   CPU_R_L,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x70, 1, "LD (HL),B\n",                CPU_R_HL, CPU_R_B,  Cpu_Execute_LD_pRR_R},
    {0x71, 1, "LD (HL),C\n",                CPU_R_HL, CPU_R_C,  Cpu_Execute_LD_pRR_R},
    {0x72, 1, "LD (HL),D\n",                CPU_R_HL, CPU_R_D,  Cpu_Execute_LD_pRR_R},
    {0x73, 1, "LD (HL),E\n",                CPU_R_HL, CPU_R_E,  Cpu_Execute_LD_pRR_R},
    {0x74, 1, "LD (HL),H\n",                CPU_R_HL, CPU_R_H,  Cpu_Execute_LD_pRR_R},
    {0x75, 1, "LD (HL),L\n",                CPU_R_HL, CPU_R_L,  Cpu_Execute_LD_pRR_R},
    {0x76, 1, "HALT",                       CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x77, 1, "LD (HL),A\n",                CPU_R_HL, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x78, 1, "LD A,B\n",                   CPU_R_A,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x79, 1, "LD A,C\n",                   CPU_R_A,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x7A, 1, "LD A,D\n",                   CPU_R_A,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x7B, 1, "LD A,E\n",                   CPU_R_A,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x7C, 1, "LD A,H\n",                   CPU_R_A,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x7D, 1, "LD A,L\n",                   CPU_R_A,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x7E, 1, "LD A,(HL)\n",                CPU_R_A,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x7F, 1, "LD A,A\n",                   CPU_R_A,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x80, 1, "ADD A,B",                    CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x81, 1, "ADD A,C",                    CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x82, 1, "ADD A,D",                    CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x83, 1, "ADD A,E",                    CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x84, 1, "ADD A,H",                    CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x85, 1, "ADD A,L",                    CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x86, 1, "ADD A,(HL)",                 CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x87, 1, "ADD A,A",                    CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0x88, 1, "ADC A,B",                    CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x89, 1, "ADC A,C",                    CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x8A, 1, "ADC A,D",                    CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x8B, 1, "ADC A,E",                    CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x8C, 1, "ADC A,H",                    CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x8D, 1, "ADC A,L",                    CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x8E, 1, "ADC A,(HL)",                 CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x8F, 1, "ADC A,A",                    CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0x90, 1, "SUB B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x91, 1, "SUB C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x92, 1, "SUB D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x93, 1, "SUB E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x94, 1, "SUB H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x95, 1, "SUB L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x96, 1, "SUB (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x97, 1, "SUB A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x98, 1, "SBC A,B",                    CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x99, 1, "SBC A,C",                    CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x9A, 1, "SBC A,D",                    CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x9B, 1, "SBC A,E",                    CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x9C, 1, "SBC A,H",                    CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x9D, 1, "SBC A,L",                    CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x9E, 1, "SBC A,(HL)",                 CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x9F, 1, "SBC A,A",                    CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0xA0, 1, "AND B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA1, 1, "AND C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA2, 1, "AND D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA3, 1, "AND E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA4, 1, "AND H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA5, 1, "AND L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA6, 1, "AND (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA7, 1, "AND A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA8, 1, "XOR B\n",                    CPU_R_B,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xA9, 1, "XOR C\n",                    CPU_R_C,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAA, 1, "XOR D\n",                    CPU_R_D,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAB, 1, "XOR E\n",                    CPU_R_E,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAC, 1, "XOR H\n",                    CPU_R_H,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAD, 1, "XOR L\n",                    CPU_R_L,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAE, 1, "XOR (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xAF, 1, "XOR A\n",                    CPU_R_A,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xB0, 1, "OR B",                       CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB1, 1, "OR C",                       CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB2, 1, "OR D",                       CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB3, 1, "OR E",                       CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB4, 1, "OR H",                       CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB5, 1, "OR L",                       CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB6, 1, "OR (HL)",                    CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB7, 1, "OR A",                       CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB8, 1, "CP B",                       CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB9, 1, "CP C",                       CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBA, 1, "CP D",                       CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBB, 1, "CP E",                       CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBC, 1, "CP H",                       CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBD, 1, "CP L",                       CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBE, 1, "CP (HL)",                    CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBF, 1, "CP A",                       CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC0, 1, "RET NZ",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC1, 1, "POP BC\n",                   CPU_R_BC, CPU_NULL, Cpu_Execute_POP_RR},
    {0xC2, 3, "JP NZ,a16",                  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC3, 3, "JP a16",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC4, 3, "CALL NZ,0x%04X\n",           CPU_F_Z,  CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xC5, 1, "PUSH BC\n",                  CPU_R_BC, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xC6, 2, "ADD A,d8",                   CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC7, 1, "RST 00H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC8, 1, "RET Z",                      CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC9, 1, "RET\n",                      CPU_NULL, CPU_NULL, Cpu_Execute_RET},
    {0xCA, 3, "JP Z,a16",                   CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xCB, 2, "PREFIX CB\n",                CPU_NULL, CPU_NULL, Cpu_Execute_PREFIX_CB},
    {0xCC, 3, "CALL Z,0x%04X\n",            CPU_F_Z,  CPU_F_Z,  Cpu_Execute_CALL_F_NN},
    {0xCD, 3, "CALL 0x%04X\n",              CPU_F_NO, CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xCE, 2, "ADC A,d8",                   CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xCF, 1, "RST 08H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD0, 1, "RET NC",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD1, 1, "POP DE\n",                   CPU_R_DE, CPU_NULL, Cpu_Execute_POP_RR},
    {0xD2, 3, "JP NC,a16",                  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD3, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD4, 3, "CALL NC,0x%04X\n",           CPU_F_C,  CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xD5, 1, "PUSH DE\n",                  CPU_R_DE, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xD6, 2, "SUB d8",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD7, 1, "RST 10H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD8, 1, "RET C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD9, 1, "RETI",                       CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDA, 3, "JP C,a16",                   CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDB, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDC, 3, "CALL C,0x%04X\n",            CPU_F_C,  CPU_F_C,  Cpu_Execute_CALL_F_NN},
    {0xDD, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDE, 2, "SBC A,d8",                   CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDF, 1, "RST 18H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE0, 2, "LD (0xFF00+0x%02X),A\n",     CPU_NULL, CPU_R_A,  Cpu_Execute_LD_pN_R},
    {0xE1, 1, "POP HL\n",                   CPU_R_HL, CPU_NULL, Cpu_Execute_POP_RR},
    {0xE2, 1, "LD (0xFF00+C),A\n",          CPU_R_C,  CPU_R_A,  Cpu_Execute_LD_pR_R},
    {0xE3, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE4, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE5, 1, "PUSH HL\n",                  CPU_R_HL, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xE6, 2, "AND d8",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE7, 1, "RST 20H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE8, 2, "ADD SP,r8",                  CPU_R_SP, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE9, 1, "JP (HL)",                    CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEA, 3, "LD (0x%04X),A\n",            CPU_NULL, CPU_R_A,  Cpu_Execute_LD_pNN_R},
    {0xEB, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEC, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xED, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEE, 2, "XOR d8",                     CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEF, 1, "RST 28H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF0, 2, "LDH A,(a8)",                 CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF1, 1, "POP AF\n",                   CPU_R_AF, CPU_NULL, Cpu_Execute_POP_RR},
    {0xF2, 2, "LD A,(C)",                   CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0xF3, 1, "DI",                         CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF4, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF5, 1, "PUSH AF\n",                  CPU_R_AF, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xF6, 2, "OR d8",                      CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF7, 1, "RST 30H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF8, 2, "LD HL,SP+r8",                CPU_R_HL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0xF9, 1, "LD SP,HL",                   CPU_R_SP, CPU_R_HL, Cpu_Execute_Unimplemented},
    {0xFA, 3, "LD A,(a16)",                 CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFB, 1, "EI",                         CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFC, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFD, 1, "-",                          CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFE, 2, "CP 0x%02X\n",                CPU_NULL, CPU_NULL, Cpu_Execute_CP_N},
    {0xFF, 1, "RST 38H",                    CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented}
};

static Cpu_OpCode_t const Cpu_OpCode_Prefix[] =
{
    {0x00, 2, "RLC B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x01, 2, "RLC C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x02, 2, "RLC D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x03, 2, "RLC E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x04, 2, "RLC H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x05, 2, "RLC L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x06, 2, "RLC (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x07, 2, "RLC A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x08, 2, "RRC B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x09, 2, "RRC C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0A, 2, "RRC D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0B, 2, "RRC E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0C, 2, "RRC H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0D, 2, "RRC L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0E, 2, "RRC (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0F, 2, "RRC A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x10, 2, "RL B\n",                     CPU_R_B,  CPU_NULL, Cpu_Execute_RL_R},
    {0x11, 2, "RL C\n",                     CPU_R_C,  CPU_NULL, Cpu_Execute_RL_R},
    {0x12, 2, "RL D\n",                     CPU_R_D,  CPU_NULL, Cpu_Execute_RL_R},
    {0x13, 2, "RL E\n",                     CPU_R_E,  CPU_NULL, Cpu_Execute_RL_R},
    {0x14, 2, "RL H\n",                     CPU_R_H,  CPU_NULL, Cpu_Execute_RL_R},
    {0x15, 2, "RL L\n",                     CPU_R_L,  CPU_NULL, Cpu_Execute_RL_R},
    {0x16, 2, "RL (HL)\n",                  CPU_R_HL, CPU_NULL, Cpu_Execute_RL_pRR},
    {0x17, 2, "RL A\n",                     CPU_R_A,  CPU_NULL, Cpu_Execute_RL_R},
    {0x18, 2, "RR B",                       CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x19, 2, "RR C",                       CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1A, 2, "RR D",                       CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1B, 2, "RR E",                       CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1C, 2, "RR H",                       CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1D, 2, "RR L",                       CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1E, 2, "RR (HL)",                    CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1F, 2, "RR A",                       CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x20, 2, "SLA B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x21, 2, "SLA C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x22, 2, "SLA D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x23, 2, "SLA E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x24, 2, "SLA H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x25, 2, "SLA L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x26, 2, "SLA (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x27, 2, "SLA A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x28, 2, "SRA B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x29, 2, "SRA C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2A, 2, "SRA D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2B, 2, "SRA E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2C, 2, "SRA H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2D, 2, "SRA L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2E, 2, "SRA (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2F, 2, "SRA A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x30, 2, "SWAP B",                     CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x31, 2, "SWAP C",                     CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x32, 2, "SWAP D",                     CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x33, 2, "SWAP E",                     CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x34, 2, "SWAP H",                     CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x35, 2, "SWAP L",                     CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x36, 2, "SWAP (HL)",                  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x37, 2, "SWAP A",                     CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x38, 2, "SRL B",                      CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x39, 2, "SRL C",                      CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3A, 2, "SRL D",                      CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3B, 2, "SRL E",                      CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3C, 2, "SRL H",                      CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3D, 2, "SRL L",                      CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3E, 2, "SRL (HL)",                   CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3F, 2, "SRL A",                      CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x40, 2, "BIT 0,B\n",                  0x01,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x41, 2, "BIT 0,C\n",                  0x01,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x42, 2, "BIT 0,D\n",                  0x01,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x43, 2, "BIT 0,E\n",                  0x01,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x44, 2, "BIT 0,H\n",                  0x01,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x45, 2, "BIT 0,L\n",                  0x01,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x46, 2, "BIT 0,(HL)\n",               0x01,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x47, 2, "BIT 0,A\n",                  0x01,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x48, 2, "BIT 1,B\n",                  0x02,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x49, 2, "BIT 1,C\n",                  0x02,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x4A, 2, "BIT 1,D\n",                  0x02,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x4B, 2, "BIT 1,E\n",                  0x02,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x4C, 2, "BIT 1,H\n",                  0x02,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x4D, 2, "BIT 1,L\n",                  0x02,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x4E, 2, "BIT 1,(HL)\n",               0x02,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x4F, 2, "BIT 1,A\n",                  0x02,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x50, 2, "BIT 2,B\n",                  0x04,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x51, 2, "BIT 2,C\n",                  0x04,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x52, 2, "BIT 2,D\n",                  0x04,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x53, 2, "BIT 2,E\n",                  0x04,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x54, 2, "BIT 2,H\n",                  0x04,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x55, 2, "BIT 2,L\n",                  0x04,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x56, 2, "BIT 2,(HL)\n",               0x04,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x57, 2, "BIT 2,A\n",                  0x04,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x58, 2, "BIT 3,B\n",                  0x08,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x59, 2, "BIT 3,C\n",                  0x08,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x5A, 2, "BIT 3,D\n",                  0x08,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x5B, 2, "BIT 3,E\n",                  0x08,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x5C, 2, "BIT 3,H\n",                  0x08,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x5D, 2, "BIT 3,L\n",                  0x08,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x5E, 2, "BIT 3,(HL)\n",               0x08,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x5F, 2, "BIT 3,A\n",                  0x08,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x60, 2, "BIT 4,B\n",                  0x10,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x61, 2, "BIT 4,C\n",                  0x10,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x62, 2, "BIT 4,D\n",                  0x10,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x63, 2, "BIT 4,E\n",                  0x10,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x64, 2, "BIT 4,H\n",                  0x10,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x65, 2, "BIT 4,L\n",                  0x10,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x66, 2, "BIT 4,(HL)\n",               0x10,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x67, 2, "BIT 4,A\n",                  0x10,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x68, 2, "BIT 5,B\n",                  0x20,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x69, 2, "BIT 5,C\n",                  0x20,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x6A, 2, "BIT 5,D\n",                  0x20,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x6B, 2, "BIT 5,E\n",                  0x20,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x6C, 2, "BIT 5,H\n",                  0x20,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x6D, 2, "BIT 5,L\n",                  0x20,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x6E, 2, "BIT 5,(HL)\n",               0x20,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x6F, 2, "BIT 5,A\n",                  0x20,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x70, 2, "BIT 6,B\n",                  0x40,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x71, 2, "BIT 6,C\n",                  0x40,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x72, 2, "BIT 6,D\n",                  0x40,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x73, 2, "BIT 6,E\n",                  0x40,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x74, 2, "BIT 6,H\n",                  0x40,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x75, 2, "BIT 6,L\n",                  0x40,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x76, 2, "BIT 6,(HL)\n",               0x40,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x77, 2, "BIT 6,A\n",                  0x40,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x78, 2, "BIT 7,B\n",                  0x80,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x79, 2, "BIT 7,C\n",                  0x80,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x7A, 2, "BIT 7,D\n",                  0x80,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x7B, 2, "BIT 7,E\n",                  0x80,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x7C, 2, "BIT 7,H\n",                  0x80,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x7D, 2, "BIT 7,L\n",                  0x80,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x7E, 2, "BIT 7,(HL)\n",               0x80,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x7F, 2, "BIT 7,A\n",                  0x80,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x80, 2, "RES 0,B\n",                  0x01,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x81, 2, "RES 0,C\n",                  0x01,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x82, 2, "RES 0,D\n",                  0x01,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x83, 2, "RES 0,E\n",                  0x01,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x84, 2, "RES 0,H\n",                  0x01,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x85, 2, "RES 0,L\n",                  0x01,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x86, 2, "RES 0,(HL)\n",               0x01,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x87, 2, "RES 0,A\n",                  0x01,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x88, 2, "RES 1,B\n",                  0x02,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x89, 2, "RES 1,C\n",                  0x02,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x8A, 2, "RES 1,D\n",                  0x02,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x8B, 2, "RES 1,E\n",                  0x02,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x8C, 2, "RES 1,H\n",                  0x02,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x8D, 2, "RES 1,L\n",                  0x02,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x8E, 2, "RES 1,(HL)\n",               0x02,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x8F, 2, "RES 1,A\n",                  0x02,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x90, 2, "RES 2,B\n",                  0x04,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x91, 2, "RES 2,C\n",                  0x04,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x92, 2, "RES 2,D\n",                  0x04,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x93, 2, "RES 2,E\n",                  0x04,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x94, 2, "RES 2,H\n",                  0x04,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x95, 2, "RES 2,L\n",                  0x04,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x96, 2, "RES 2,(HL)\n",               0x04,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x97, 2, "RES 2,A\n",                  0x04,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x98, 2, "RES 3,B\n",                  0x08,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x99, 2, "RES 3,C\n",                  0x08,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x9A, 2, "RES 3,D\n",                  0x08,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x9B, 2, "RES 3,E\n",                  0x08,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x9C, 2, "RES 3,H\n",                  0x08,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x9D, 2, "RES 3,L\n",                  0x08,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x9E, 2, "RES 3,(HL)\n",               0x08,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x9F, 2, "RES 3,A\n",                  0x08,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xA0, 2, "RES 4,B\n",                  0x10,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xA1, 2, "RES 4,C\n",                  0x10,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xA2, 2, "RES 4,D\n",                  0x10,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xA3, 2, "RES 4,E\n",                  0x10,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xA4, 2, "RES 4,H\n",                  0x10,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xA5, 2, "RES 4,L\n",                  0x10,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xA6, 2, "RES 4,(HL)\n",               0x10,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xA7, 2, "RES 4,A\n",                  0x10,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xA8, 2, "RES 5,B\n",                  0x20,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xA9, 2, "RES 5,C\n",                  0x20,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xAA, 2, "RES 5,D\n",                  0x20,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xAB, 2, "RES 5,E\n",                  0x20,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xAC, 2, "RES 5,H\n",                  0x20,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xAD, 2, "RES 5,L\n",                  0x20,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xAE, 2, "RES 5,(HL)\n",               0x20,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xAF, 2, "RES 5,A\n",                  0x20,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xB0, 2, "RES 6,B\n",                  0x40,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xB1, 2, "RES 6,C\n",                  0x40,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xB2, 2, "RES 6,D\n",                  0x40,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xB3, 2, "RES 6,E\n",                  0x40,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xB4, 2, "RES 6,H\n",                  0x40,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xB5, 2, "RES 6,L\n",                  0x40,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xB6, 2, "RES 6,(HL)\n",               0x40,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xB7, 2, "RES 6,A\n",                  0x40,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xB8, 2, "RES 7,B\n",                  0x80,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xB9, 2, "RES 7,C\n",                  0x80,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xBA, 2, "RES 7,D\n",                  0x80,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xBB, 2, "RES 7,E\n",                  0x80,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xBC, 2, "RES 7,H\n",                  0x80,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xBD, 2, "RES 7,L\n",                  0x80,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xBE, 2, "RES 7,(HL)\n",               0x80,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xBF, 2, "RES 7,A\n",                  0x80,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xC0, 2, "SET 0,B\n",                  0x01,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xC1, 2, "SET 0,C\n",                  0x01,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xC2, 2, "SET 0,D\n",                  0x01,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xC3, 2, "SET 0,E\n",                  0x01,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xC4, 2, "SET 0,H\n",                  0x01,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xC5, 2, "SET 0,L\n",                  0x01,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xC6, 2, "SET 0,(HL)\n",               0x01,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xC7, 2, "SET 0,A\n",                  0x01,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xC8, 2, "SET 1,B\n",                  0x02,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xC9, 2, "SET 1,C\n",                  0x02,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xCA, 2, "SET 1,D\n",                  0x02,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xCB, 2, "SET 1,E\n",                  0x02,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xCC, 2, "SET 1,H\n",                  0x02,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xCD, 2, "SET 1,L\n",                  0x02,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xCE, 2, "SET 1,(HL)\n",               0x02,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xCF, 2, "SET 1,A\n",                  0x02,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xD0, 2, "SET 2,B\n",                  0x04,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xD1, 2, "SET 2,C\n",                  0x04,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xD2, 2, "SET 2,D\n",                  0x04,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xD3, 2, "SET 2,E\n",                  0x04,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xD4, 2, "SET 2,H\n",                  0x04,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xD5, 2, "SET 2,L\n",                  0x04,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xD6, 2, "SET 2,(HL)\n",               0x04,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xD7, 2, "SET 2,A\n",                  0x04,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xD8, 2, "SET 3,B\n",                  0x08,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xD9, 2, "SET 3,C\n",                  0x08,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xDA, 2, "SET 3,D\n",                  0x08,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xDB, 2, "SET 3,E\n",                  0x08,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xDC, 2, "SET 3,H\n",                  0x08,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xDD, 2, "SET 3,L\n",                  0x08,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xDE, 2, "SET 3,(HL)\n",               0x08,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xDF, 2, "SET 3,A\n",                  0x08,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xE0, 2, "SET 4,B\n",                  0x10,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xE1, 2, "SET 4,C\n",                  0x10,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xE2, 2, "SET 4,D\n",                  0x10,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xE3, 2, "SET 4,E\n",                  0x10,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xE4, 2, "SET 4,H\n",                  0x10,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xE5, 2, "SET 4,L\n",                  0x10,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xE6, 2, "SET 4,(HL)\n",               0x10,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xE7, 2, "SET 4,A\n",                  0x10,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xE8, 2, "SET 5,B\n",                  0x20,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xE9, 2, "SET 5,C\n",                  0x20,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xEA, 2, "SET 5,D\n",                  0x20,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xEB, 2, "SET 5,E\n",                  0x20,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xEC, 2, "SET 5,H\n",                  0x20,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xED, 2, "SET 5,L\n",                  0x20,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xEE, 2, "SET 5,(HL)\n",               0x20,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xEF, 2, "SET 5,A\n",                  0x20,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xF0, 2, "SET 6,B\n",                  0x40,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xF1, 2, "SET 6,C\n",                  0x40,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xF2, 2, "SET 6,D\n",                  0x40,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xF3, 2, "SET 6,E\n",                  0x40,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xF4, 2, "SET 6,H\n",                  0x40,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xF5, 2, "SET 6,L\n",                  0x40,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xF6, 2, "SET 6,(HL)\n",               0x40,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xF7, 2, "SET 6,A\n",                  0x40,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xF8, 2, "SET 7,B\n",                  0x80,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xF9, 2, "SET 7,C\n",                  0x80,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xFA, 2, "SET 7,D\n",                  0x80,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xFB, 2, "SET 7,E\n",                  0x80,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xFC, 2, "SET 7,H\n",                  0x80,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xFD, 2, "SET 7,L\n",                  0x80,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xFE, 2, "SET 7,(HL)\n",               0x80,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xFF, 2, "SET 7,A\n",                  0x80,     CPU_R_A,  Cpu_Execute_SET_N_R}
};


/******************************************************/
/* Function                                           */
/******************************************************/

void Cpu_Initialize(void)
{
    for(int i=0; i<CPU_REG_NUM; i++)
    {
        CPU_REG16(i)->UWord = 0;
    }
}


uint32_t Cpu_Step(void)
{
    /* Get instruction */
    uint8_t const data = Cpu_ReadPc();
    Cpu_OpCode_t const * opcode = &Cpu_OpCode[data];

    /* Execute instruction */
    return opcode->Callback(opcode);
}


static inline uint8_t Cpu_ReadPc(void)
{
    uint16_t const pc = CPU_REG16(CPU_R_PC)->UWord;
    uint8_t const data = Memory_Read(pc);
    CPU_REG16(CPU_R_PC)->UWord = pc + 1;

    /* Notfiy PC for breakpoint */
    Debugger_NotifyPcChange(pc + 1);

    return data;
}


void Cpu_GetOpcodeInfo(uint16_t addr, char *buffer, int *size)
{
    uint8_t const data = Memory_Read(addr);
    Cpu_OpCode_t const * opcode = &Cpu_OpCode[data];
    /* @todo implement a way to retrieve string */
    strcpy(buffer, "TODO");
    *size = opcode->Size;
}


/**
 * OpCode: XXXXXX
 * Size:X, Duration:X, ZNHC Flag:XXXX
 * @todo Delete this function when all opcode implemented
 */
static int Cpu_Execute_Unimplemented(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_INFO("CPU Register:\n");
    DEBUGGER_INFO("#AF: 0x%04X\n", CPU_REG16(CPU_R_AF)->UWord);
    DEBUGGER_INFO("#BC: 0x%04X\n", CPU_REG16(CPU_R_BC)->UWord);
    DEBUGGER_INFO("#DE: 0x%04X\n", CPU_REG16(CPU_R_DE)->UWord);
    DEBUGGER_INFO("#HL: 0x%04X\n", CPU_REG16(CPU_R_HL)->UWord);
    DEBUGGER_INFO("#SP: 0x%04X\n", CPU_REG16(CPU_R_SP)->UWord);
    DEBUGGER_INFO("#PC: 0x%04X\n", CPU_REG16(CPU_R_PC)->UWord);
    DEBUGGER_INFO("Unimplemented Opcode 0x%02X: %s\n", opcode->Value, opcode->Name);
    assert(0);
    return 0;
}


/******************************************************/
/* Misc/Control Command                               */
/******************************************************/

/**
 * Handle OpCode begining by 0xCB
 */
static int Cpu_Execute_PREFIX_CB(Cpu_OpCode_t const * const opcode)
{
    /* Unused parameter */
    (void) opcode;

    /* Get instruction */
    uint8_t const data = Cpu_ReadPc();
    Cpu_OpCode_t const * opcode_prefix = &Cpu_OpCode_Prefix[data];

    /* Execute instruction */
    return opcode_prefix->Callback(opcode_prefix);
}


/******************************************************/
/* Jump/Call Command                                  */
/******************************************************/

/**
 * opcode: CALL F,NN
 * size:3, duration:24/12, znhc flag:----
 */
static int Cpu_Execute_CALL_F_NN(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    uint8_t const data0 = Cpu_ReadPc();
    uint8_t const data1 = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, CONCAT(data0, data1));

    /* Execute the command */
    uint8_t mask = opcode->Param0;
    uint8_t compare = opcode->Param1;
    if(CPU_FLAG_CHECK(mask, compare))
    {
        /* Copy PC in the Stack and update SP */
        uint8_t const pc0 = CPU_REG16(CPU_R_PC)->Byte[0].UByte;
        uint8_t const pc1 = CPU_REG16(CPU_R_PC)->Byte[1].UByte;
        uint16_t const sp = CPU_REG16(CPU_R_SP)->UWord;
        Memory_Write(sp - 1, pc1);
        Memory_Write(sp - 2, pc0);
        CPU_REG16(CPU_R_SP)->UWord = sp - 2;
        
        /* Set PC to the call addr */
        CPU_REG16(CPU_R_PC)->Byte[0].UByte = data0;
        CPU_REG16(CPU_R_PC)->Byte[1].UByte = data1;
        return 24;
    }

    return 12;
}


/**
 * opcode: JR F,N
 * size:2, duration:12/8, znhc flag:----
 */
static int Cpu_Execute_JR_F_N(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    int8_t const data = Cpu_ReadPc();
 
    DEBUGGER_TRACE(opcode->Name, data);

    /* Execute the command */
    uint8_t mask = opcode->Param0;
    uint8_t compare = opcode->Param1;
    if(CPU_FLAG_CHECK(mask, compare))
    {
        CPU_REG16(CPU_R_PC)->UWord += data;
        return 12;
    }

    return 8;
}


/**
 * opcode: RET
 * size:1, duration:16, znhc flag:----
 */
static int Cpu_Execute_RET(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const sp = CPU_REG16(CPU_R_SP)->UWord;
    uint8_t data0 = Memory_Read(sp);
    uint8_t data1 = Memory_Read(sp + 1);
    CPU_REG16(CPU_R_PC)->Byte[0].UByte = data0;
    CPU_REG16(CPU_R_PC)->Byte[1].UByte = data1;
    CPU_REG16(CPU_R_SP)->UWord = sp + 2;
        
    return 16;
}


/******************************************************/
/* 8 bit Load/Move/Store Command                      */
/******************************************************/

/**
 * opcode: LD R,(RR)
 * size:1, duration:8, znhc flag:----
 */
static int Cpu_Execute_LD_R_pRR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const addr = CPU_REG16(opcode->Param1)->UWord;
    uint8_t const data = Memory_Read(addr);
    CPU_REG8(opcode->Param0)->UByte = data;

    return 8;
}


/**
 * opcode: LD R,R
 * size:1, duration:4, znhc flag:----
 */
static int Cpu_Execute_LD_R_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    CPU_REG8(opcode->Param0)->UByte = data;

    return 4;
}


/**
 * opcode: LD R,N
 * size:2, duration:8, znhc flag:----
 */
static int Cpu_Execute_LD_R_N(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    uint8_t const data = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, data);

    /* Execute the command */
    CPU_REG8(opcode->Param0)->UByte = data;

    return 8;
}


/**
 * opcode: LD (NN),R
 * size:3, duration:16, znhc flag:----
 */
static int Cpu_Execute_LD_pNN_R(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    uint8_t const data0 = Cpu_ReadPc();
    uint8_t const data1 = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, CONCAT(data0, data1));

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = CONCAT(data0, data1);
    Memory_Write(addr, data);

    return 16;
}


/**
 * opcode: LD (0xFF00+N),R
 * size:2, duration:12, znhc flag:----
 */
static int Cpu_Execute_LD_pN_R(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    uint8_t const addrOffset = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, addrOffset);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = 0xFF00 + addrOffset;
    Memory_Write(addr, data);

    return 12;
}


/**
 * opcode: LD (0xFF00+R),R
 * size:1, duration:8, znhc flag:----
 */
static int Cpu_Execute_LD_pR_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = 0xFF00 + CPU_REG8(opcode->Param0)->UByte;
    Memory_Write(addr, data);

    return 8;
}


/**
 * opcode: LD (RR),R
 * size:1, duration:8, znhc flag:----
 */
static int Cpu_Execute_LD_pRR_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = CPU_REG16(opcode->Param0)->UWord;
    Memory_Write(addr, data);

    return 8;
}


/**
 * opcode: LD (RR-),R
 * size:1, duration:8, znhc flag:----
 */
static int Cpu_Execute_LDD_pRR_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = CPU_REG16(opcode->Param0)->UWord;
    Memory_Write(addr, data);
    CPU_REG16(opcode->Param0)->UWord = addr - 1;

    return 8;
}


/**
 * opcode: LD (RR+),R
 * size:1, duration:8, znhc flag:----
 */
static int Cpu_Execute_LDI_pRR_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint16_t const addr = CPU_REG16(opcode->Param0)->UWord;
    Memory_Write(addr, data);
    CPU_REG16(opcode->Param0)->UWord = addr + 1;

    return 8;
}



/******************************************************/
/* 16 bit Load/Move/Store Command                     */
/******************************************************/

/**
 * OpCode: LD RR,d16
 * Size:3, Duration:12, ZNHC Flag:----
 */
static int Cpu_Execute_LD_RR_NN(Cpu_OpCode_t const * const opcode)
{
    /* Get the opcde parameter */
    uint8_t const data0 = Cpu_ReadPc();
    uint8_t const data1 = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, CONCAT(data0, data1));

    /* Execute the command */
    CPU_REG16(opcode->Param0)->Byte[0].UByte = data0;
    CPU_REG16(opcode->Param0)->Byte[1].UByte = data1;

    return 12;
}


/**
 * opcode: PUSH RR
 * size:1, duration:16, znhc flag:----
 */
static int Cpu_Execute_PUSH_RR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const rr0 = CPU_REG16(opcode->Param0)->Byte[0].UByte;
    uint8_t const rr1 = CPU_REG16(opcode->Param0)->Byte[1].UByte;
    uint16_t const sp = CPU_REG16(CPU_R_SP)->UWord;
    Memory_Write(sp - 1, rr1);
    Memory_Write(sp - 2, rr0);
    CPU_REG16(CPU_R_SP)->UWord = sp - 2;
        
    return 16;
}


/**
 * opcode: POP RR
 * size:1, duration:12, znhc flag:----
 */
static int Cpu_Execute_POP_RR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const sp = CPU_REG16(CPU_R_SP)->UWord;
    uint8_t data0 = Memory_Read(sp);
    uint8_t data1 = Memory_Read(sp + 1);
    CPU_REG16(opcode->Param0)->Byte[0].UByte = data0;
    CPU_REG16(opcode->Param0)->Byte[1].UByte = data1;
    CPU_REG16(CPU_R_SP)->UWord = sp + 2;
        
    return 12;
}


/******************************************************/
/* 8 bit Arithmetic/Logical Command                   */
/******************************************************/

/**
 * OpCode: INC R
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
static int Cpu_Execute_INC_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param0)->UByte;
    uint8_t const result = data + 1;
    CPU_REG8(opcode->Param0)->UByte = result;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_Z | CPU_F_N | CPU_F_H);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }
    if((data & 0x0F) == 0x0F)
    {
        CPU_FLAG_SET(CPU_F_H);
    }

    return 4;
}


/**
 * OpCode: DEC R
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
static int Cpu_Execute_DEC_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param0)->UByte;
    uint8_t const result = data - 1;
    CPU_REG8(opcode->Param0)->UByte = result;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_Z | CPU_F_H);
    CPU_FLAG_SET(CPU_F_N);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }
    if((data & 0x0F) == 0x00)
    {
        CPU_FLAG_SET(CPU_F_H);
    }

    return 4;
}


/**
 * OpCode: XOR R
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
static int Cpu_Execute_XOR_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const dataA = CPU_REG8(CPU_R_A)->UByte;
    uint8_t const dataR = CPU_REG8(opcode->Param0)->UByte;
    uint8_t const result = dataA ^ dataR;
    CPU_REG8(CPU_R_A)->UByte = result;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_ALL);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }

    return 4;
}


/**
 * OpCode: CP N
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
static int Cpu_Execute_CP_N(Cpu_OpCode_t const * const opcode)
{
    /* Get instruction */
    uint8_t const data = Cpu_ReadPc();

    DEBUGGER_TRACE(opcode->Name, data);

    /* Execute the command */
    uint8_t const dataA = CPU_REG8(CPU_R_A)->UByte;
    uint8_t const result = dataA - data;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_Z | CPU_F_H | CPU_F_C);
    CPU_FLAG_SET(CPU_F_N);
    if(result == 0)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }
    if((data & 0x0F) > (dataA & 0x0F))
    {
        CPU_FLAG_SET(CPU_F_H);
    }
    if(data > dataA)
    {
        CPU_FLAG_SET(CPU_F_C);
    }

    return 8;
}


/******************************************************/
/* 16 bit Arithmetic/Logical Command                  */
/******************************************************/

/**
 * OpCode: INC RR
 * Size:1, Duration:8, ZNHC Flag:----
 */
static int Cpu_Execute_INC_RR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const data = CPU_REG16(opcode->Param0)->UWord;
    uint16_t const result = data + 1;
    CPU_REG16(opcode->Param0)->UWord = result;

    return 8;
}


/******************************************************/
/* 8 bit Rotation/Shift/Bit Command                   */
/******************************************************/

/**
 * OpCode: BIT N,R
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
static int Cpu_Execute_BIT_N_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint8_t const result = data & mask;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_Z | CPU_F_N);
    CPU_FLAG_SET(CPU_F_H);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }

    return 8;
}


/**
 * OpCode: BIT N,(RR)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
static int Cpu_Execute_BIT_N_pRR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint16_t const addr = CPU_REG16(opcode->Param1)->UWord;
    uint8_t const data = Memory_Read(addr);
    uint8_t const result = data & mask;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_Z | CPU_F_N);
    CPU_FLAG_SET(CPU_F_H);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }

    return 16;
}


/**
 * OpCode: SET N,R
 * Size:2, Duration:8, ZNHC Flag:----
 */
static int Cpu_Execute_SET_N_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint8_t const result = data | mask;
    CPU_REG8(opcode->Param1)->UByte = result;

    return 8;
}


/**
 * OpCode: SET N,pRR
 * Size:2, Duration:16, ZNHC Flag:----
 */
static int Cpu_Execute_SET_N_pRR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint16_t const addr = CPU_REG16(opcode->Param1)->UWord;
    uint8_t const data = Memory_Read(addr);
    uint8_t const result = data | mask;
    Memory_Write(addr, result);

    return 16;
}


/**
 * OpCode: RES N,R
 * Size:2, Duration:8, ZNHC Flag:----
 */
static int Cpu_Execute_RES_N_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint8_t const data = CPU_REG8(opcode->Param1)->UByte;
    uint8_t const result = data & ~mask;
    CPU_REG8(opcode->Param1)->UByte = result;

    return 8;
}


/**
 * OpCode: RES N,pRR
 * Size:2, Duration:16, ZNHC Flag:----
 */
static int Cpu_Execute_RES_N_pRR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const mask = opcode->Param0;
    uint16_t const addr = CPU_REG16(opcode->Param1)->UWord;
    uint8_t const data = Memory_Read(addr);
    uint8_t const result = data & ~mask;
    Memory_Write(addr, result);

    return 16;
}


/**
 * OpCode: RLA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
static int Cpu_Execute_RLA(Cpu_OpCode_t const * const opcode)
{
    /* Unused parameter */
    (void) opcode;

    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(CPU_R_A)->UByte;
    uint8_t const carry = CPU_FLAG_CHECK(CPU_F_C, CPU_F_C) ? 0x01 : 0x00;
    uint8_t const result = (data << 1) | carry;
    CPU_REG8(CPU_R_A)->UByte = result;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_ALL);
    if((data & 0x80) == 0x80)
    {
        CPU_FLAG_SET(CPU_F_C);
    }

    return 4;
}


/**
 * OpCode: RL R
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
static int Cpu_Execute_RL_R(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint8_t const data = CPU_REG8(opcode->Param0)->UByte;
    uint8_t const carry = CPU_FLAG_CHECK(CPU_F_C, CPU_F_C) ? 0x01 : 0x00;
    uint8_t const result = (data << 1) | carry;
    CPU_REG8(opcode->Param0)->UByte = result;

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_ALL);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }
    if((data & 0x80) == 0x80)
    {
        CPU_FLAG_SET(CPU_F_C);
    }

    return 8;
}


/**
 * OpCode: RL pRR
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
static int Cpu_Execute_RL_pRR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const addr = CPU_REG16(opcode->Param0)->UWord;
    uint8_t const data = Memory_Read(addr);
    uint8_t const carry = CPU_FLAG_CHECK(CPU_F_C, CPU_F_C) ? 0x01 : 0x00;
    uint8_t const result = (data << 1) | carry;
    Memory_Write(addr, result);

    /* Set up Flag */
    CPU_FLAG_CLEAR(CPU_F_ALL);
    if(result == 0x00)
    {
        CPU_FLAG_SET(CPU_F_Z);
    }
    if((data & 0x80) == 0x80)
    {
        CPU_FLAG_SET(CPU_F_C);
    }

    return 16;
}

