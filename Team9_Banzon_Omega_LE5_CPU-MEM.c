/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Lab Exercise 5: The Memory 
Date: April 21, 2025
*/

#include <stdio.h>
#include <stdlib.h>

// Global Registers and Control Signals
unsigned char CONTROL;
unsigned char FLAGS = 0;
unsigned char BUS = 0;
unsigned char MBR = 0;
unsigned char MAR = 0;
int ACC = 0;

// External Control Signals
unsigned char IOM = 0;
unsigned char RW = 0;
unsigned char OE = 0;
unsigned short ADDR = 0x000;

// TRACS Main Memory Chips
long A1[32], A2[32], A3[32], A4[32], A5[32], A6[32], A7[32], A8[32];
long B1[32], B2[32], B3[32], B4[32], B5[32], B6[32], B7[32], B8[32];

// Flag Bit Positions
#define ZF 0x01 // Zero Flag (bit 0)
#define CF 0x02 // Carry Flag (bit 1)
#define SF 0x40 // Sign Flag (bit 6)
#define OF 0x80 // Overflow Flag (bit 7)

// Instruction Set
#define ADD 0x1E
#define SUB 0x1D
#define MUL 0x1B
#define AND 0x1A
#define OR 0x19
#define NOT 0x18
#define XOR 0x17
#define SHL 0x16
#define SHR 0x15

#define WM 0x01
#define RM 0x02
#define WB 0x06
#define WACC 0x09
#define RACC 0x0B

#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11

#define EOP 0x1F

// ======== Function Prototypes ========
void MainMemory();
void printBin(int num, int bits);
void setFlags(int result);
int ALU();
void CU(unsigned char inst_code);
unsigned int boothMultiplication(unsigned char op1, unsigned char op2);

// ======== MAIN MEMORY START ========
void MainMemory()
{
    unsigned short col = ADDR & 0x001F;
    unsigned short row = (ADDR >> 5) & 0x001F;
    unsigned short cs = (ADDR >> 10) & 0x0001;

    if (OE && RW && IOM)
    {
        // WRITE
        unsigned char data = BUS;
        if (cs == 0)
        {
            if (data & 0x01)
                A1[row] |= (1UL << col);
            else
                A1[row] &= ~(1UL << col);
            if (data & 0x02)
                A2[row] |= (1UL << col);
            else
                A2[row] &= ~(1UL << col);
            if (data & 0x04)
                A3[row] |= (1UL << col);
            else
                A3[row] &= ~(1UL << col);
            if (data & 0x08)
                A4[row] |= (1UL << col);
            else
                A4[row] &= ~(1UL << col);
            if (data & 0x10)
                A5[row] |= (1UL << col);
            else
                A5[row] &= ~(1UL << col);
            if (data & 0x20)
                A6[row] |= (1UL << col);
            else
                A6[row] &= ~(1UL << col);
            if (data & 0x40)
                A7[row] |= (1UL << col);
            else
                A7[row] &= ~(1UL << col);
            if (data & 0x80)
                A8[row] |= (1UL << col);
            else
                A8[row] &= ~(1UL << col);
        }
        else
        {
            if (data & 0x01)
                B1[row] |= (1UL << col);
            else
                B1[row] &= ~(1UL << col);
            if (data & 0x02)
                B2[row] |= (1UL << col);
            else
                B2[row] &= ~(1UL << col);
            if (data & 0x04)
                B3[row] |= (1UL << col);
            else
                B3[row] &= ~(1UL << col);
            if (data & 0x08)
                B4[row] |= (1UL << col);
            else
                B4[row] &= ~(1UL << col);
            if (data & 0x10)
                B5[row] |= (1UL << col);
            else
                B5[row] &= ~(1UL << col);
            if (data & 0x20)
                B6[row] |= (1UL << col);
            else
                B6[row] &= ~(1UL << col);
            if (data & 0x40)
                B7[row] |= (1UL << col);
            else
                B7[row] &= ~(1UL << col);
            if (data & 0x80)
                B8[row] |= (1UL << col);
            else
                B8[row] &= ~(1UL << col);
        }
    }
    else if (OE && !RW && IOM)
    {
        // READ
        unsigned char data = 0;
        if (cs == 0)
        {
            if (A1[row] & (1UL << col))
                data |= 0x01;
            if (A2[row] & (1UL << col))
                data |= 0x02;
            if (A3[row] & (1UL << col))
                data |= 0x04;
            if (A4[row] & (1UL << col))
                data |= 0x08;
            if (A5[row] & (1UL << col))
                data |= 0x10;
            if (A6[row] & (1UL << col))
                data |= 0x20;
            if (A7[row] & (1UL << col))
                data |= 0x40;
            if (A8[row] & (1UL << col))
                data |= 0x80;
        }
        else
        {
            if (B1[row] & (1UL << col))
                data |= 0x01;
            if (B2[row] & (1UL << col))
                data |= 0x02;
            if (B3[row] & (1UL << col))
                data |= 0x04;
            if (B4[row] & (1UL << col))
                data |= 0x08;
            if (B5[row] & (1UL << col))
                data |= 0x10;
            if (B6[row] & (1UL << col))
                data |= 0x20;
            if (B7[row] & (1UL << col))
                data |= 0x40;
            if (B8[row] & (1UL << col))
                data |= 0x80;
        }
        BUS = data;
    }
}

