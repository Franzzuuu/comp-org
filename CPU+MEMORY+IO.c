/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Lab Exercise 6: Input/Output (Integrated with Memory and ALU)
Date: May 7, 2025
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

// I/O Memory (latches and buffers)
unsigned char iOData[32]; // 8-bit data x 32 (16 latches and 16 buffers)

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
#define WIO 0x03
#define RIO 0x04
#define WB 0x06
#define WACC 0x09
#define RACC 0x0B
#define SWAP 0x0C

#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11

#define EOP 0x1F

// ======== Function Prototypes ========
void MainMemory();
void IOMemory();
void InputSim();
void SevenSegment();
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

// ======== I/O MEMORY START ========
void IOMemory()
{
    if (OE) // check if output is enabled
    {
        if (RW && !IOM) // check if memory write and IO Memory access
        {
            if (ADDR >= 0x000 && ADDR <= 0x00F) // check the address if valid for latches
            {
                iOData[ADDR] = BUS; // write data in BUS to IO Memory

                // Special case for 7-segment display
                if (ADDR == 0x000)
                {
                    SevenSegment();
                }
            }
        }
        else if (!RW && !IOM)
        {
            if (ADDR >= 0x010 && ADDR <= 0x01F) // check the address if valid for buffers
            {
                BUS = iOData[ADDR]; // load data to BUS
            }
        }
    }
}

// Function to simulate input connectivity between latch and buffer
void InputSim()
{
    unsigned char data;
    for (int i = 7; i >= 0; i--)
    {
        /* load source data */
        data = iOData[0x001];
        /* shift bit to LSB */
        data = data >> i;
        /* mask upper bits */
        data = data & 0x01;
        /* position bit */
        data = data << (7 - i);
        /* write bit to dest buffer */
        iOData[0x01F] = iOData[0x01F] | data;
    }
}

