/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Team9_SP1_TRACS_IntegratedMod
Modified: May 8, 2025
Version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Registers and Control Signals
unsigned char CONTROL;
unsigned char FLAGS = 0;
unsigned char BUS = 0;
unsigned char MBR = 0;
unsigned char MAR = 0;
unsigned char IOBR = 0;
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
#define WIB 0x07
#define WACC 0x09
#define RACC 0x0B
#define SWAP 0x0C
#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11
#define BR 0x10
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
void loadProgram();
void executeProgram(unsigned short startAddr);

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
    printf("\n--- Executing InputSim() ---\n");
    printf("Before InputSim: iOData[0x001] = 0x%02X, iOData[0x01F] = 0x%02X\n",
           iOData[0x001], iOData[0x01F]);

    // Clear the destination buffer
    iOData[0x01F] = 0x00;

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

    printf("After InputSim: iOData[0x001] = 0x%02X, iOData[0x01F] = 0x%02X\n",
           iOData[0x001], iOData[0x01F]);
}

// 7-Segment Display
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
    else if (iOData[ADDR] == 0x0B)
    {
        printf("\n X\n");
        printf(" X   \n");
        printf(" X   \n");
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf(" (B)\n");
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
    if ((result & 0xFF) == 0)
        FLAGS |= ZF;
    if ((result & 0x80) != 0)
        FLAGS |= SF;
    if (result > 0xFF)
        FLAGS |= CF;
    if ((result & 0x80) != 0 && (result < 0))
        FLAGS |= OF;
}

// ALU Function
int ALU()
{
    int result = ACC;
    int op1 = ACC & 0xFF;
    int op2 = BUS & 0xFF;

    printf("\n*****************************************");
    printf("\nFetching Operands...\nOP1 = 0x%02X (", op1);
    printBin(op1, 8);
    printf(")\nOP2 = 0x%02X (", op2);
    printBin(op2, 8);
    printf(")");

    switch (CONTROL)
    {
    case ADD:
        printf("\nOperation = ADD");
        result = op1 + op2;
        break;
    case SUB:
        printf("\nOperation = SUB");
        result = op1 - op2;
        break;
    case MUL:
        printf("\nOperation = MUL (Booth's Algorithm)");
        result = boothMultiplication(op1, op2);
        break;
    case AND:
        printf("\nOperation = AND");
        result = op1 & op2;
        break;
    case OR:
        printf("\nOperation = OR");
        result = op1 | op2;
        break;
    case NOT:
        printf("\nOperation = NOT");
        result = ~op1;
        break;
    case XOR:
        printf("\nOperation = XOR");
        result = op1 ^ op2;
        break;
    case SHL:
        printf("\nOperation = SHL");
        result = op1 << 1;
        break;
    case SHR:
        printf("\nOperation = SHR");
        result = op1 >> 1;
        break;
    default:
        printf("\nInvalid ALU Operation\n");
        exit(1);
    }

    ACC = result;
    printf("\nResult = 0x%02X (", ACC & 0xFF);
    printBin(ACC & 0xFF, 8);
    printf(")");
    setFlags(result);
    printf("\nZF = %d, CF = %d, SF = %d, OF = %d", FLAGS & ZF ? 1 : 0, (FLAGS & CF) ? 1 : 0,
           (FLAGS & SF) ? 1 : 0, (FLAGS & OF) ? 1 : 0);
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
    switch (inst_code)
    {
    case WACC:
        printf("\nExecuting WACC: ACC = MBR (0x%02X)", MBR);
        ACC = MBR;
        break;
    case RACC:
        printf("\nExecuting RACC: MBR = ACC (0x%02X)", ACC & 0xFF);
        MBR = ACC & 0xFF;
        break;
    case WB:
        printf("\nExecuting WB: BUS = MBR (0x%02X)", MBR);
        BUS = MBR;
        break;
    case WIB:
        printf("\nExecuting WIB: IOBR = MBR (0x%02X)", MBR);
        IOBR = MBR;
        break;
    case RM:
        printf("\nExecuting RM: Read Memory at 0x%03X", MAR);
        IOM = 1;
        RW = 0;
        OE = 1;
        ADDR = MAR;
        MainMemory();
        MBR = BUS;
        printf(" -> MBR = 0x%02X", MBR);
        break;
    case WM:
        printf("\nExecuting WM: Write Memory at 0x%03X = 0x%02X", MAR, MBR);
        IOM = 1;
        RW = 1;
        OE = 1;
        ADDR = MAR;
        BUS = MBR;
        MainMemory();
        break;
    case RIO:
        printf("\nExecuting RIO: Read IO at 0x%03X", MAR);
        IOM = 0;
        RW = 0;
        OE = 1;
        ADDR = MAR;
        IOMemory();
        MBR = BUS;
        printf(" -> MBR = 0x%02X", MBR);
        break;
    case WIO:
        printf("\nExecuting WIO: Write IO at 0x%03X = 0x%02X", MAR, MBR);
        IOM = 0;
        RW = 1;
        OE = 1;
        ADDR = MAR;
        BUS = MBR;
        IOMemory();

        // Call InputSim when writing to I/O address 0x001
        if (ADDR == 0x001)
        {
            InputSim();
        }
        break;
    case SWAP:
        printf("\nExecuting SWAP: MBR <-> IOBR");
        unsigned char temp = MBR;
        MBR = IOBR;
        IOBR = temp;
        break;
    case BRE:
    case BRNE:
    case BRGT:
    case BRLT:
    case BR:
        break;
    case EOP:
        printf("\nExecuting EOP: End of Program\n");
        exit(0);
        break;
    default:
        if (inst_code >= SHR && inst_code <= ADD)
        {
            printf("\nExecuting ALU Operation: 0x%02X", inst_code);
            CONTROL = inst_code;
            ALU();
        }
        else
        {
            printf("\nUnknown instruction: 0x%02X\n", inst_code);
            exit(1);
        }
    }
}

