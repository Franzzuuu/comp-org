/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Lab Exercise 4: The ALU Version 2
Date: March 15, 2025
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

// Memory and I/O Buffers
unsigned char dataMemory[2048];
unsigned char ioBuffer[32];

// External Control Signals
unsigned char IOM = 0;
unsigned char RW = 0;
unsigned char OE = 0;

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

// Function Prototypes
void mainMemory();
void IOMemory();
void printBin(int num, int bits);
void setFlags(int result);
int ALU();
void CU(unsigned char inst_code);
void executeBranch(unsigned char inst_code);
unsigned char twosComp(unsigned char data);
unsigned int boothMultiplication(unsigned char op1, unsigned char op2);

// Function to print binary representation
void printBin(int num, int bits)
{
    for (int i = bits - 1; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
    }
}

// Function to update FLAGS register
void setFlags(int result)
{
    FLAGS = 0; // Reset FLAGS

    if (result == 0)
        FLAGS |= ZF; // Zero Flag
    if (result & 0x80)
        FLAGS |= SF; // Sign Flag
    if (result > 0xFF)
        FLAGS |= CF; // Carry Flag
    if (result > 0x7F)
        FLAGS |= OF; // Overflow Flag
}

// ALU Function
int ALU()
{
    int temp_ACC = ACC;

    printf("\n*****************************************");
    printf("\nFetching Operands...");
    printf("\nOP1 = ");
    printBin(ACC, 8);
    printf("\nOP2 = ");
    printBin(BUS, 8);

    switch (CONTROL)
    {
    case ADD:
        printf("\nOperation = ADD");
        printf("\nAdding OP1 & OP2");
        ACC = ACC + BUS;
        break;
    case SUB:
        printf("\nOperation = SUB");
        printf("\nSubtracting OP1 & OP2");
        ACC = ACC - BUS;
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
    printf("\nZF = %d, CF = %d, SF = %d, OF = %d\n",
           FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 6) & 0x01, (FLAGS >> 7) & 0x01);

    return ACC;
}

// Booth's Algorithm for Multiplication
unsigned int boothMultiplication(unsigned char op1, unsigned char op2)
{
    unsigned char A = 0x00;
    unsigned char Q = op2;
    unsigned char Qneg1 = 0;
    unsigned char M = op1;
    unsigned int result = 0;

    printf("\n    A        Q     Q_-1     M\n");

    for (int i = 0; i < 8; i++)
    {
        printf(" ");
        printBin(A, 8);
        printf(" ");
        printBin(Q, 8);
        printf("   %d", Qneg1);
        printf("   ");
        printBin(M, 8);
        printf("\n");

        if ((Q & 1) == 1 && Qneg1 == 0)
            A -= M;
        else if ((Q & 1) == 0 && Qneg1 == 1)
            A += M;

        Qneg1 = Q & 1;
        Q = (Q >> 1) | ((A & 1) << 7);
        A >>= 1;
    }

    result = (A << 8) | Q;
    return result;
}

// Control Unit Function
void CU(unsigned char inst_code)
{
    if (inst_code == WACC)
    {
        printf("\nWACC: Writing MBR to ACC");
        ACC = MBR;
    }
    else if (inst_code == RACC)
    {
        printf("\nRACC: Reading ACC to MBR");
        MBR = ACC;
    }
    else if (inst_code == WB)
    {
        printf("\nWB: Writing MBR to BUS");
        BUS = MBR;
    }
    else if (inst_code == RM)
    {
        printf("\nRM: Reading memory to MBR");
        MBR = dataMemory[MAR];
    }
    else if (inst_code == WM)
    {
        printf("\nWM: Writing MBR to memory");
        dataMemory[MAR] = MBR;
    }
    else if (inst_code >= 0x15 && inst_code <= 0x1E)
    {
        printf("\nExecuting ALU instruction...");
        CONTROL = inst_code;
        ALU();
    }
    else if (inst_code == EOP)
    {
        printf("\nEnd of Program");
        exit(0);
    }
}

int main()
{
    printf("\n===== STARTING ALU TESTS =====\n");

    // ADD Test: 0x05 + 0x08 = 0x0D
    MBR = 0x05;
    CU(WACC); // Load 0x05 into ACC
    MBR = 0x08;
    CU(WB);  // Load 0x08 into BUS
    CU(ADD); // ACC = ACC + BUS (0x05 + 0x08 = 0x0D)
    printf("\nExpected ACC: 00001101 (0x0D)\n");

    // SUB Test: 0x0D - 0x04 = 0x09
    MBR = 0x04;
    CU(WB);  // Load 0x04 into BUS
    CU(SUB); // ACC = ACC - BUS (0x0D - 0x04 = 0x09)
    printf("\nExpected ACC: 00001001 (0x09)\n");

    // MUL Test: 0x09 * 0x03 = 0x1B (Using Booth's Algorithm)
    MBR = 0x03;
    CU(WB);  // Load 0x03 into BUS
    CU(MUL); // ACC = ACC * BUS (0x09 * 0x03 = 0x1B)
    printf("\nExpected ACC: 00011011 (0x1B)\n");

    // AND Test: 0x1B & 0x0F = 0x0B
    MBR = 0x0F;
    CU(WB);
    CU(AND);
    printf("\nExpected ACC: 00001011 (0x0B)\n");

    // OR Test: 0x0B | 0x30 = 0x3B
    MBR = 0x30;
    CU(WB);
    CU(OR);
    printf("\nExpected ACC: 00111011 (0x3B)\n");

    // XOR Test: 0x3B ^ 0x2A = 0x11
    MBR = 0x2A;
    CU(WB);
    CU(XOR);
    printf("\nExpected ACC: 00010001 (0x11)\n");

    // NOT Test: ~0x11 = 0xEE
    CU(NOT);
    printf("\nExpected ACC: 11101110 (0xEE)\n");

    // SHL Test: 0xEE << 1 = 0xDC
    CU(SHL);
    printf("\nExpected ACC: 11011100 (0xDC)\n");

    // SHR Test: 0xDC >> 1 = 0x6E
    CU(SHR);
    printf("\nExpected ACC: 01101110 (0x6E)\n");

    printf("\n===== END OF ALU TESTS =====\n");

    return 0;
}