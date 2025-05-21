//
// Created by henry on 5/18/2025.
//

#include "detour.h"

#define POWERPC_REGISTERINDEX_R0      0
#define POWERPC_REGISTERINDEX_R1      1
#define POWERPC_REGISTERINDEX_R2      2
#define POWERPC_REGISTERINDEX_R3      3
#define POWERPC_REGISTERINDEX_R4      4
#define POWERPC_REGISTERINDEX_R5      5
#define POWERPC_REGISTERINDEX_R6      6
#define POWERPC_REGISTERINDEX_R7      7
#define POWERPC_REGISTERINDEX_R8      8
#define POWERPC_REGISTERINDEX_R9      9
#define POWERPC_REGISTERINDEX_R10     10
#define POWERPC_REGISTERINDEX_R11     11
#define POWERPC_REGISTERINDEX_R12     12
#define POWERPC_REGISTERINDEX_R13     13
#define POWERPC_REGISTERINDEX_R14     14
#define POWERPC_REGISTERINDEX_R15     15
#define POWERPC_REGISTERINDEX_R16     16
#define POWERPC_REGISTERINDEX_R17     17
#define POWERPC_REGISTERINDEX_R18     18
#define POWERPC_REGISTERINDEX_R19     19
#define POWERPC_REGISTERINDEX_R20     20
#define POWERPC_REGISTERINDEX_R21     21
#define POWERPC_REGISTERINDEX_R22     22
#define POWERPC_REGISTERINDEX_R23     23
#define POWERPC_REGISTERINDEX_R24     24
#define POWERPC_REGISTERINDEX_R25     25
#define POWERPC_REGISTERINDEX_R26     26
#define POWERPC_REGISTERINDEX_R27     27
#define POWERPC_REGISTERINDEX_R28     28
#define POWERPC_REGISTERINDEX_R29     29
#define POWERPC_REGISTERINDEX_R30     30
#define POWERPC_REGISTERINDEX_R31     31
#define POWERPC_REGISTERINDEX_SP      1
#define POWERPC_REGISTERINDEX_RTOC    2

#define MASK_N_BITS(N) ( ( 1 << ( N ) ) - 1 )

#define POWERPC_HI(X) ( ( X >> 16 ) & 0xFFFF )
#define POWERPC_LO(X) ( X & 0xFFFF )

// PowerPC most significant bit is addressed as bit 0 in documentation.
#define POWERPC_BIT32(N) ( 31 - N )

// Opcode is bits 0-5.
// Allowing for op codes ranging from 0-63.
#define POWERPC_OPCODE(OP)       (uint32_t)( OP << 26 )
#define POWERPC_OPCODE_ADDI      POWERPC_OPCODE( 14 )
#define POWERPC_OPCODE_ADDIS     POWERPC_OPCODE( 15 )
#define POWERPC_OPCODE_LIS       POWERPC_OPCODE( 15 )
#define POWERPC_OPCODE_BC        POWERPC_OPCODE( 16 )
#define POWERPC_OPCODE_B         POWERPC_OPCODE( 18 )
#define POWERPC_OPCODE_BCCTR     POWERPC_OPCODE( 19 )
#define POWERPC_OPCODE_ORI       POWERPC_OPCODE( 24 )
#define POWERPC_OPCODE_EXTENDED  POWERPC_OPCODE( 31 ) // Use extended opcodes.
#define POWERPC_OPCODE_STW       POWERPC_OPCODE( 36 )
#define POWERPC_OPCODE_LWZ       POWERPC_OPCODE( 32 )
#define POWERPC_OPCODE_LD        POWERPC_OPCODE( 58 )
#define POWERPC_OPCODE_STD       POWERPC_OPCODE( 62 )
#define POWERPC_OPCODE_MASK      POWERPC_OPCODE( 63 )

#define POWERPC_EXOPCODE(OP)     ( OP << 1 )
#define POWERPC_EXOPCODE_BCCTR   POWERPC_EXOPCODE( 528 )
#define POWERPC_EXOPCODE_MTSPR   POWERPC_EXOPCODE( 467 )

// SPR field is encoded as two 5 bit bitfields.
#define POWERPC_SPR(SPR) (uint32_t)( ( ( SPR & 0x1F ) << 5 ) | ( ( SPR >> 5 ) & 0x1F ) )

// Instruction helpers.
// rD - Destination register.
// rS - Source register.
// rA/rB - Register inputs.
// SPR - Special purpose register.
// UIMM/SIMM - Unsigned/signed immediate.
#define POWERPC_ADDI(rD, rA, SIMM)  (uint32_t)( POWERPC_OPCODE_ADDI | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_ADDIS(rD, rA, SIMM) (uint32_t)( POWERPC_OPCODE_ADDIS | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_LIS(rD, SIMM)       POWERPC_ADDIS( rD, 0, SIMM ) // Mnemonic for addis %rD, 0, SIMM
#define POWERPC_LI(rD, SIMM)        POWERPC_ADDI( rD, 0, SIMM )  // Mnemonic for addi %rD, 0, SIMM
#define POWERPC_MTSPR(SPR, rS)      (uint32_t)( POWERPC_OPCODE_EXTENDED | ( rS << POWERPC_BIT32( 10 ) ) | ( POWERPC_SPR( SPR ) << POWERPC_BIT32( 20 ) ) | POWERPC_EXOPCODE_MTSPR )
#define POWERPC_MTCTR(rS)           POWERPC_MTSPR( 9, rS ) // Mnemonic for mtspr 9, rS
#define POWERPC_ORI(rS, rA, UIMM)   (uint32_t)( POWERPC_OPCODE_ORI | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | UIMM )
#define POWERPC_BCCTR(BO, BI, LK)   (uint32_t)( POWERPC_OPCODE_BCCTR | ( BO << POWERPC_BIT32( 10 ) ) | ( BI << POWERPC_BIT32( 15 ) ) | ( LK & 1 ) | POWERPC_EXOPCODE_BCCTR )
#define POWERPC_STD(rS, DS, rA)     (uint32_t)( POWERPC_OPCODE_STD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (int16_t)DS & 0xFFFF ) )
#define POWERPC_LD(rS, DS, rA)      (uint32_t)( POWERPC_OPCODE_LD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (int16_t)DS & 0xFFFF ) )

// Branch related fields.
#define POWERPC_BRANCH_LINKED    1
#define POWERPC_BRANCH_ABSOLUTE  2
#define POWERPC_BRANCH_TYPE_MASK ( POWERPC_BRANCH_LINKED | POWERPC_BRANCH_ABSOLUTE )

#define POWERPC_BRANCH_OPTIONS_ALWAYS ( 20 )