void loadProgram() //(TRACS CODE)
{
    // Clear memory
    memset(A1, 0, sizeof(A1));
    memset(A2, 0, sizeof(A2));
    memset(A3, 0, sizeof(A3));
    memset(A4, 0, sizeof(A4));
    memset(A5, 0, sizeof(A5));
    memset(A6, 0, sizeof(A6));
    memset(A7, 0, sizeof(A7));
    memset(A8, 0, sizeof(A8));
    memset(B1, 0, sizeof(B1));
    memset(B2, 0, sizeof(B2));
    memset(B3, 0, sizeof(B3));
    memset(B4, 0, sizeof(B4));
    memset(B5, 0, sizeof(B5));
    memset(B6, 0, sizeof(B6));
    memset(B7, 0, sizeof(B7));
    memset(B8, 0, sizeof(B8));

    // Machine code instructions and data
    unsigned char program[] = {
        // 000
        0x06, 0x15, // WB 0x15
        // 002
        0x01, 0x00, 0x04, // WM 0x400
        // 006
        0x06, 0x05, // WB 0x05
        // 008
        0x09, // WACC
        // 00A
        0x06, 0x08, // WB 0x08
        // 00C
        0x1E, // ADD
        // 00E
        0x02, 0x00, 0x04, // RM 0x400
        // 012
        0x1B, // MUL
        // 014
        0x0B, // RACC
        // 016
        0x01, 0x01, 0x04, // WM 0x401
        // 01A
        0x07, 0x0B, // WIB 0x0B
        // 01C
        0x03, 0x00, 0x00, // WIO 0x000
        // 01E
        0x06, 0x10, // WB 0x10
        // 020
        0x1D, // SUB
        // 022
        0x0B, // RACC
        // 024
        0x03, 0x01, 0x00, // WIO 0x001
        // 028
        0x16, // SHL
        // 02A
        0x16, // SHL
        // 02C
        0x15, // SHR
        // 02E
        0x02, 0x01, 0x04, // RM 0x401
        // 032
        0x19, // OR
        // 034
        0x18, // NOT
        // 036
        0x04, 0x1F, 0x00, // RIO 0x01F
        // 03A
        0x0C, // SWAP
        // 03C
        0x17, // XOR
        // 03E
        0x06, 0xFF, // WB 0xFF
        // 040
        0x1A, // AND
        // 042
        0x02, 0x01, 0x04, // RM 0x401
        // 046
        0x14, 0x3C, 0x00, // BRE 0x03C
        // 04A
        0x06, 0x2E, // WB 0x2E
        // 04C
        0x12, 0x42, 0x00, // BRGT 0x042
        // 050
        0x1A, // AND
        // 052
        0x11, 0x46, 0x00, // BRLT 0x046
        // 056
        0x06, 0x00, // WB 0x00 (unreachable)
        // 058
        0x09, // WACC (unreachable)
        // 05A
        0x06, 0x03, // WB 0x03
        // 05C
        0x09, // WACC
        // 05E
        0x06, 0x00, // WB 0x00 (loop start)
        // 060
        0x14, 0x54, 0x00, // BRE 0x054
        // 064
        0x06, 0x01, // WB 0x01
        // 066
        0x1D, // SUB
        // 068
        0x10, 0x4A, 0x00, // BR 0x04A
        // 06C
        0x1F // EOP
    };

    // Number of machine code bytes
    int programSize = sizeof(program);

    // Load program into memory
    printf("\nLoading program into memory...\n");
    for (int i = 0; i < programSize; i++)
    {
        IOM = 1;
        RW = 1;
        OE = 1;
        ADDR = i;
        BUS = program[i];
        MainMemory();

        if (i % 10 == 0)
            printf("\nLoaded at 0x%03X: 0x%02X", i, program[i]);
    }

    printf("\n\nProgram loaded successfully!\n");
}

