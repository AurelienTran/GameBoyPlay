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

#include <stdint.h>


/******************************************************/
/* Macro                                              */
/******************************************************/

#define BYTE_BY_WORD    2
#define Bus_Read(Addr)  0 /** @todo define the function */


/******************************************************/
/* Type                                               */
/******************************************************/

/** All purpose 16 bit register type */
typedef union tagReg16_t
{
    uint16_t UWord;                 /**< Unsigned 16 bit access */
    int16_t  SWord;                 /**< Signed 16 bit access */
    uint8_t  UByte[BYTE_BY_WORD];   /**< Unsigned 8 bit access */
    int8_t   SByte[BYTE_BY_WORD];   /**< Signed 8 bit access */
} Reg16_t;

/** Z80 Register name */
typedef enum tagZ80_RegName_e
{
    /* 16 bit Register Name */
    Z80_AF = 0,     /**< Accumulator and Flag register */
    Z80_BC,         /**< All purpose register */
    Z80_DE,         /**< All purpose register */
    Z80_HL,         /**< All purpose register */
    Z80_SP,         /**< Stack Pointer register */
    Z80_PC,         /**< Program Pointer register */
    Z80_REG_NUM,    /**< Number of internal register */

    /* 8 bit Register Name */
    Z80_A = 0,      /**< Accumulator register */
    Z80_F,          /**< Flag register */
    Z80_B,          /**< All purpose register */
    Z80_C,          /**< All purpose register */
    Z80_D,          /**< All purpose register */
    Z80_E,          /**< All purpose register */
    Z80_H,          /**< All purpose register */
    Z80_L,          /**< All purpose register */

    /* Special location */
    Z80_NONE,       /**< Unused */
    Z80_D8,         /**< Immediate 8 bit data */
    Z80_D16,        /**< Immediate 16 bit data */
    Z80_A8,         /**< 8 bit unsigned data where 0xFF00 are added */
    Z80_R8          /**< 8 bit signed data which are added to program counter */
} Z80_RegName_e;

/** Z80 State */
typedef struct tagZ80_State_t
{
    Reg16_t Reg[Z80_REG_NUM];   /**< Internal Register */
} Z80_State_t;

/* Forward declaration for Z80_ExecuteCallback_t definition */
typedef struct tagZ80_OpCode_t Z80_OpCode_t;

/**
 * Callback to Execute an OpCode
 * @param dst Destination register
 * @param src Source register
 */
typedef void (*Z80_ExecuteCallback_t)(Z80_OpCode_t* opcode);

/** Z80 OpCode information */
typedef struct tagZ80_OpCode_t
{
    uint32_t Value;         /**< OpCode Value */
    uint32_t Size;          /**< OpCode Byte size */
    char* Name;             /**< OpCode Name */
    Z80_RegName_e RegDst;   /**< OpCode result register */
    Z80_RegName_e RegSrc;   /**< OpCode source register */
    Z80_ExecuteCallback_t ExecuteCallback; /**< OpCode execution callback */
} Z80_OpCode_t;


/******************************************************/
/* Prototype                                          */
/******************************************************/

/** Initialize Z80 */
extern void Z80_Initialize(void);

/* Misc/Contoll OpCode Callback */
static void Z80_Execute_DoNothing(Z80_OpCode_t* opcode);
static void Z80_Execute_NOP(Z80_OpCode_t* opcode);

/* Load/Store/Move OpCode Callback */
static void Z80_Execute_LD_Reg8_Reg8(Z80_OpCode_t* opcode);
static void Z80_Execute_LD_Reg8_D8(Z80_OpCode_t* opcode);
static void Z80_Execute_LD_Reg16_D16(Z80_OpCode_t* opcode);


/******************************************************/
/* Variable                                           */
/******************************************************/

/** Z80 State */
static Z80_State_t Z80_State;

