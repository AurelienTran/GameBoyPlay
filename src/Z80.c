#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Z80 Internal register
 */
typedef struct tagZ80_Register_t
{
    uint8_t A; /**< Accumulator register */
    uint8_t F; /**< Flag register */
    uint8_t B; /**< Higher byte of BC register */
    uint8_t C; /**< Lower  byte of BC register */
    uint8_t D; /**< Higher byte of DE register */
    uint8_t E; /**< Lower  byte of DE register */
    uint8_t H; /**< Higher byte of HL register */
    uint8_t L; /**< Lower  byte of HL register */

    uint16_t SP; /**< Stack Pointer register */
    uint16_t PC; /**< Program Pointer register */
} Z80_Register_t;

/* Callback to Execute an OpCode */
typedef void (*Z80_ExecuteOpCodeCallback_t)(Z80_Register_t* reg);

/* Z80 OpCode information */
typedef struct tagZ80_OpCode_t
{
    uint32_t Value;
    uint32_t Size;
    char* Name;
    Z80_ExecuteOpCodeCallback_t ExecuteCallback;
} Z80_OpCode_t;


void Z80_Execute_Illegal(Z80_Register_t* reg)
{
    printf("Z80: Illegal instruction\n");
    exit(1);
}

void Z80_Execute_NOP(Z80_Register_t* reg);

static Z80_OpCode_t const Z80_OpCode[] =
{
    {0x00, 1, "NOP",        Z80_Execute_NOP},
    {0x01, 3, "LD BC, d16", Z80_Execute_Illegal} // TODO
};


/**
 * OpCode: NOP
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_NOP(Z80_Register_t* reg)
{
    reg->PC += 1;
}

#if 0
/**
 * OpCode: LD BC,d16
 * Size:3, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD BC,d16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (BC),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (BC),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC BC
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC BC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC B
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC B
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD B,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLCA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RLCA(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (a16),SP
 * Size:3, Duration:20, ZNHC Flag:----
 */
void Z80_Execute_LD (a16),SP(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,BC
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,BC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(BC)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(BC)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC BC
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC BC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC C
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC C
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD C,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRCA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RRCA(Z80_Register_t* reg)
{
    /** @todo */
}

1x 
/**
 * OpCode: STOP 0
 * Size:2, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_STOP 0(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD DE,d16
 * Size:3, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD DE,d16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (DE),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (DE),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC DE
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC DE(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC D
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC D
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD D,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RLA(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JR r8
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_JR r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,DE
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,DE(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(DE)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(DE)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC DE
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC DE(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC E
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC E
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD E,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRA
 * Size:1, Duration:4, ZNHC Flag:000C
 */
void Z80_Execute_RRA(Z80_Register_t* reg)
{
    /** @todo */
}

2x 
/**
 * OpCode: JR NZ,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR NZ,r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD HL,d16
 * Size:3, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD HL,d16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL+),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL+),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC H
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC H
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD H,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DAA
 * Size:1, Duration:4, ZNHC Flag:Z-0C
 */
void Z80_Execute_DAA(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JR Z,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR Z,r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,HL
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL+)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL+)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC L
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC L
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD L,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CPL
 * Size:1, Duration:4, ZNHC Flag:-11-
 */
void Z80_Execute_CPL(Z80_Register_t* reg)
{
    /** @todo */
}

3x 
/**
 * OpCode: JR NC,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR NC,r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD SP,d16
 * Size:3, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD SP,d16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL-),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL-),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC SP
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_INC SP(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC (HL)
 * Size:1, Duration:12, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC (HL)
 * Size:1, Duration:12, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),d8
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SCF
 * Size:1, Duration:4, ZNHC Flag:-001
 */
void Z80_Execute_SCF(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JR C,r8
 * Size:2, Duration:12/8, ZNHC Flag:----
 */
void Z80_Execute_JR C,r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD HL,SP
 * Size:1, Duration:8, ZNHC Flag:-0HC
 */
void Z80_Execute_ADD HL,SP(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL-)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL-)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC SP
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_DEC SP(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: INC A
 * Size:1, Duration:4, ZNHC Flag:Z0H-
 */
void Z80_Execute_INC A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DEC A
 * Size:1, Duration:4, ZNHC Flag:Z1H-
 */
void Z80_Execute_DEC A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,d8
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CCF
 * Size:1, Duration:4, ZNHC Flag:-00C
 */
void Z80_Execute_CCF(Z80_Register_t* reg)
{
    /** @todo */
}

4x 
/**
 * OpCode: LD B,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD B,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD B,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD B,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD C,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD C,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD C,A(Z80_Register_t* reg)
{
    /** @todo */
}