// 7-Segment Display Simulator
void SevenSegment()
{
    if (iOData[ADDR] == 0x01)
    {
        printf(" X\n");
        printf(" X\n");
        printf(" X\n");
        printf(" X\n");
        printf(" X\n");
        printf(" X\n");
        printf(" X\n");
    }
    else if (iOData[ADDR] == 0x02)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
        printf(" X \n");
        printf(" X \n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x03)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x04)
    {
        printf(" X    X\n");
        printf(" X    X\n");
        printf(" X    X\n");
        printf(" XXXXXX\n");
        printf("      X\n");
        printf("      X\n");
        printf("      X\n");
    }
    else if (iOData[ADDR] == 0x05)
    {
        printf(" XXXXX\n");
        printf(" X \n");
        printf(" X \n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x06)
    {
        printf(" XXXXX\n");
        printf(" X \n");
        printf(" X \n");
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x07)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
    }
    else if (iOData[ADDR] == 0x08)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x09)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }
    else if (iOData[ADDR] == 0x00)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
    // getc(stdin);
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
    else if (inst_code == RIO)
    {
        IOM = 0;
        RW = 0;
        OE = 1;
        ADDR = MAR;
        IOMemory();
        MBR = BUS;
    }
    else if (inst_code == WIO)
    {
        IOM = 0;
        RW = 1;
        OE = 1;
        ADDR = MAR;
        BUS = MBR;
        IOMemory();
        // For testing purpose, call InputSim when writing to I/O address 0x001
        if (ADDR == 0x001)
        {
            InputSim();
        }
    }
    else if (inst_code == SWAP)
    {
        unsigned char temp = MBR;
        MBR = ACC;
        ACC = temp;
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

// Function to write machine code to memory
void loadProgram(unsigned short startAddr, unsigned char *program, int size)
{
    for (int i = 0; i < size; i++)
    {
        IOM = 1;
        RW = 1;
        OE = 1;
        ADDR = startAddr + i;
        BUS = program[i];
        MainMemory();
    }
}

// Function to execute program from memory
void executeProgram(unsigned short startAddr)
{
    unsigned short PC = startAddr;
    unsigned char opcode;
    unsigned char operand;

    while (1)
    {
        // Fetch instruction
        IOM = 1;
        RW = 0;
        OE = 1;
        ADDR = PC;
        MainMemory();
        opcode = BUS;
        PC++;

        // Fetch operand if needed
        if (opcode != EOP && opcode != NOT && opcode != SHL && opcode != SHR && opcode != RACC && opcode != WACC && opcode != SWAP)
        {
            IOM = 1;
            RW = 0;
            OE = 1;
            ADDR = PC;
            MainMemory();
            operand = BUS;
            PC++;

            // Load operand to MAR if it's a memory or I/O operation
            if (opcode == RM || opcode == WM || opcode == RIO || opcode == WIO)
            {
                MAR = operand;
            }
            else
            {
                MBR = operand;
            }
        }

        // Execute instruction
        CU(opcode);

        // Debugging output
        printf("\nPC: 0x%03X, Opcode: 0x%02X, ACC: 0x%02X, MBR: 0x%02X, FLAGS: 0x%02X\n",
               PC, opcode, ACC & 0xFF, MBR, FLAGS);
    }
}

// Sample program to test I/O functionality
unsigned char testIO[] = {
    // Send data to latch at address 0x001
    0x09, 0xCA, // WACC 0xCA (Load 0xCA into ACC)
    0x0B,       // RACC (ACC to MBR)
    0x03, 0x01, // WIO 0x01 (Write to I/O address 0x01)

    // Read from buffer at address 0x01F
    0x04, 0x1F, // RIO 0x1F (Read from I/O address 0x1F)
    0x09,       // WACC (MBR to ACC)

    // Move data to memory address 0x406
    0x0B,       // RACC (ACC to MBR)
    0x01, 0x06, // WM 0x06 (Write to memory address 0x406)
    0x04, 0x04,

    // Verify by reading back
    0x02, 0x06, // RM 0x06 (Read from memory address 0x406)
    0x04, 0x04,

    // End program
    0x1F // EOP
};

// Sample countdown program using 7-segment display
unsigned char countdownProgram[] = {
    // Initialize counter to 9
    0x09, 0x09, // WACC 0x09 (Load 9 into ACC)

    // Display loop
    0x0B,       // RACC (ACC to MBR)
    0x03, 0x00, // WIO 0x00 (Write to 7-segment display at address 0x00)

    // Delay (simulated with NOPs)
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, // SWAP instructions as NOP

    // Decrement counter
    0x0B,       // RACC (ACC to MBR)
    0x06,       // WB (MBR to BUS)
    0x09, 0x01, // WACC 0x01 (Load 1 into ACC)
    0x1D,       // SUB (Subtract 1 from ACC)

    // Check if counter is zero
    0x14, 0x1F, // BRE 0x1F (Branch to EOP if zero)

    // Loop back
    0x09, 0x09, // WACC ? (Load jump target address)

    // End program
    0x1F // EOP
};

int main()
{
    printf("\n\n===== TESTING I/O MEMORY OPERATIONS =====\n");

    // Test Write to I/O (Latch)
    printf("\n--- Testing Write to I/O Latch ---\n");
    MAR = 0x001; // I/O address (latch)
    MBR = 0xCA;  // Data to write
    CU(WIO);     // Write to I/O
    printf("Wrote 0xCA to I/O latch at address 0x001\n");

    // Test Read from I/O (Buffer)
    printf("\n--- Testing Read from I/O Buffer ---\n");
    MAR = 0x01F; // I/O address (buffer)
    CU(RIO);     // Read from I/O
    printf("Read from I/O buffer at address 0x01F: 0x%02X\n", MBR);

    printf("\n\n===== TESTING SEVEN SEGMENT DISPLAY =====\n");

    for (int i = 0; i <= 9; i++)
    {
        printf("\n--- Testing Seven Segment Display for digit %d ---\n", i);
        MAR = 0x000; // Seven segment display address
        MBR = i;     // Digit to display
        CU(WIO);     // Write to I/O (Seven Segment)
        printf("Displayed digit %d on seven segment display\n", i);
    }
    return 0;
}