// Function to execute program from memory
void executeProgram(unsigned short startAddr)
{
    unsigned short PC = startAddr;
    unsigned char opcode;
    unsigned short operand;
    int steps = 0;

    printf("\nStarting program execution from address 0x%03X\n", startAddr);

    while (1)
    {
        steps++;
        printf("\n========== Step %d ==========", steps);
        printf("\nPC: 0x%03X", PC);

        // Fetch instruction
        IOM = 1;
        RW = 0;
        OE = 1;
        ADDR = PC;
        MainMemory();
        opcode = BUS;
        PC++;

        printf("\nFetched opcode: 0x%02X", opcode);

        // Handle EOP
        if (opcode == EOP)
        {
            printf("\nFound EOP instruction. Ending program execution.");
            CU(EOP);
            return;
        }

        // Handle branch instructions
        if (opcode == BRE || opcode == BRNE || opcode == BRGT || opcode == BRLT || opcode == BR)
        {
            IOM = 1;
            RW = 0;
            OE = 1;
            ADDR = PC;
            MainMemory();
            operand = BUS;
            PC++;

            IOM = 1;
            RW = 0;
            OE = 1;
            ADDR = PC;
            MainMemory();
            operand |= (BUS << 8);
            PC++;

            // Compare ACC with MBR
            int condition = 0;
            int diff = ACC - MBR;

            // Set flags based on comparison
            setFlags(diff);

            switch (opcode)
            {
            case BRE:
                printf("\nExecuting BRE 0x%03X", operand);
                printf("\nComparing ACC (0x%02X) with MBR (0x%02X)", ACC & 0xFF, MBR);
                if (FLAGS & ZF)
                {
                    printf(" - Equal, branching to 0x%03X", operand);
                    PC = operand;
                }
                else
                {
                    printf(" - Not equal, continuing to 0x%03X", PC);
                }
                break;
            case BRNE:
                printf("\nExecuting BRNE 0x%03X", operand);
                printf("\nComparing ACC (0x%02X) with MBR (0x%02X)", ACC & 0xFF, MBR);
                if (!(FLAGS & ZF))
                {
                    printf(" - Not equal, branching to 0x%03X", operand);
                    PC = operand;
                }
                else
                {
                    printf(" - Equal, continuing to 0x%03X", PC);
                }
                break;
            case BRGT:
                printf("\nExecuting BRGT 0x%03X", operand);
                printf("\nComparing ACC (0x%02X) with MBR (0x%02X)", ACC & 0xFF, MBR);
                if (!(FLAGS & ZF) && !(FLAGS & SF))
                {
                    printf(" - Greater than, branching to 0x%03X", operand);
                    PC = operand;
                }
                else
                {
                    printf(" - Not greater than, continuing to 0x%03X", PC);
                }
                break;
            case BRLT:
                printf("\nExecuting BRLT 0x%03X", operand);
                printf("\nComparing ACC (0x%02X) with MBR (0x%02X)", ACC & 0xFF, MBR);
                if ((FLAGS & SF) && !(FLAGS & ZF))
                {
                    printf(" - Less than, branching to 0x%03X", operand);
                    PC = operand;
                }
                else
                {
                    printf(" - Not less than, continuing to 0x%03X", PC);
                }
                break;
            case BR:
                printf("\nExecuting BR 0x%03X", operand);
                printf(" - Unconditional branch to 0x%03X", operand);
                PC = operand;
                break;
            }
        }
        else
        {
            // Check if this instruction requires operands
            if (opcode == WB || opcode == WIB)
            {
                // Get 1-byte operand
                IOM = 1;
                RW = 0;
                OE = 1;
                ADDR = PC;
                MainMemory();
                operand = BUS;
                PC++;
                MBR = operand;
                printf("\nOperand: 0x%02X", operand);
            }
            else if (opcode == RM || opcode == WM || opcode == RIO || opcode == WIO)
            {
                // Get 2-byte address operand
                IOM = 1;
                RW = 0;
                OE = 1;
                ADDR = PC;
                MainMemory();
                operand = BUS;
                PC++;

                IOM = 1;
                RW = 0;
                OE = 1;
                ADDR = PC;
                MainMemory();
                operand |= (BUS << 8);
                PC++;

                // Store address in MAR before executing instruction
                MAR = operand;
                printf("\nAddress operand: 0x%03X", operand);
            }
            // Execute the instruction
            CU(opcode);
        }
        printf("\nAfter execution: ACC=0x%02X, MBR=0x%02X, MAR=0x%03X, IOBR=0x%02X, BUS=0x%02X",
               ACC & 0xFF, MBR, MAR, IOBR, BUS);
        printf("\nFlags: ZF=%d, CF=%d, SF=%d, OF=%d\n",
               (FLAGS & ZF) ? 1 : 0, (FLAGS & CF) ? 1 : 0,
               (FLAGS & SF) ? 1 : 0, (FLAGS & OF) ? 1 : 0);
    }
}

// Main function
int main()
{
    printf("TRACS Computer System Emulator\n");
    printf("==============================\n");

    // Initialize memory and registers
    printf("Initializing system...\n");
    ACC = 0;
    MBR = 0;
    MAR = 0;
    IOBR = 0;
    BUS = 0;
    FLAGS = 0;

    memset(iOData, 0, sizeof(iOData));
    loadProgram();
    executeProgram(0x000);

    printf("\nProgram execution completed.\n");
    return 0;
}