// Function to print binary representation
void printBin(int num, int bits)
{
    for (int i = bits - 1; i >= 0; i--)
        printf("%d", (num >> i) & 1);
}

// Function to update FLAGS register
void setFlags(int result)
{
    FLAGS = 0;
    if (result == 0)
        FLAGS |= ZF;
    if (result & 0x80)
        FLAGS |= SF;
    if (result > 0xFF)
        FLAGS |= CF;
    if (result > 0x7F)
        FLAGS |= OF;
}

// ALU Function
int ALU()
{
    printf("\n*****************************************");
    printf("\nFetching Operands...\nOP1 = ");
    printBin(ACC, 8);
    printf("\nOP2 = ");
    printBin(BUS, 8);

    switch (CONTROL)
    {
    case ADD:
        printf("\nOperation = ADD");
        ACC += BUS;
        break;
    case SUB:
        printf("\nOperation = SUB");
        ACC -= BUS;
        break;
    case MUL:
        printf("\nOperation = MUL (Booth's Algorithm)");
        ACC = boothMultiplication(ACC, BUS);
        break;
    case AND:
        printf("\nOperation = AND");
        ACC &= BUS;
        break;
    case OR:
        printf("\nOperation = OR");
        ACC |= BUS;
        break;
    case NOT:
        printf("\nOperation = NOT");
        ACC = ~ACC;
        break;
    case XOR:
        printf("\nOperation = XOR");
        ACC ^= BUS;
        break;
    case SHL:
        printf("\nOperation = SHL");
        ACC <<= 1;
        break;
    case SHR:
        printf("\nOperation = SHR");
        ACC = (unsigned char)ACC >> 1;
        break;
    default:
        printf("\nInvalid ALU Operation\n");
        exit(1);
    }

    printf("\nUpdated ACC = ");
    printBin(ACC, 8);
    setFlags(ACC);
    printf("\nZF = %d, CF = %d, SF = %d, OF = %d\n", FLAGS & ZF, (FLAGS >> 1) & 1, (FLAGS >> 6) & 1, (FLAGS >> 7) & 1);
    return ACC;
}

unsigned int boothMultiplication(unsigned char op1, unsigned char op2)
{
    unsigned char A = 0x00, Q = op2, Qneg1 = 0, M = op1;
    for (int i = 0; i < 8; i++)
    {
        if ((Q & 1) == 1 && Qneg1 == 0)
            A -= M;
        else if ((Q & 1) == 0 && Qneg1 == 1)
            A += M;
        Qneg1 = Q & 1;
        Q = (Q >> 1) | ((A & 1) << 7);
        A >>= 1;
    }
    return (A << 8) | Q;
}

