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
unsigned short ADDR = 0;
int ACC = 0;

// Memory and I/O Buffers
unsigned char ioBuffer[32];

// 32x32 bit memory chips declaration
long A1[32], A2[32], A3[32], A4[32], A5[32], A6[32], A7[32], A8[32]; // chip group A
long B1[32], B2[32], B3[32], B4[32], B5[32], B6[32], B7[32], B8[32]; // chip group B

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
#define OR  0x19
#define NOT 0x18
#define XOR 0x17
#define SHL 0x16
#define SHR 0x15

#define WM  0x01
#define RM  0x02
#define WB  0x06
#define WACC 0x09
#define RACC 0x0B
#define EOP 0x1F

// Function Prototypes
void MainMemory();
void printBin(int num, int bits);
void setFlags(int result);
int ALU();
void CU(unsigned char inst_code);

// Function to print binary representation
void printBin(int num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

// Function to update FLAGS register
void setFlags(int result) {
    FLAGS = 0;
    if (result == 0) FLAGS |= ZF;
    if (result & 0x80) FLAGS |= SF;
    if (result > 0xFF) FLAGS |= CF;
    if (result > 0x7F) FLAGS |= OF;
}

// ALU Function
int ALU() {
    printf("\n*****************************************");
    printf("\nFetching Operands...");
    printf("\nOP1 = "); printBin(ACC, 8);
    printf("\nOP2 = "); printBin(BUS, 8);

    switch (CONTROL) {
        case ADD: printf("\nOperation = ADD"); ACC += BUS; break;
        case SUB: printf("\nOperation = SUB"); ACC -= BUS; break;
        case AND: printf("\nOperation = AND"); ACC &= BUS; break;
        case OR:  printf("\nOperation = OR"); ACC |= BUS; break;
        case NOT: printf("\nOperation = NOT"); ACC = ~ACC; break;
        case XOR: printf("\nOperation = XOR"); ACC ^= BUS; break;
        case SHL: printf("\nOperation = SHL"); ACC <<= 1; break;
        case SHR: printf("\nOperation = SHR"); ACC >>= 1; break;
        default: printf("\nInvalid ALU operation\n"); exit(1);
    }

    printf("\nUpdated ACC = "); printBin(ACC, 8);
    setFlags(ACC);
    printf("\nZF = %d, CF = %d, SF = %d, OF = %d\n",
        FLAGS & ZF, (FLAGS >> 1) & 1, (FLAGS >> 6) & 1, (FLAGS >> 7) & 1);
    return ACC;
}

// Control Unit Function
void CU(unsigned char inst_code) {
    switch (inst_code) {
        case WACC: ACC = MBR; break;
        case RACC: MBR = ACC; break;
        case WB:   BUS = MBR; break;
        case RM:   MAR = ADDR & 0x7FF; IOM = 1; RW = 0; OE = 1; MainMemory(); break;
        case WM:   MAR = ADDR & 0x7FF; BUS = MBR; IOM = 1; RW = 1; OE = 1; MainMemory(); break;
        case EOP:  exit(0); break;
        default:   CONTROL = inst_code; ALU(); break;
    }
}

// Main Memory function using TRACS
void MainMemory() {
    int col = MAR & 0x1F;
    int row = (MAR >> 5) & 0x1F;
    unsigned char cs = (MAR >> 10) & 0x01;
    unsigned char bit;

    if (IOM && OE) {
        if (RW) {
            for (int i = 0; i < 8; i++) {
                bit = (BUS >> i) & 1;
                if (cs == 0) {
                    if (i == 0) A1[row] = (A1[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 1) A2[row] = (A2[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 2) A3[row] = (A3[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 3) A4[row] = (A4[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 4) A5[row] = (A5[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 5) A6[row] = (A6[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 6) A7[row] = (A7[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 7) A8[row] = (A8[row] & ~(1L << col)) | ((long)bit << col);
                } else {
                    if (i == 0) B1[row] = (B1[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 1) B2[row] = (B2[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 2) B3[row] = (B3[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 3) B4[row] = (B4[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 4) B5[row] = (B5[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 5) B6[row] = (B6[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 6) B7[row] = (B7[row] & ~(1L << col)) | ((long)bit << col);
                    else if (i == 7) B8[row] = (B8[row] & ~(1L << col)) | ((long)bit << col);
                }
            }
        } else {
            BUS = 0;
            for (int i = 0; i < 8; i++) {
                if (cs == 0) {
                    bit = (i == 0) ? (A1[row] >> col) & 1 :
                          (i == 1) ? (A2[row] >> col) & 1 :
                          (i == 2) ? (A3[row] >> col) & 1 :
                          (i == 3) ? (A4[row] >> col) & 1 :
                          (i == 4) ? (A5[row] >> col) & 1 :
                          (i == 5) ? (A6[row] >> col) & 1 :
                          (i == 6) ? (A7[row] >> col) & 1 :
                                    (A8[row] >> col) & 1;
                } else {
                    bit = (i == 0) ? (B1[row] >> col) & 1 :
                          (i == 1) ? (B2[row] >> col) & 1 :
                          (i == 2) ? (B3[row] >> col) & 1 :
                          (i == 3) ? (B4[row] >> col) & 1 :
                          (i == 4) ? (B5[row] >> col) & 1 :
                          (i == 5) ? (B6[row] >> col) & 1 :
                          (i == 6) ? (B7[row] >> col) & 1 :
                                    (B8[row] >> col) & 1;
                }
                BUS |= (bit << i);
            }
            MBR = BUS;
        }
    }
}

// Main Function to Test TRACS Memory
int main() {
    printf("===== STARTING ALU AND MEMORY TESTS =====\n\n");

    // Write value 0xFA to memory address 0x400
    IOM = 1; RW = 1; OE = 1;
    ADDR = 0x400; BUS = 0xFA; MainMemory();

    // Read it back into MBR
    RW = 0; OE = 1;
    ADDR = 0x400; MainMemory();
    printf("Read MBR from 0x400: "); printBin(MBR, 8); printf("\n");

    // ADD: ACC = 0xFA, BUS = 0x05 -> ACC = 0xFF
    MBR = 0xFA; CU(WACC);
    MBR = 0x05; CU(WB); CU(ADD);

    // SUB: ACC = 0xFF, BUS = 0x0F -> ACC = 0xF0
    MBR = 0x0F; CU(WB); CU(SUB);

    // AND: ACC = 0xF0 & 0x0F = 0x00
    MBR = 0x0F; CU(WB); CU(AND);

    // OR: ACC = 0x00 | 0xAA = 0xAA
    MBR = 0xAA; CU(WB); CU(OR);

    // XOR: ACC = 0xAA ^ 0xFF = 0x55
    MBR = 0xFF; CU(WB); CU(XOR);

    // NOT: ~0x55 = 0xAA
    CU(NOT);

    // SHL: 0xAA << 1 = 0x54
    CU(SHL);

    // SHR: 0x54 >> 1 = 0x2A
    CU(SHR);

    printf("\n===== END OF TESTS =====\n");
    return 0;
}