5x 
/**
 * OpCode: LD D,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD D,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD D,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD D,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD E,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD E,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD E,A(Z80_Register_t* reg)
{
    /** @todo */
}

6x 
/**
 * OpCode: LD H,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD H,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD H,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD H,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD L,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD L,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD L,A(Z80_Register_t* reg)
{
    /** @todo */
}

7x 
/**
 * OpCode: LD (HL),B
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),C
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),D
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),E
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),H
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),L
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: HALT
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_HALT(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (HL),A
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (HL),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,B
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,C
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,D
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,E
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,H
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,L
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(HL)
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,A
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_LD A,A(Z80_Register_t* reg)
{
    /** @todo */
}

8x 
/**
 * OpCode: ADD A,B
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,C
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,D
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,E
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,H
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,L
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,A
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,B
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,C
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,D
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,E
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,H
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,L
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,A
 * Size:1, Duration:4, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,A(Z80_Register_t* reg)
{
    /** @todo */
}

9x 
/**
 * OpCode: SUB B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB (HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,(HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,A(Z80_Register_t* reg)
{
    /** @todo */
}

Ax 
/**
 * OpCode: AND B
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND C
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND D
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND E
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND H
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND L
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND (HL)
 * Size:1, Duration:8, ZNHC Flag:Z010
 */