void CU(unsigned char inst_code)
{
    if (inst_code == WACC)
    {
        ACC = MBR;
    }
    else if (inst_code == RACC)
    {
        MBR = ACC;
    }
    else if (inst_code == WB)
    {
        BUS = MBR;
    }
    else if (inst_code == RM)
    {
        IOM = 1;
        RW = 0;
        OE = 1;
        ADDR = MAR;
        MainMemory();
        MBR = BUS;
    }
    else if (inst_code == WM)
    {
        IOM = 1;
        RW = 1;
        OE = 1;
        ADDR = MAR;
        BUS = MBR;
        MainMemory();
    }
    else if (inst_code >= SHR && inst_code <= ADD)
    {
        CONTROL = inst_code;
        ALU();
    }
    else if (inst_code == EOP)
    {
        printf("\nEnd of Program\n");
        exit(0);
    }
}

int main()
{
    printf("===== STARTING ALU AND MEMORY TESTS =====\n\n");

    // Write value 0xFA to memory address 0x400
    IOM = 1;
    RW = 1;
    OE = 1;
    ADDR = 0x400;
    BUS = 0xFA;
    MainMemory();

    // Read it back into MBR
    RW = 0;
    OE = 1;
    ADDR = 0x400;
    MainMemory();
    printf("Read MBR from 0x400: ");
    printBin(MBR, 8);
    printf("  (Actual), Expected: 11111010 (0xFA)\n");

    // ADD: ACC = 0xFA, BUS = 0x05 -> ACC = 0xFF
    MBR = 0xFA;
    CU(WACC);
    MBR = 0x05;
    CU(WB);
    printf("\n>> Performing ADD Operation...\n");
    CU(ADD); // Should print ACC and FLAGS internally

    printf("Expected ACC: 11111111 (0xFF), ZF=0, CF=0, SF=1, OF=1\n");

    // SUB: ACC = 0xFF, BUS = 0x0F -> ACC = 0xF0
    MBR = 0x0F;
    CU(WB);
    printf("\n>> Performing SUB Operation...\n");
    CU(SUB);

    printf("Expected ACC: 11110000 (0xF0), ZF=0, CF=0, SF=1, OF=1\n");

    // AND: ACC = 0xF0 & 0x0F = 0x00
    MBR = 0x0F;
    CU(WB);
    printf("\n>> Performing AND Operation...\n");
    CU(AND);

    printf("Expected ACC: 00000000 (0x00), ZF=1, CF=0, SF=0, OF=0\n");

    // OR: ACC = 0x00 | 0xAA = 0xAA
    MBR = 0xAA;
    CU(WB);
    printf("\n>> Performing OR Operation...\n");
    CU(OR);

    printf("Expected ACC: 10101010 (0xAA), ZF=0, CF=0, SF=1, OF=1\n");

    // XOR: ACC = 0xAA ^ 0xFF = 0x55
    MBR = 0xFF;
    CU(WB);
    printf("\n>> Performing XOR Operation...\n");
    CU(XOR);

    printf("Expected ACC: 01010101 (0x55), ZF=0, CF=0, SF=0, OF=0\n");

    // NOT: ~0x55 = 0xAA
    printf("\n>> Performing NOT Operation...\n");
    CU(NOT);

    printf("Expected ACC: 10101010 (0xAA), ZF=0, CF=0, SF=1, OF=1\n");

    // SHL: 0xAA << 1 = 0x54 (note: 0xAA = 10101010)
    printf("\n>> Performing SHL Operation...\n");
    CU(SHL);

    printf("Expected ACC: 01010100 (0x54), ZF=0, CF=0, SF=0, OF=0\n");

    // SHR: 0x54 >> 1 = 0x2A
    printf("\n>> Performing SHR Operation...\n");
    CU(SHR);

    printf("Expected ACC: 00101010 (0x2A), ZF=0, CF=0, SF=0, OF=0\n");

    printf("\n===== END OF TESTS =====\n");
    return 0;
}