/** Callback table for each OpCode */
static Z80_OpCode_t const Z80_OpCode[] =
{
    {0x00, 1, "NOP",            Z80_NONE,   Z80_NONE,   Z80_Execute_NOP},
    {0x01, 3, "LD BC, d16",     Z80_BC,     Z80_D16,    Z80_Execute_LD_Reg16_D16},

    {0x06, 2, "LD B, d8",       Z80_B,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x0E, 2, "LD C, d8",       Z80_C,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x11, 3, "LD DE, d16",     Z80_DE,     Z80_D16,    Z80_Execute_LD_Reg16_D16},

    {0x16, 2, "LD D, d8",       Z80_D,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x1E, 2, "LD E, d8",       Z80_E,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x21, 3, "LD HL, d16",     Z80_HL,     Z80_D16,    Z80_Execute_LD_Reg16_D16},

    {0x26, 2, "LD H, d8",       Z80_H,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x2E, 2, "LD L, d8",       Z80_L,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x31, 3, "LD SP, d16",     Z80_SP,     Z80_D16,    Z80_Execute_LD_Reg16_D16},

    {0x3E, 2, "LD A, d8",       Z80_A,      Z80_D8,     Z80_Execute_LD_Reg8_D8},

    {0x40, 2, "LD B, B",        Z80_B,      Z80_B,      Z80_Execute_LD_Reg8_Reg8},
    {0x41, 2, "LD B, C",        Z80_B,      Z80_C,      Z80_Execute_LD_Reg8_Reg8},
    {0x42, 2, "LD B, D",        Z80_B,      Z80_D,      Z80_Execute_LD_Reg8_Reg8},
    {0x43, 2, "LD B, E",        Z80_B,      Z80_E,      Z80_Execute_LD_Reg8_Reg8},
    {0x44, 2, "LD B, H",        Z80_B,      Z80_H,      Z80_Execute_LD_Reg8_Reg8},
    {0x45, 2, "LD B, L",        Z80_B,      Z80_L,      Z80_Execute_LD_Reg8_Reg8},

    {0x47, 2, "LD B, A",        Z80_B,      Z80_A,      Z80_Execute_LD_Reg8_Reg8},

    {0x50, 2, "LD D, B",        Z80_D,      Z80_B,      Z80_Execute_LD_Reg8_Reg8},
    {0x51, 2, "LD D, C",        Z80_D,      Z80_C,      Z80_Execute_LD_Reg8_Reg8},
    {0x52, 2, "LD D, D",        Z80_D,      Z80_D,      Z80_Execute_LD_Reg8_Reg8},
    {0x53, 2, "LD D, E",        Z80_D,      Z80_E,      Z80_Execute_LD_Reg8_Reg8},
    {0x54, 2, "LD D, H",        Z80_D,      Z80_H,      Z80_Execute_LD_Reg8_Reg8},
    {0x55, 2, "LD D, L",        Z80_D,      Z80_L,      Z80_Execute_LD_Reg8_Reg8},

    {0x57, 2, "LD D, A",        Z80_D,      Z80_A,      Z80_Execute_LD_Reg8_Reg8},

    {0x60, 2, "LD H, B",        Z80_H,      Z80_B,      Z80_Execute_LD_Reg8_Reg8},
    {0x61, 2, "LD H, C",        Z80_H,      Z80_C,      Z80_Execute_LD_Reg8_Reg8},
    {0x62, 2, "LD H, D",        Z80_H,      Z80_D,      Z80_Execute_LD_Reg8_Reg8},
    {0x63, 2, "LD H, E",        Z80_H,      Z80_E,      Z80_Execute_LD_Reg8_Reg8},
    {0x64, 2, "LD H, H",        Z80_H,      Z80_H,      Z80_Execute_LD_Reg8_Reg8},
    {0x65, 2, "LD H, L",        Z80_H,      Z80_L,      Z80_Execute_LD_Reg8_Reg8},

    {0x67, 2, "LD H, A",        Z80_H,      Z80_A,      Z80_Execute_LD_Reg8_Reg8},


    {0xD3, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},

    {0xDB, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},

    {0xDD, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
    
    {0xE3, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
    {0xE4, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},

    {0xEB, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
    {0xEC, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
    {0xED, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},

    {0xF4, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},

    {0xFC, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
    {0xFD, 1, "-",              Z80_NONE,   Z80_NONE,   Z80_Execute_DoNothing},
};


/******************************************************/
/* Function                                           */
/******************************************************/

void Z80_Initialize(void)
{
    for(int i=0; i<Z80_REG_NUM; i++)
    {
        Z80_State.Reg[i].UWord = 0;
    }
}


/**
 * OpCode which lock up the CPU
 */
static void Z80_Execute_DoNothing(Z80_OpCode_t* opcode)
{
    /**
     * This opcode lock up the cpu
     * If any register does not change, this opcode
     * will be called over and over simulating a lock up
     */
}


/**
 * OpCode: NOP
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_NOP(Z80_OpCode_t* opcode)
{
    Z80_State.Reg[Z80_PC].UWord += 1;
}


/**
 * OpCode: LD r,r
 * where r can be A/B/C/D/E/H/L
 * Size:1, Duration:4, ZNHC Flag:----
 */
static void Z80_Execute_LD_Reg8_Reg8(Z80_OpCode_t* opcode)
{
    uint8_t const d8 = Z80_State.Reg[opcode->RegSrc / BYTE_BY_WORD].UByte[opcode->RegSrc % BYTE_BY_WORD];
    Z80_State.Reg[opcode->RegDst / BYTE_BY_WORD].UByte[opcode->RegDst % BYTE_BY_WORD] = d8;
    Z80_State.Reg[Z80_PC].UWord += 1;
}


/**
 * OpCode: LD r,d8
 * where r can be A/B/C/D/E/H/L
 * Size:2, Duration:8, ZNHC Flag:----
 */
static void Z80_Execute_LD_Reg8_D8(Z80_OpCode_t* opcode)
{
    uint8_t const d8 = Bus_Read(Z80_State.Reg[Z80_PC] + 1);
    Z80_State.Reg[opcode->RegDst / BYTE_BY_WORD].UByte[opcode->RegDst % BYTE_BY_WORD] = d8;
    Z80_State.Reg[Z80_PC].UWord += 2;
}


/**
 * OpCode: LD rr,d16
 * where rr can be BC/DE/HL/SP
 * Size:3, Duration:12, ZNHC Flag:----
 */
static void Z80_Execute_LD_Reg16_D16(Z80_OpCode_t* opcode)
{
    Z80_State.Reg[opcode->RegDst].UByte[0] = Bus_Read(Z80_State.Reg[Z80_PC] + 1);
    Z80_State.Reg[opcode->RegDst].UByte[1] = Bus_Read(Z80_State.Reg[Z80_PC] + 2);
    Z80_State.Reg[Z80_PC].UWord += 3;
}


#if 0

/**
 * OpCode: LD (BC),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (BC),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC BC
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC BC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC B
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC B
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLCA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RLCA(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (a16),SP
 * Size:3, Duration:20, ZNHC Flag:----
 */
void Z80_Execute_LD (a16),SP(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,BC
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,BC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(BC)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(BC)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC BC
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC BC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC C
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC C
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRCA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RRCA(Z80_OpCode_t* opcode)
{
    /** @todo */
}

1x 
/**
 * OpCode: STOP 0
 * Size:2, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_STOP 0(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (DE),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (DE),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC DE
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC DE(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC D
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC D
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RLA(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JR r8
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_JR r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,DE
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,DE(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(DE)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(DE)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC DE
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC DE(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC E
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC E
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RRA(Z80_OpCode_t* opcode)
{
    /** @todo */
}

2x 
/**
 * OpCode: JR NZ,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR NZ,r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL+),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL+),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC H
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC H
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DAA
 * Size:1, Duration:4, ZNHC Flag:Z-0C
 */
void Z80_Execute_DAA(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JR Z,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR Z,r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,HL
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL+)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL+)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC L
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC L
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CPL
 * Size:1, Duration:4, ZNHC Flag:-11-
 */
void Z80_Execute_CPL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

3x 
/**
 * OpCode: JR NC,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR NC,r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL-),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL-),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC SP
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC SP(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC (HL)
 * Size:1, Duration:12, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC (HL)
 * Size:1, Duration:12, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),d8
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SCF
 * Size:1, Duration:4, ZNHC Flag:-001
 */
void Z80_Execute_SCF(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JR C,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR C,r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,SP
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,SP(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL-)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL-)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC SP
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC SP(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: INC A
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DEC A
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CCF
 * Size:1, Duration:4, ZNHC Flag:-00C
 */
void Z80_Execute_CCF(Z80_OpCode_t* opcode)
{
    /** @todo */
}

4x 

/**
 * OpCode: LD B,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD B,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD C,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD C,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

5x 
/**
 * OpCode: LD D,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD D,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD E,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD E,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

6x 
/**
 * OpCode: LD H,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD H,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD L,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD L,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

7x 
/**
 * OpCode: LD (HL),B
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),C
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),D
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),E
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),H
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),L
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: HALT
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_HALT(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

8x 
/**
 * OpCode: ADD A,B
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,C
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,D
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,E
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,H
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,L
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,A
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,B
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,C
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,D
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,E
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,H
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,L
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,A
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

9x 
/**
 * OpCode: SUB B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB (HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Ax 
/**
 * OpCode: AND B
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND C
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND D
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND E
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND H
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND L
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND (HL)
 * Size:1, Duration:8, ZNHC Flag:Z010
 */
void Z80_Execute_AND (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND A
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR B
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR C
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR D
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR E
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR H
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR L
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR (HL)
 * Size:1, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_XOR (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR A
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Bx 
/**
 * OpCode: OR B
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR C
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR D
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR E
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR H
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR L
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR (HL)
 * Size:1, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_OR (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR A
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP (HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Cx 
/**
 * OpCode: RET NZ
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET NZ(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: POP BC
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP BC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP NZ,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP NZ,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP a16
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_JP a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CALL NZ,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL NZ,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: PUSH BC
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH BC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD A,d8
 * Size:2, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 00H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 00H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RET Z
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET Z(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RET
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RET(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP Z,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP Z,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: PREFIX CB
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_PREFIX CB(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CALL Z,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL Z,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CALL a16
 * Size:3, Duration:24, ZNHC Flag:----
 */
void Z80_Execute_CALL a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADC A,d8
 * Size:2, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 08H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 08H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Dx 
/**
 * OpCode: RET NC
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET NC(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: POP DE
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP DE(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP NC,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP NC,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CALL NC,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL NC,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: PUSH DE
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH DE(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SUB d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 10H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 10H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RET C
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RETI
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RETI(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP C,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP C,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CALL C,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL C,a16(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SBC A,d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 18H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 18H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Ex 
/**
 * OpCode: LDH (a8),A
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LDH (a8),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: POP HL
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (C),A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (C),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: PUSH HL
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: AND d8
 * Size:2, Duration:8, ZNHC Flag:Z010
 */
void Z80_Execute_AND d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 20H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 20H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: ADD SP,r8
 * Size:2, Duration:16, ZNHC Flag:00HC
 */
void Z80_Execute_ADD SP,r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: JP (HL)
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_JP (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD (a16),A
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_LD (a16),A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: XOR d8
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_XOR d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 28H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 28H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Fx 
/**
 * OpCode: LDH A,(a8)
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LDH A,(a8)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: POP AF
 * Size:1, Duration:12, ZNHC Flag:ZNHC
 */
void Z80_Execute_POP AF(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(C)
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(C)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: DI
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_DI(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: PUSH AF
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH AF(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: OR d8
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_OR d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 30H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 30H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD HL,SP+r8
 * Size:2, Duration:12, ZNHC Flag:00HC
 */
void Z80_Execute_LD HL,SP+r8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD SP,HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD SP,HL(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: LD A,(a16)
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_LD A,(a16)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: EI
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_EI(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: CP d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP d8(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RST 38H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 38H(Z80_OpCode_t* opcode)
{
    /** @todo */
}


0x 
/**
 * OpCode: RLC B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RLC A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RRC A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

1x 
/**
 * OpCode: RL B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RL (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RL A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RR (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RR A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

2x 
/**
 * OpCode: SLA B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SLA A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA B
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA C
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA D
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA E
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA H
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA L
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA (HL)
 * Size:2, Duration:16, ZNHC Flag:Z000
 */
void Z80_Execute_SRA (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRA A
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

3x 
/**
 * OpCode: SWAP B
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP C
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP D
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP E
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP H
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP L
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP (HL)
 * Size:2, Duration:16, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SWAP A
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL (HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SRL A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

4x 
/**
 * OpCode: BIT 0,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

5x 
/**
 * OpCode: BIT 2,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

6x 
/**
 * OpCode: BIT 4,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

7x 
/**
 * OpCode: BIT 6,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

8x 
/**
 * OpCode: RES 0,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 0,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 0,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 1,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 1,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

9x 
/**
 * OpCode: RES 2,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 2,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 2,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 3,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 3,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Ax 
/**
 * OpCode: RES 4,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 4,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 4,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 5,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 5,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Bx 
/**
 * OpCode: RES 6,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 6,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 6,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 7,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: RES 7,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Cx 
/**
 * OpCode: SET 0,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 0,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 0,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 1,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 1,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Dx 
/**
 * OpCode: SET 2,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 2,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 2,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 3,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 3,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Ex 
/**
 * OpCode: SET 4,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 4,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 4,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 5,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 5,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

Fx 
/**
 * OpCode: SET 6,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 6,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 6,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,B(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,C(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,D(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,E(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,H(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,L(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 7,(HL)(Z80_OpCode_t* opcode)
{
    /** @todo */
}

/**
 * OpCode: SET 7,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,A(Z80_OpCode_t* opcode)
{
    /** @todo */
}

#endif