void Z80_Execute_AND (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND A
 * Size:1, Duration:4, ZNHC Flag:Z010
 */
void Z80_Execute_AND A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR B
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR C
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR D
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR E
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR H
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR L
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR (HL)
 * Size:1, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_XOR (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR A
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_XOR A(Z80_Register_t* reg)
{
    /** @todo */
}

Bx 
/**
 * OpCode: OR B
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR C
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR D
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR E
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR H
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR L
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR (HL)
 * Size:1, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_OR (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR A
 * Size:1, Duration:4, ZNHC Flag:Z000
 */
void Z80_Execute_OR A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP B
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP C
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP D
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP E
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP H
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP L
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP (HL)
 * Size:1, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP A
 * Size:1, Duration:4, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP A(Z80_Register_t* reg)
{
    /** @todo */
}

Cx 
/**
 * OpCode: RET NZ
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET NZ(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: POP BC
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP BC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP NZ,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP NZ,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP a16
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_JP a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CALL NZ,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL NZ,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: PUSH BC
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH BC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD A,d8
 * Size:2, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADD A,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 00H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 00H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RET Z
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET Z(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RET
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RET(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP Z,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP Z,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: PREFIX CB
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_PREFIX CB(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CALL Z,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL Z,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CALL a16
 * Size:3, Duration:24, ZNHC Flag:----
 */
void Z80_Execute_CALL a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADC A,d8
 * Size:2, Duration:8, ZNHC Flag:Z0HC
 */
void Z80_Execute_ADC A,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 08H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 08H(Z80_Register_t* reg)
{
    /** @todo */
}

Dx 
/**
 * OpCode: RET NC
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET NC(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: POP DE
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP DE(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP NC,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP NC,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CALL NC,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL NC,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: PUSH DE
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH DE(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SUB d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SUB d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 10H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 10H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RET C
 * Size:1, Duration:20/8, ZNHC Flag:----
 */
void Z80_Execute_RET C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RETI
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RETI(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP C,a16
 * Size:3, Duration:16/12, ZNHC Flag:----
 */
void Z80_Execute_JP C,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CALL C,a16
 * Size:3, Duration:24/12, ZNHC Flag:----
 */
void Z80_Execute_CALL C,a16(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SBC A,d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_SBC A,d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 18H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 18H(Z80_Register_t* reg)
{
    /** @todo */
}

Ex 
/**
 * OpCode: LDH (a8),A
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LDH (a8),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: POP HL
 * Size:1, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_POP HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (C),A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD (C),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: PUSH HL
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: AND d8
 * Size:2, Duration:8, ZNHC Flag:Z010
 */
void Z80_Execute_AND d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 20H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 20H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: ADD SP,r8
 * Size:2, Duration:16, ZNHC Flag:00HC
 */
void Z80_Execute_ADD SP,r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: JP (HL)
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_JP (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD (a16),A
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_LD (a16),A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: XOR d8
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_XOR d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 28H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 28H(Z80_Register_t* reg)
{
    /** @todo */
}

Fx 
/**
 * OpCode: LDH A,(a8)
 * Size:2, Duration:12, ZNHC Flag:----
 */
void Z80_Execute_LDH A,(a8)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: POP AF
 * Size:1, Duration:12, ZNHC Flag:ZNHC
 */
void Z80_Execute_POP AF(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(C)
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD A,(C)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: DI
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_DI(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: PUSH AF
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_PUSH AF(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: OR d8
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_OR d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 30H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 30H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD HL,SP+r8
 * Size:2, Duration:12, ZNHC Flag:00HC
 */
void Z80_Execute_LD HL,SP+r8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD SP,HL
 * Size:1, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_LD SP,HL(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: LD A,(a16)
 * Size:3, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_LD A,(a16)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: EI
 * Size:1, Duration:4, ZNHC Flag:----
 */
void Z80_Execute_EI(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: CP d8
 * Size:2, Duration:8, ZNHC Flag:Z1HC
 */
void Z80_Execute_CP d8(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RST 38H
 * Size:1, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RST 38H(Z80_Register_t* reg)
{
    /** @todo */
}


0x 
/**
 * OpCode: RLC B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RLC A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RLC A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RRC A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RRC A(Z80_Register_t* reg)
{
    /** @todo */
}

1x 
/**
 * OpCode: RL B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RL (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RL A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RL A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_RR (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RR A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_RR A(Z80_Register_t* reg)
{
    /** @todo */
}

2x 
/**
 * OpCode: SLA B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SLA A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SLA A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA B
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA C
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA D
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA E
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA H
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA L
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA (HL)
 * Size:2, Duration:16, ZNHC Flag:Z000
 */
void Z80_Execute_SRA (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRA A
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SRA A(Z80_Register_t* reg)
{
    /** @todo */
}

3x 
/**
 * OpCode: SWAP B
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP C
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP D
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP E
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP H
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP L
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP (HL)
 * Size:2, Duration:16, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SWAP A
 * Size:2, Duration:8, ZNHC Flag:Z000
 */
void Z80_Execute_SWAP A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL B
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL C
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL D
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL E
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL H
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL L
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL (HL)
 * Size:2, Duration:16, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL (HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SRL A
 * Size:2, Duration:8, ZNHC Flag:Z00C
 */
void Z80_Execute_SRL A(Z80_Register_t* reg)
{
    /** @todo */
}

4x 
/**
 * OpCode: BIT 0,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 0,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 0,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 1,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 1,A(Z80_Register_t* reg)
{
    /** @todo */
}

5x 
/**
 * OpCode: BIT 2,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 2,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 2,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 3,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 3,A(Z80_Register_t* reg)
{
    /** @todo */
}

6x 
/**
 * OpCode: BIT 4,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 4,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 4,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 5,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 5,A(Z80_Register_t* reg)
{
    /** @todo */
}

7x 
/**
 * OpCode: BIT 6,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 6,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 6,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,B
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,C
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,D
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,E
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,H
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,L
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: BIT 7,A
 * Size:2, Duration:8, ZNHC Flag:Z01-
 */
void Z80_Execute_BIT 7,A(Z80_Register_t* reg)
{
    /** @todo */
}

8x 
/**
 * OpCode: RES 0,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 0,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 0,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 0,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 1,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 1,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 1,A(Z80_Register_t* reg)
{
    /** @todo */
}

9x 
/**
 * OpCode: RES 2,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 2,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 2,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 2,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 3,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 3,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 3,A(Z80_Register_t* reg)
{
    /** @todo */
}

Ax 
/**
 * OpCode: RES 4,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 4,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 4,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 4,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 5,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 5,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 5,A(Z80_Register_t* reg)
{
    /** @todo */
}

Bx 
/**
 * OpCode: RES 6,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 6,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 6,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 6,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_RES 7,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: RES 7,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_RES 7,A(Z80_Register_t* reg)
{
    /** @todo */
}

Cx 
/**
 * OpCode: SET 0,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 0,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 0,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 0,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 1,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 1,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 1,A(Z80_Register_t* reg)
{
    /** @todo */
}

Dx 
/**
 * OpCode: SET 2,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 2,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 2,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 2,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 3,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 3,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 3,A(Z80_Register_t* reg)
{
    /** @todo */
}

Ex 
/**
 * OpCode: SET 4,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 4,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 4,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 4,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 5,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 5,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 5,A(Z80_Register_t* reg)
{
    /** @todo */
}

Fx 
/**
 * OpCode: SET 6,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 6,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 6,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 6,A(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,B
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,B(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,C
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,C(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,D
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,D(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,E
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,E(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,H
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,H(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,L
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,L(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,(HL)
 * Size:2, Duration:16, ZNHC Flag:----
 */
void Z80_Execute_SET 7,(HL)(Z80_Register_t* reg)
{
    /** @todo */
}

/**
 * OpCode: SET 7,A
 * Size:2, Duration:8, ZNHC Flag:----
 */
void Z80_Execute_SET 7,A(Z80_Register_t* reg)
{
    /** @todo */
}

#endif

