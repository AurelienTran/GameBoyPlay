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
    uint32_t NameParam; /**< OpCode Name parameter type */
    uint32_t Param0;    /**< OpCode Param 1 */
    uint32_t Param1;    /**< Opcode Param 2 */
    Cpu_Callback_t Callback; /**< OpCode execution callback */
} Cpu_OpCode_t;

/** CPU Name parameter */
typedef enum tagCpu_NameParam_t
{
    CPU_P_NONE,         /**< No param */
    CPU_P_UWORD,        /**< Unsigned 16 bit param */
    CPU_P_UBYTE,        /**< Unsigned 8 bit param */
    CPU_P_SBYTE         /**< Signed 8 bit param */
} Cpu_NameParam_t;


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
static int Cpu_Execute_DEC_RR(Cpu_OpCode_t const * const opcode);

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
    {0x00, 1, "NOP",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x01, 3, "LD BC,0x%04x",       CPU_P_UWORD, CPU_R_BC, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x02, 1, "LD (BC),A",          CPU_P_NONE,  CPU_R_BC, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x03, 1, "INC BC",             CPU_P_NONE,  CPU_R_BC, CPU_NULL, Cpu_Execute_INC_RR},
    {0x04, 1, "INC B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_INC_R},
    {0x05, 1, "DEC B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x06, 2, "LD B,0x%02x",        CPU_P_UBYTE, CPU_R_B,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x07, 1, "RLCA",               CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x08, 3, "LD (0x%04x),SP",     CPU_P_UWORD, CPU_NULL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0x09, 1, "ADD HL,BC",          CPU_P_NONE,  CPU_R_HL, CPU_R_BC, Cpu_Execute_Unimplemented},
    {0x0A, 1, "LD A,(BC)",          CPU_P_NONE,  CPU_R_A,  CPU_R_BC, Cpu_Execute_LD_R_pRR},
    {0x0B, 1, "DEC BC",             CPU_P_NONE,  CPU_R_BC, CPU_NULL, Cpu_Execute_DEC_RR},
    {0x0C, 1, "INC C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_INC_R},
    {0x0D, 1, "DEC C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x0E, 2, "LD C,0x%02x",        CPU_P_UBYTE, CPU_R_C,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x0F, 1, "RRCA",               CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x10, 2, "STOP",               CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x11, 3, "LD DE,0x%04x",       CPU_P_UWORD, CPU_R_DE, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x12, 1, "LD (DE),A",          CPU_P_NONE,  CPU_R_DE, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x13, 1, "INC DE",             CPU_P_NONE,  CPU_R_DE, CPU_NULL, Cpu_Execute_INC_RR},
    {0x14, 1, "INC D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_INC_R},
    {0x15, 1, "DEC D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x16, 2, "LD D,0x%02x",        CPU_P_UBYTE, CPU_R_D,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x17, 1, "RLA",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_RLA},
    {0x18, 2, "JR %d",              CPU_P_SBYTE, CPU_F_NO, CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x19, 1, "ADD HL,DE",          CPU_P_NONE,  CPU_R_HL, CPU_R_DE, Cpu_Execute_Unimplemented},
    {0x1A, 1, "LD A,(DE)",          CPU_P_NONE,  CPU_R_A,  CPU_R_DE, Cpu_Execute_LD_R_pRR},
    {0x1B, 1, "DEC DE",             CPU_P_NONE,  CPU_R_DE, CPU_NULL, Cpu_Execute_DEC_RR},
    {0x1C, 1, "INC E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_INC_R},
    {0x1D, 1, "DEC E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x1E, 2, "LD E,0x%02x",        CPU_P_UBYTE, CPU_R_E,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x1F, 1, "RRA",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x20, 2, "JR NZ,%d",           CPU_P_SBYTE, CPU_F_Z,  CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x21, 3, "LD HL,0x%04x",       CPU_P_UWORD, CPU_R_HL, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x22, 1, "LD (HL+),A",         CPU_P_NONE,  CPU_R_HL, CPU_R_A,  Cpu_Execute_LDI_pRR_R},
    {0x23, 1, "INC HL",             CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_INC_RR},
    {0x24, 1, "INC H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_INC_R},
    {0x25, 1, "DEC H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x26, 2, "LD H,0x%02x",        CPU_P_UBYTE, CPU_R_H,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x27, 1, "DAA",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x28, 2, "JR Z,%d",            CPU_P_SBYTE, CPU_F_Z,  CPU_F_Z,  Cpu_Execute_JR_F_N},
    {0x29, 1, "ADD HL,HL",          CPU_P_NONE,  CPU_R_HL, CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x2A, 1, "LD A,(HL+)",         CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x2B, 1, "DEC HL",             CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_DEC_RR},
    {0x2C, 1, "INC L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_INC_R},
    {0x2D, 1, "DEC L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x2E, 2, "LD L,0x%02x",        CPU_P_UBYTE, CPU_R_L,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x2F, 1, "CPL",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x30, 2, "JR NC,%d",           CPU_P_SBYTE, CPU_F_C,  CPU_F_NO, Cpu_Execute_JR_F_N},
    {0x31, 3, "LD SP,0x%04x",       CPU_P_UWORD, CPU_R_SP, CPU_NULL, Cpu_Execute_LD_RR_NN},
    {0x32, 1, "LD (HL-),A",         CPU_P_NONE,  CPU_R_HL, CPU_R_A,  Cpu_Execute_LDD_pRR_R},
    {0x33, 1, "INC SP",             CPU_P_NONE,  CPU_R_SP, CPU_NULL, Cpu_Execute_INC_RR},
    {0x34, 1, "INC (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x35, 1, "DEC (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x36, 2, "LD (HL),0x%02x",     CPU_P_UBYTE, CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x37, 1, "SCF",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x38, 2, "JR C,%d",            CPU_P_SBYTE, CPU_F_C,  CPU_F_C,  Cpu_Execute_JR_F_N},
    {0x39, 1, "ADD HL,SP",          CPU_P_NONE,  CPU_R_HL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0x3A, 1, "LD A,(HL-)",         CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x3B, 1, "DEC SP",             CPU_P_NONE,  CPU_R_SP, CPU_NULL, Cpu_Execute_DEC_RR},
    {0x3C, 1, "INC A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_INC_R},
    {0x3D, 1, "DEC A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_DEC_R},
    {0x3E, 2, "LD A,0x%02x",        CPU_P_UBYTE, CPU_R_A,  CPU_NULL, Cpu_Execute_LD_R_N},
    {0x3F, 1, "CCF",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x40, 1, "LD B,B",             CPU_P_NONE,  CPU_R_B,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x41, 1, "LD B,C",             CPU_P_NONE,  CPU_R_B,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x42, 1, "LD B,D",             CPU_P_NONE,  CPU_R_B,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x43, 1, "LD B,E",             CPU_P_NONE,  CPU_R_B,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x44, 1, "LD B,H",             CPU_P_NONE,  CPU_R_B,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x45, 1, "LD B,L",             CPU_P_NONE,  CPU_R_B,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x46, 1, "LD B,(HL)",          CPU_P_NONE,  CPU_R_B,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x47, 1, "LD B,A",             CPU_P_NONE,  CPU_R_B,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x48, 1, "LD C,B",             CPU_P_NONE,  CPU_R_C,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x49, 1, "LD C,C",             CPU_P_NONE,  CPU_R_C,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x4A, 1, "LD C,D",             CPU_P_NONE,  CPU_R_C,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x4B, 1, "LD C,E",             CPU_P_NONE,  CPU_R_C,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x4C, 1, "LD C,H",             CPU_P_NONE,  CPU_R_C,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x4D, 1, "LD C,L",             CPU_P_NONE,  CPU_R_C,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x4E, 1, "LD C,(HL)",          CPU_P_NONE,  CPU_R_C,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x4F, 1, "LD C,A",             CPU_P_NONE,  CPU_R_C,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x50, 1, "LD D,B",             CPU_P_NONE,  CPU_R_D,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x51, 1, "LD D,C",             CPU_P_NONE,  CPU_R_D,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x52, 1, "LD D,D",             CPU_P_NONE,  CPU_R_D,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x53, 1, "LD D,E",             CPU_P_NONE,  CPU_R_D,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x54, 1, "LD D,H",             CPU_P_NONE,  CPU_R_D,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x55, 1, "LD D,L",             CPU_P_NONE,  CPU_R_D,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x56, 1, "LD D,(HL)",          CPU_P_NONE,  CPU_R_D,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x57, 1, "LD D,A",             CPU_P_NONE,  CPU_R_D,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x58, 1, "LD E,B",             CPU_P_NONE,  CPU_R_E,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x59, 1, "LD E,C",             CPU_P_NONE,  CPU_R_E,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x5A, 1, "LD E,D",             CPU_P_NONE,  CPU_R_E,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x5B, 1, "LD E,E",             CPU_P_NONE,  CPU_R_E,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x5C, 1, "LD E,H",             CPU_P_NONE,  CPU_R_E,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x5D, 1, "LD E,L",             CPU_P_NONE,  CPU_R_E,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x5E, 1, "LD E,(HL)",          CPU_P_NONE,  CPU_R_E,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x5F, 1, "LD E,A",             CPU_P_NONE,  CPU_R_E,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x60, 1, "LD H,B",             CPU_P_NONE,  CPU_R_H,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x61, 1, "LD H,C",             CPU_P_NONE,  CPU_R_H,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x62, 1, "LD H,D",             CPU_P_NONE,  CPU_R_H,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x63, 1, "LD H,E",             CPU_P_NONE,  CPU_R_H,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x64, 1, "LD H,H",             CPU_P_NONE,  CPU_R_H,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x65, 1, "LD H,L",             CPU_P_NONE,  CPU_R_H,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x66, 1, "LD H,(HL)",          CPU_P_NONE,  CPU_R_H,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x67, 1, "LD H,A",             CPU_P_NONE,  CPU_R_H,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x68, 1, "LD L,B",             CPU_P_NONE,  CPU_R_L,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x69, 1, "LD L,C",             CPU_P_NONE,  CPU_R_L,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x6A, 1, "LD L,D",             CPU_P_NONE,  CPU_R_L,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x6B, 1, "LD L,E",             CPU_P_NONE,  CPU_R_L,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x6C, 1, "LD L,H",             CPU_P_NONE,  CPU_R_L,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x6D, 1, "LD L,L",             CPU_P_NONE,  CPU_R_L,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x6E, 1, "LD L,(HL)",          CPU_P_NONE,  CPU_R_L,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x6F, 1, "LD L,A",             CPU_P_NONE,  CPU_R_L,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x70, 1, "LD (HL),B",          CPU_P_NONE,  CPU_R_HL, CPU_R_B,  Cpu_Execute_LD_pRR_R},
    {0x71, 1, "LD (HL),C",          CPU_P_NONE,  CPU_R_HL, CPU_R_C,  Cpu_Execute_LD_pRR_R},
    {0x72, 1, "LD (HL),D",          CPU_P_NONE,  CPU_R_HL, CPU_R_D,  Cpu_Execute_LD_pRR_R},
    {0x73, 1, "LD (HL),E",          CPU_P_NONE,  CPU_R_HL, CPU_R_E,  Cpu_Execute_LD_pRR_R},
    {0x74, 1, "LD (HL),H",          CPU_P_NONE,  CPU_R_HL, CPU_R_H,  Cpu_Execute_LD_pRR_R},
    {0x75, 1, "LD (HL),L",          CPU_P_NONE,  CPU_R_HL, CPU_R_L,  Cpu_Execute_LD_pRR_R},
    {0x76, 1, "HALT",               CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x77, 1, "LD (HL),A",          CPU_P_NONE,  CPU_R_HL, CPU_R_A,  Cpu_Execute_LD_pRR_R},
    {0x78, 1, "LD A,B",             CPU_P_NONE,  CPU_R_A,  CPU_R_B,  Cpu_Execute_LD_R_R},
    {0x79, 1, "LD A,C",             CPU_P_NONE,  CPU_R_A,  CPU_R_C,  Cpu_Execute_LD_R_R},
    {0x7A, 1, "LD A,D",             CPU_P_NONE,  CPU_R_A,  CPU_R_D,  Cpu_Execute_LD_R_R},
    {0x7B, 1, "LD A,E",             CPU_P_NONE,  CPU_R_A,  CPU_R_E,  Cpu_Execute_LD_R_R},
    {0x7C, 1, "LD A,H",             CPU_P_NONE,  CPU_R_A,  CPU_R_H,  Cpu_Execute_LD_R_R},
    {0x7D, 1, "LD A,L",             CPU_P_NONE,  CPU_R_A,  CPU_R_L,  Cpu_Execute_LD_R_R},
    {0x7E, 1, "LD A,(HL)",          CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_LD_R_pRR},
    {0x7F, 1, "LD A,A",             CPU_P_NONE,  CPU_R_A,  CPU_R_A,  Cpu_Execute_LD_R_R},
    {0x80, 1, "ADD A,B",            CPU_P_NONE,  CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x81, 1, "ADD A,C",            CPU_P_NONE,  CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x82, 1, "ADD A,D",            CPU_P_NONE,  CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x83, 1, "ADD A,E",            CPU_P_NONE,  CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x84, 1, "ADD A,H",            CPU_P_NONE,  CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x85, 1, "ADD A,L",            CPU_P_NONE,  CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x86, 1, "ADD A,(HL)",         CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x87, 1, "ADD A,A",            CPU_P_NONE,  CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0x88, 1, "ADC A,B",            CPU_P_NONE,  CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x89, 1, "ADC A,C",            CPU_P_NONE,  CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x8A, 1, "ADC A,D",            CPU_P_NONE,  CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x8B, 1, "ADC A,E",            CPU_P_NONE,  CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x8C, 1, "ADC A,H",            CPU_P_NONE,  CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x8D, 1, "ADC A,L",            CPU_P_NONE,  CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x8E, 1, "ADC A,(HL)",         CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x8F, 1, "ADC A,A",            CPU_P_NONE,  CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0x90, 1, "SUB B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x91, 1, "SUB C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x92, 1, "SUB D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x93, 1, "SUB E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x94, 1, "SUB H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x95, 1, "SUB L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x96, 1, "SUB (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x97, 1, "SUB A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x98, 1, "SBC A,B",            CPU_P_NONE,  CPU_R_A,  CPU_R_B,  Cpu_Execute_Unimplemented},
    {0x99, 1, "SBC A,C",            CPU_P_NONE,  CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0x9A, 1, "SBC A,D",            CPU_P_NONE,  CPU_R_A,  CPU_R_D,  Cpu_Execute_Unimplemented},
    {0x9B, 1, "SBC A,E",            CPU_P_NONE,  CPU_R_A,  CPU_R_E,  Cpu_Execute_Unimplemented},
    {0x9C, 1, "SBC A,H",            CPU_P_NONE,  CPU_R_A,  CPU_R_H,  Cpu_Execute_Unimplemented},
    {0x9D, 1, "SBC A,L",            CPU_P_NONE,  CPU_R_A,  CPU_R_L,  Cpu_Execute_Unimplemented},
    {0x9E, 1, "SBC A,(HL)",         CPU_P_NONE,  CPU_R_A,  CPU_R_HL, Cpu_Execute_Unimplemented},
    {0x9F, 1, "SBC A,A",            CPU_P_NONE,  CPU_R_A,  CPU_R_A,  Cpu_Execute_Unimplemented},
    {0xA0, 1, "AND B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA1, 1, "AND C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA2, 1, "AND D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA3, 1, "AND E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA4, 1, "AND H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA5, 1, "AND L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA6, 1, "AND (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA7, 1, "AND A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xA8, 1, "XOR B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xA9, 1, "XOR C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAA, 1, "XOR D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAB, 1, "XOR E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAC, 1, "XOR H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAD, 1, "XOR L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xAE, 1, "XOR (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xAF, 1, "XOR A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_XOR_R},
    {0xB0, 1, "OR B",               CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB1, 1, "OR C",               CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB2, 1, "OR D",               CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB3, 1, "OR E",               CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB4, 1, "OR H",               CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB5, 1, "OR L",               CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB6, 1, "OR (HL)",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB7, 1, "OR A",               CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB8, 1, "CP B",               CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xB9, 1, "CP C",               CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBA, 1, "CP D",               CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBB, 1, "CP E",               CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBC, 1, "CP H",               CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBD, 1, "CP L",               CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBE, 1, "CP (HL)",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xBF, 1, "CP A",               CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC0, 1, "RET NZ",             CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC1, 1, "POP BC",             CPU_P_NONE,  CPU_R_BC, CPU_NULL, Cpu_Execute_POP_RR},
    {0xC2, 3, "JP NZ,0x%04x",       CPU_P_UWORD, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC3, 3, "JP 0x%04x",          CPU_P_UWORD, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC4, 3, "CALL NZ,0x%04x",     CPU_P_UWORD, CPU_F_Z,  CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xC5, 1, "PUSH BC",            CPU_P_NONE,  CPU_R_BC, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xC6, 2, "ADD A,0x%02x",       CPU_P_UBYTE, CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC7, 1, "RST 00H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC8, 1, "RET Z",              CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xC9, 1, "RET",                CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_RET},
    {0xCA, 3, "JP Z,0x%04x",        CPU_P_UWORD, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xCB, 2, "PREFIX CB",          CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_PREFIX_CB},
    {0xCC, 3, "CALL Z,0x%04x",      CPU_P_UWORD, CPU_F_Z,  CPU_F_Z,  Cpu_Execute_CALL_F_NN},
    {0xCD, 3, "CALL 0x%04x",        CPU_P_UWORD, CPU_F_NO, CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xCE, 2, "ADC A,0x%02x",       CPU_P_UBYTE, CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xCF, 1, "RST 08H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD0, 1, "RET NC",             CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD1, 1, "POP DE",             CPU_P_NONE,  CPU_R_DE, CPU_NULL, Cpu_Execute_POP_RR},
    {0xD2, 3, "JP NC,0x%04x",       CPU_P_UWORD, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD3, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD4, 3, "CALL NC,0x%04x",     CPU_P_UWORD, CPU_F_C,  CPU_F_NO, Cpu_Execute_CALL_F_NN},
    {0xD5, 1, "PUSH DE",            CPU_P_NONE,  CPU_R_DE, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xD6, 2, "SUB 0x%02x",         CPU_P_UBYTE, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD7, 1, "RST 10H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD8, 1, "RET C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xD9, 1, "RETI",               CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDA, 3, "JP C,0x%04x",        CPU_P_UWORD, CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDB, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDC, 3, "CALL C,0x%04x",      CPU_P_UWORD, CPU_F_C,  CPU_F_C,  Cpu_Execute_CALL_F_NN},
    {0xDD, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDE, 2, "SBC A,0x%02x",       CPU_P_UBYTE, CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xDF, 1, "RST 18H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE0, 2, "LD (0xff%02x),A",    CPU_P_UBYTE, CPU_NULL, CPU_R_A,  Cpu_Execute_LD_pN_R},
    {0xE1, 1, "POP HL",             CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_POP_RR},
    {0xE2, 1, "LD (0xff00+C),A",    CPU_P_NONE,  CPU_R_C,  CPU_R_A,  Cpu_Execute_LD_pR_R},
    {0xE3, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE4, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE5, 1, "PUSH HL",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xE6, 2, "AND 0x%02x",         CPU_P_UBYTE, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE7, 1, "RST 20H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE8, 2, "ADD SP,%d",          CPU_P_SBYTE, CPU_R_SP, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xE9, 1, "JP (HL)",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEA, 3, "LD (0x%04x),A",      CPU_P_UWORD, CPU_NULL, CPU_R_A,  Cpu_Execute_LD_pNN_R},
    {0xEB, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEC, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xED, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEE, 2, "XOR 0x%02x",         CPU_P_UBYTE, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xEF, 1, "RST 28H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF0, 2, "LDH A,(0x%02x)",     CPU_P_UBYTE, CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF1, 1, "POP AF",             CPU_P_NONE,  CPU_R_AF, CPU_NULL, Cpu_Execute_POP_RR},
    {0xF2, 2, "LD A,(C)",           CPU_P_NONE,  CPU_R_A,  CPU_R_C,  Cpu_Execute_Unimplemented},
    {0xF3, 1, "DI",                 CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF4, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF5, 1, "PUSH AF",            CPU_P_NONE,  CPU_R_AF, CPU_NULL, Cpu_Execute_PUSH_RR},
    {0xF6, 2, "OR 0x%02x",          CPU_P_UBYTE, CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF7, 1, "RST 30H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xF8, 2, "LD HL,SP+%d",        CPU_P_SBYTE, CPU_R_HL, CPU_R_SP, Cpu_Execute_Unimplemented},
    {0xF9, 1, "LD SP,HL",           CPU_P_NONE,  CPU_R_SP, CPU_R_HL, Cpu_Execute_Unimplemented},
    {0xFA, 3, "LD A,(0x%04x)",      CPU_P_UWORD, CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFB, 1, "EI",                 CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFC, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFD, 1, "UNKNOWN",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0xFE, 2, "CP 0x%02x",          CPU_P_UBYTE, CPU_NULL, CPU_NULL, Cpu_Execute_CP_N},
    {0xFF, 1, "RST 38H",            CPU_P_NONE,  CPU_NULL, CPU_NULL, Cpu_Execute_Unimplemented}
};

static Cpu_OpCode_t const Cpu_OpCode_Prefix[] =
{
    {0x00, 2, "RLC B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x01, 2, "RLC C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x02, 2, "RLC D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x03, 2, "RLC E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x04, 2, "RLC H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x05, 2, "RLC L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x06, 2, "RLC (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x07, 2, "RLC A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x08, 2, "RRC B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x09, 2, "RRC C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0A, 2, "RRC D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0B, 2, "RRC E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0C, 2, "RRC H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0D, 2, "RRC L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0E, 2, "RRC (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x0F, 2, "RRC A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x10, 2, "RL B",               CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_RL_R},
    {0x11, 2, "RL C",               CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_RL_R},
    {0x12, 2, "RL D",               CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_RL_R},
    {0x13, 2, "RL E",               CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_RL_R},
    {0x14, 2, "RL H",               CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_RL_R},
    {0x15, 2, "RL L",               CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_RL_R},
    {0x16, 2, "RL (HL)",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_RL_pRR},
    {0x17, 2, "RL A",               CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_RL_R},
    {0x18, 2, "RR B",               CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x19, 2, "RR C",               CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1A, 2, "RR D",               CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1B, 2, "RR E",               CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1C, 2, "RR H",               CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1D, 2, "RR L",               CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1E, 2, "RR (HL)",            CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x1F, 2, "RR A",               CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x20, 2, "SLA B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x21, 2, "SLA C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x22, 2, "SLA D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x23, 2, "SLA E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x24, 2, "SLA H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x25, 2, "SLA L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x26, 2, "SLA (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x27, 2, "SLA A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x28, 2, "SRA B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x29, 2, "SRA C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2A, 2, "SRA D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2B, 2, "SRA E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2C, 2, "SRA H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2D, 2, "SRA L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2E, 2, "SRA (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x2F, 2, "SRA A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x30, 2, "SWAP B",             CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x31, 2, "SWAP C",             CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x32, 2, "SWAP D",             CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x33, 2, "SWAP E",             CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x34, 2, "SWAP H",             CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x35, 2, "SWAP L",             CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x36, 2, "SWAP (HL)",          CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x37, 2, "SWAP A",             CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x38, 2, "SRL B",              CPU_P_NONE,  CPU_R_B,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x39, 2, "SRL C",              CPU_P_NONE,  CPU_R_C,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3A, 2, "SRL D",              CPU_P_NONE,  CPU_R_D,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3B, 2, "SRL E",              CPU_P_NONE,  CPU_R_E,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3C, 2, "SRL H",              CPU_P_NONE,  CPU_R_H,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3D, 2, "SRL L",              CPU_P_NONE,  CPU_R_L,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3E, 2, "SRL (HL)",           CPU_P_NONE,  CPU_R_HL, CPU_NULL, Cpu_Execute_Unimplemented},
    {0x3F, 2, "SRL A",              CPU_P_NONE,  CPU_R_A,  CPU_NULL, Cpu_Execute_Unimplemented},
    {0x40, 2, "BIT 0,B",            CPU_P_NONE,  0x01,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x41, 2, "BIT 0,C",            CPU_P_NONE,  0x01,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x42, 2, "BIT 0,D",            CPU_P_NONE,  0x01,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x43, 2, "BIT 0,E",            CPU_P_NONE,  0x01,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x44, 2, "BIT 0,H",            CPU_P_NONE,  0x01,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x45, 2, "BIT 0,L",            CPU_P_NONE,  0x01,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x46, 2, "BIT 0,(HL)",         CPU_P_NONE,  0x01,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x47, 2, "BIT 0,A",            CPU_P_NONE,  0x01,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x48, 2, "BIT 1,B",            CPU_P_NONE,  0x02,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x49, 2, "BIT 1,C",            CPU_P_NONE,  0x02,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x4A, 2, "BIT 1,D",            CPU_P_NONE,  0x02,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x4B, 2, "BIT 1,E",            CPU_P_NONE,  0x02,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x4C, 2, "BIT 1,H",            CPU_P_NONE,  0x02,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x4D, 2, "BIT 1,L",            CPU_P_NONE,  0x02,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x4E, 2, "BIT 1,(HL)",         CPU_P_NONE,  0x02,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x4F, 2, "BIT 1,A",            CPU_P_NONE,  0x02,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x50, 2, "BIT 2,B",            CPU_P_NONE,  0x04,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x51, 2, "BIT 2,C",            CPU_P_NONE,  0x04,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x52, 2, "BIT 2,D",            CPU_P_NONE,  0x04,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x53, 2, "BIT 2,E",            CPU_P_NONE,  0x04,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x54, 2, "BIT 2,H",            CPU_P_NONE,  0x04,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x55, 2, "BIT 2,L",            CPU_P_NONE,  0x04,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x56, 2, "BIT 2,(HL)",         CPU_P_NONE,  0x04,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x57, 2, "BIT 2,A",            CPU_P_NONE,  0x04,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x58, 2, "BIT 3,B",            CPU_P_NONE,  0x08,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x59, 2, "BIT 3,C",            CPU_P_NONE,  0x08,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x5A, 2, "BIT 3,D",            CPU_P_NONE,  0x08,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x5B, 2, "BIT 3,E",            CPU_P_NONE,  0x08,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x5C, 2, "BIT 3,H",            CPU_P_NONE,  0x08,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x5D, 2, "BIT 3,L",            CPU_P_NONE,  0x08,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x5E, 2, "BIT 3,(HL)",         CPU_P_NONE,  0x08,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x5F, 2, "BIT 3,A",            CPU_P_NONE,  0x08,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x60, 2, "BIT 4,B",            CPU_P_NONE,  0x10,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x61, 2, "BIT 4,C",            CPU_P_NONE,  0x10,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x62, 2, "BIT 4,D",            CPU_P_NONE,  0x10,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x63, 2, "BIT 4,E",            CPU_P_NONE,  0x10,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x64, 2, "BIT 4,H",            CPU_P_NONE,  0x10,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x65, 2, "BIT 4,L",            CPU_P_NONE,  0x10,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x66, 2, "BIT 4,(HL)",         CPU_P_NONE,  0x10,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x67, 2, "BIT 4,A",            CPU_P_NONE,  0x10,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x68, 2, "BIT 5,B",            CPU_P_NONE,  0x20,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x69, 2, "BIT 5,C",            CPU_P_NONE,  0x20,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x6A, 2, "BIT 5,D",            CPU_P_NONE,  0x20,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x6B, 2, "BIT 5,E",            CPU_P_NONE,  0x20,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x6C, 2, "BIT 5,H",            CPU_P_NONE,  0x20,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x6D, 2, "BIT 5,L",            CPU_P_NONE,  0x20,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x6E, 2, "BIT 5,(HL)",         CPU_P_NONE,  0x20,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x6F, 2, "BIT 5,A",            CPU_P_NONE,  0x20,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x70, 2, "BIT 6,B",            CPU_P_NONE,  0x40,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x71, 2, "BIT 6,C",            CPU_P_NONE,  0x40,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x72, 2, "BIT 6,D",            CPU_P_NONE,  0x40,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x73, 2, "BIT 6,E",            CPU_P_NONE,  0x40,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x74, 2, "BIT 6,H",            CPU_P_NONE,  0x40,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x75, 2, "BIT 6,L",            CPU_P_NONE,  0x40,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x76, 2, "BIT 6,(HL)",         CPU_P_NONE,  0x40,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x77, 2, "BIT 6,A",            CPU_P_NONE,  0x40,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x78, 2, "BIT 7,B",            CPU_P_NONE,  0x80,     CPU_R_B,  Cpu_Execute_BIT_N_R},
    {0x79, 2, "BIT 7,C",            CPU_P_NONE,  0x80,     CPU_R_C,  Cpu_Execute_BIT_N_R},
    {0x7A, 2, "BIT 7,D",            CPU_P_NONE,  0x80,     CPU_R_D,  Cpu_Execute_BIT_N_R},
    {0x7B, 2, "BIT 7,E",            CPU_P_NONE,  0x80,     CPU_R_E,  Cpu_Execute_BIT_N_R},
    {0x7C, 2, "BIT 7,H",            CPU_P_NONE,  0x80,     CPU_R_H,  Cpu_Execute_BIT_N_R},
    {0x7D, 2, "BIT 7,L",            CPU_P_NONE,  0x80,     CPU_R_L,  Cpu_Execute_BIT_N_R},
    {0x7E, 2, "BIT 7,(HL)",         CPU_P_NONE,  0x80,     CPU_R_HL, Cpu_Execute_BIT_N_pRR},
    {0x7F, 2, "BIT 7,A",            CPU_P_NONE,  0x80,     CPU_R_A,  Cpu_Execute_BIT_N_R},
    {0x80, 2, "RES 0,B",            CPU_P_NONE,  0x01,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x81, 2, "RES 0,C",            CPU_P_NONE,  0x01,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x82, 2, "RES 0,D",            CPU_P_NONE,  0x01,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x83, 2, "RES 0,E",            CPU_P_NONE,  0x01,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x84, 2, "RES 0,H",            CPU_P_NONE,  0x01,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x85, 2, "RES 0,L",            CPU_P_NONE,  0x01,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x86, 2, "RES 0,(HL)",         CPU_P_NONE,  0x01,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x87, 2, "RES 0,A",            CPU_P_NONE,  0x01,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x88, 2, "RES 1,B",            CPU_P_NONE,  0x02,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x89, 2, "RES 1,C",            CPU_P_NONE,  0x02,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x8A, 2, "RES 1,D",            CPU_P_NONE,  0x02,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x8B, 2, "RES 1,E",            CPU_P_NONE,  0x02,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x8C, 2, "RES 1,H",            CPU_P_NONE,  0x02,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x8D, 2, "RES 1,L",            CPU_P_NONE,  0x02,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x8E, 2, "RES 1,(HL)",         CPU_P_NONE,  0x02,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x8F, 2, "RES 1,A",            CPU_P_NONE,  0x02,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x90, 2, "RES 2,B",            CPU_P_NONE,  0x04,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x91, 2, "RES 2,C",            CPU_P_NONE,  0x04,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x92, 2, "RES 2,D",            CPU_P_NONE,  0x04,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x93, 2, "RES 2,E",            CPU_P_NONE,  0x04,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x94, 2, "RES 2,H",            CPU_P_NONE,  0x04,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x95, 2, "RES 2,L",            CPU_P_NONE,  0x04,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x96, 2, "RES 2,(HL)",         CPU_P_NONE,  0x04,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x97, 2, "RES 2,A",            CPU_P_NONE,  0x04,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0x98, 2, "RES 3,B",            CPU_P_NONE,  0x08,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0x99, 2, "RES 3,C",            CPU_P_NONE,  0x08,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0x9A, 2, "RES 3,D",            CPU_P_NONE,  0x08,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0x9B, 2, "RES 3,E",            CPU_P_NONE,  0x08,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0x9C, 2, "RES 3,H",            CPU_P_NONE,  0x08,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0x9D, 2, "RES 3,L",            CPU_P_NONE,  0x08,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0x9E, 2, "RES 3,(HL)",         CPU_P_NONE,  0x08,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0x9F, 2, "RES 3,A",            CPU_P_NONE,  0x08,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xA0, 2, "RES 4,B",            CPU_P_NONE,  0x10,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xA1, 2, "RES 4,C",            CPU_P_NONE,  0x10,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xA2, 2, "RES 4,D",            CPU_P_NONE,  0x10,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xA3, 2, "RES 4,E",            CPU_P_NONE,  0x10,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xA4, 2, "RES 4,H",            CPU_P_NONE,  0x10,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xA5, 2, "RES 4,L",            CPU_P_NONE,  0x10,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xA6, 2, "RES 4,(HL)",         CPU_P_NONE,  0x10,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xA7, 2, "RES 4,A",            CPU_P_NONE,  0x10,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xA8, 2, "RES 5,B",            CPU_P_NONE,  0x20,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xA9, 2, "RES 5,C",            CPU_P_NONE,  0x20,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xAA, 2, "RES 5,D",            CPU_P_NONE,  0x20,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xAB, 2, "RES 5,E",            CPU_P_NONE,  0x20,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xAC, 2, "RES 5,H",            CPU_P_NONE,  0x20,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xAD, 2, "RES 5,L",            CPU_P_NONE,  0x20,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xAE, 2, "RES 5,(HL)",         CPU_P_NONE,  0x20,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xAF, 2, "RES 5,A",            CPU_P_NONE,  0x20,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xB0, 2, "RES 6,B",            CPU_P_NONE,  0x40,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xB1, 2, "RES 6,C",            CPU_P_NONE,  0x40,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xB2, 2, "RES 6,D",            CPU_P_NONE,  0x40,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xB3, 2, "RES 6,E",            CPU_P_NONE,  0x40,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xB4, 2, "RES 6,H",            CPU_P_NONE,  0x40,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xB5, 2, "RES 6,L",            CPU_P_NONE,  0x40,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xB6, 2, "RES 6,(HL)",         CPU_P_NONE,  0x40,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xB7, 2, "RES 6,A",            CPU_P_NONE,  0x40,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xB8, 2, "RES 7,B",            CPU_P_NONE,  0x80,     CPU_R_B,  Cpu_Execute_RES_N_R},
    {0xB9, 2, "RES 7,C",            CPU_P_NONE,  0x80,     CPU_R_C,  Cpu_Execute_RES_N_R},
    {0xBA, 2, "RES 7,D",            CPU_P_NONE,  0x80,     CPU_R_D,  Cpu_Execute_RES_N_R},
    {0xBB, 2, "RES 7,E",            CPU_P_NONE,  0x80,     CPU_R_E,  Cpu_Execute_RES_N_R},
    {0xBC, 2, "RES 7,H",            CPU_P_NONE,  0x80,     CPU_R_H,  Cpu_Execute_RES_N_R},
    {0xBD, 2, "RES 7,L",            CPU_P_NONE,  0x80,     CPU_R_L,  Cpu_Execute_RES_N_R},
    {0xBE, 2, "RES 7,(HL)",         CPU_P_NONE,  0x80,     CPU_R_HL, Cpu_Execute_RES_N_pRR},
    {0xBF, 2, "RES 7,A",            CPU_P_NONE,  0x80,     CPU_R_A,  Cpu_Execute_RES_N_R},
    {0xC0, 2, "SET 0,B",            CPU_P_NONE,  0x01,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xC1, 2, "SET 0,C",            CPU_P_NONE,  0x01,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xC2, 2, "SET 0,D",            CPU_P_NONE,  0x01,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xC3, 2, "SET 0,E",            CPU_P_NONE,  0x01,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xC4, 2, "SET 0,H",            CPU_P_NONE,  0x01,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xC5, 2, "SET 0,L",            CPU_P_NONE,  0x01,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xC6, 2, "SET 0,(HL)",         CPU_P_NONE,  0x01,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xC7, 2, "SET 0,A",            CPU_P_NONE,  0x01,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xC8, 2, "SET 1,B",            CPU_P_NONE,  0x02,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xC9, 2, "SET 1,C",            CPU_P_NONE,  0x02,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xCA, 2, "SET 1,D",            CPU_P_NONE,  0x02,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xCB, 2, "SET 1,E",            CPU_P_NONE,  0x02,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xCC, 2, "SET 1,H",            CPU_P_NONE,  0x02,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xCD, 2, "SET 1,L",            CPU_P_NONE,  0x02,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xCE, 2, "SET 1,(HL)",         CPU_P_NONE,  0x02,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xCF, 2, "SET 1,A",            CPU_P_NONE,  0x02,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xD0, 2, "SET 2,B",            CPU_P_NONE,  0x04,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xD1, 2, "SET 2,C",            CPU_P_NONE,  0x04,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xD2, 2, "SET 2,D",            CPU_P_NONE,  0x04,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xD3, 2, "SET 2,E",            CPU_P_NONE,  0x04,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xD4, 2, "SET 2,H",            CPU_P_NONE,  0x04,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xD5, 2, "SET 2,L",            CPU_P_NONE,  0x04,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xD6, 2, "SET 2,(HL)",         CPU_P_NONE,  0x04,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xD7, 2, "SET 2,A",            CPU_P_NONE,  0x04,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xD8, 2, "SET 3,B",            CPU_P_NONE,  0x08,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xD9, 2, "SET 3,C",            CPU_P_NONE,  0x08,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xDA, 2, "SET 3,D",            CPU_P_NONE,  0x08,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xDB, 2, "SET 3,E",            CPU_P_NONE,  0x08,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xDC, 2, "SET 3,H",            CPU_P_NONE,  0x08,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xDD, 2, "SET 3,L",            CPU_P_NONE,  0x08,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xDE, 2, "SET 3,(HL)",         CPU_P_NONE,  0x08,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xDF, 2, "SET 3,A",            CPU_P_NONE,  0x08,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xE0, 2, "SET 4,B",            CPU_P_NONE,  0x10,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xE1, 2, "SET 4,C",            CPU_P_NONE,  0x10,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xE2, 2, "SET 4,D",            CPU_P_NONE,  0x10,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xE3, 2, "SET 4,E",            CPU_P_NONE,  0x10,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xE4, 2, "SET 4,H",            CPU_P_NONE,  0x10,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xE5, 2, "SET 4,L",            CPU_P_NONE,  0x10,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xE6, 2, "SET 4,(HL)",         CPU_P_NONE,  0x10,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xE7, 2, "SET 4,A",            CPU_P_NONE,  0x10,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xE8, 2, "SET 5,B",            CPU_P_NONE,  0x20,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xE9, 2, "SET 5,C",            CPU_P_NONE,  0x20,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xEA, 2, "SET 5,D",            CPU_P_NONE,  0x20,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xEB, 2, "SET 5,E",            CPU_P_NONE,  0x20,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xEC, 2, "SET 5,H",            CPU_P_NONE,  0x20,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xED, 2, "SET 5,L",            CPU_P_NONE,  0x20,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xEE, 2, "SET 5,(HL)",         CPU_P_NONE,  0x20,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xEF, 2, "SET 5,A",            CPU_P_NONE,  0x20,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xF0, 2, "SET 6,B",            CPU_P_NONE,  0x40,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xF1, 2, "SET 6,C",            CPU_P_NONE,  0x40,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xF2, 2, "SET 6,D",            CPU_P_NONE,  0x40,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xF3, 2, "SET 6,E",            CPU_P_NONE,  0x40,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xF4, 2, "SET 6,H",            CPU_P_NONE,  0x40,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xF5, 2, "SET 6,L",            CPU_P_NONE,  0x40,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xF6, 2, "SET 6,(HL)",         CPU_P_NONE,  0x40,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xF7, 2, "SET 6,A",            CPU_P_NONE,  0x40,     CPU_R_A,  Cpu_Execute_SET_N_R},
    {0xF8, 2, "SET 7,B",            CPU_P_NONE,  0x80,     CPU_R_B,  Cpu_Execute_SET_N_R},
    {0xF9, 2, "SET 7,C",            CPU_P_NONE,  0x80,     CPU_R_C,  Cpu_Execute_SET_N_R},
    {0xFA, 2, "SET 7,D",            CPU_P_NONE,  0x80,     CPU_R_D,  Cpu_Execute_SET_N_R},
    {0xFB, 2, "SET 7,E",            CPU_P_NONE,  0x80,     CPU_R_E,  Cpu_Execute_SET_N_R},
    {0xFC, 2, "SET 7,H",            CPU_P_NONE,  0x80,     CPU_R_H,  Cpu_Execute_SET_N_R},
    {0xFD, 2, "SET 7,L",            CPU_P_NONE,  0x80,     CPU_R_L,  Cpu_Execute_SET_N_R},
    {0xFE, 2, "SET 7,(HL)",         CPU_P_NONE,  0x80,     CPU_R_HL, Cpu_Execute_SET_N_pRR},
    {0xFF, 2, "SET 7,A",            CPU_P_NONE,  0x80,     CPU_R_A,  Cpu_Execute_SET_N_R}
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

    return data;
}


void Cpu_GetOpcodeInfo(uint16_t addr, char *buffer, int *size)
{
    uint8_t  data = Memory_Read(addr ++);
    Cpu_OpCode_t const * opcode = &Cpu_OpCode[data];

    /* Handle CB prefix */
    if(data == 0xCB)
    {
        data = Memory_Read(addr ++);
        opcode = &Cpu_OpCode_Prefix[data];
    }

    /* Set opcode size */
    *size = opcode->Size;

    /* Genererate opcode string */
    uint8_t ubyte0, ubyte1;
    int8_t sbyte;

    /* @todo implement a way to retrieve string */
    switch(opcode->NameParam)
    {
        case CPU_P_UWORD:
            ubyte0 = Memory_Read(addr ++);
            ubyte1 = Memory_Read(addr ++);
            sprintf(buffer, opcode->Name, CONCAT(ubyte0, ubyte1));
            break;
        case CPU_P_UBYTE:
            ubyte0 = Memory_Read(addr ++);
            sprintf(buffer, opcode->Name, ubyte0);
            break;
        case CPU_P_SBYTE:
            sbyte = Memory_Read(addr ++);
            sprintf(buffer, opcode->Name, sbyte);
            break;
        default:
            /* No parameter CPU_P_NONE */
            sprintf(buffer, opcode->Name);
    }
}


/**
 * OpCode: XXXXXX
 * Size:X, Duration:X, ZNHC Flag:XXXX
 * @todo Delete this function when all opcode implemented
 */
static int Cpu_Execute_Unimplemented(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_INFO("CPU Register:\n");
    DEBUGGER_INFO("#AF: 0x%04x\n", CPU_REG16(CPU_R_AF)->UWord);
    DEBUGGER_INFO("#BC: 0x%04x\n", CPU_REG16(CPU_R_BC)->UWord);
    DEBUGGER_INFO("#DE: 0x%04x\n", CPU_REG16(CPU_R_DE)->UWord);
    DEBUGGER_INFO("#HL: 0x%04x\n", CPU_REG16(CPU_R_HL)->UWord);
    DEBUGGER_INFO("#SP: 0x%04x\n", CPU_REG16(CPU_R_SP)->UWord);
    DEBUGGER_INFO("#PC: 0x%04x\n", CPU_REG16(CPU_R_PC)->UWord);
    DEBUGGER_INFO("Unimplemented Opcode 0x%02x: %s\n", opcode->Value, opcode->Name);
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
 * OpCode: LD RR,NN
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


/**
 * OpCode: DEC RR
 * Size:1, Duration:8, ZNHC Flag:----
 */
static int Cpu_Execute_DEC_RR(Cpu_OpCode_t const * const opcode)
{
    DEBUGGER_TRACE(opcode->Name);

    /* Execute the command */
    uint16_t const data = CPU_REG16(opcode->Param0)->UWord;
    uint16_t const result = data - 1;
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

