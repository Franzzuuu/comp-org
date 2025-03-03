/*
    Banzon, Skyler Jason P.
    Omega, Franz Ivan D.
    March 3, 2025
*/

#include <stdio.h>

unsigned char dataMemory[2048]; // 2^11
unsigned char ioBuffer[32];     // 2^5

int CU(void)
{
    unsigned int PC = 0x000, IR, MAR, IOAR, inst_code, operand;
    unsigned char MBR = 0, IOBR = 0;

    while (1)
    {
        printf("Initializing Main Memory...\n");
        printf("\n==============================\n");
        printf("PC : 0x%03X\n", PC);

        printf("Fetching instruction...\n");
        IR = dataMemory[PC] << 8; // Upper byte
        PC++;
        IR |= dataMemory[PC]; // Lower byte
        PC++;

        printf("IR : 0x%04X\n", IR);

        inst_code = IR >> 11;
        operand = IR & 0x07FF;

        printf("Instruction Code: 0x%02X\n", inst_code);
        printf("Operand : 0x%03X\n", operand);

        switch (inst_code)
        {
        case 0x06: //(Write to MBR)
            printf("Instruction : WB\n");
            printf("Loading data to MBR...\n");
            MBR = operand & 0xFF;
            printf("MBR  : 0x%02X\n", MBR);
            break;

        case 0x01: //(Write to Memory)
            printf("Instruction : WM\n");
            printf("Writing data to memory...\n");
            MAR = operand;
            dataMemory[MAR] = MBR;
            break;

        case 0x02: // Read from Memory
            printf("Instruction : RM\n");
            printf("Reading data from memory...\n");
            MAR = operand;
            MBR = dataMemory[MAR];
            printf("MBR  : 0x%02X\n", MBR);
            break;

        case 0x03: // Branch
            printf("Instruction : BR\n");
            printf("Branch to 0x%03X on next cycle.\n", operand);
            PC = operand;
            continue; // Skip incrementing PC

        case 0x07: // Write to IOBR
            printf("Instruction : WIB\n");
            printf("Loading data to IOBR...\n");
            IOBR = operand & 0xFF;
            printf("IOBR : 0x%02X\n", IOBR);
            break;

        case 0x05: // Write to IO Buffer
            printf("Instruction : WIO\n");
            printf("Writing to IO buffer...\n");
            IOAR = operand;
            ioBuffer[IOAR] = IOBR;
            break;

        case 0x1F: // End of Program
            printf("Instruction : EOP\n");
            printf("End of program.\n");
            return 1;

        default:
            printf("Error: Unknown instruction code!\n");
            return 0;
        }
    }
}

// inputs
void initMemory(void)
{
    dataMemory[0x000] = 0x30;
    dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C;
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14;
    dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19;
    dataMemory[0x007] = 0x2A;
    dataMemory[0x12A] = 0x38;
    dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28;
    dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8;
    dataMemory[0x12F] = 0x00;
}

// pangcheck if sakto
int main(void)
{
    initMemory();

    if (CU() == 1)
        printf("\nProgram run successfully!\n");
    else
        printf("\nError! Program terminated!\n");

    return 0;
}
