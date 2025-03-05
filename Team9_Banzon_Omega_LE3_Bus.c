#include <stdio.h>
#include <stdint.h>

#define MEMORY_SIZE 2048  // 2^11 = 2048
#define IO_BUFFER_SIZE 32 // 2^5 = 32

// Global variables for buses and control signals
uint16_t ADDR = 0x000;  // Address Bus (11-bit)
uint8_t BUS = 0x00;     // Data Bus (8-bit)
uint8_t CONTROL = 0x00; // Control Bus (5-bit)

// Control signals
uint8_t IOM = 0;  // IO/M signal (1 = Memory, 0 = I/O)
uint8_t RW = 0;   // R/W signal (1 = Write, 0 = Read)
uint8_t OE = 0;   // Output Enable signal (1 = Enable, 0 = Disable)

// Memory and I/O buffer
uint8_t dataMemory[MEMORY_SIZE];  // 8-bit wide memory
uint8_t ioBuffer[IO_BUFFER_SIZE]; // 8-bit wide I/O buffer

// Function prototypes
void MainMemory(void);
void IOMemory(void);
void initMemory(void);

// Main Memory function
void MainMemory(void) {
    if (IOM == 1) {  // Main memory access
        if (RW == 0 && OE == 1) {  // Memory read
            BUS = dataMemory[ADDR];
        } else if (RW == 1 && OE == 1) {  // Memory write
            dataMemory[ADDR] = BUS;
        }
    }
}

// I/O Memory function
void IOMemory(void) {
    if (IOM == 0) {  // I/O buffer access
        if (RW == 0 && OE == 1) {  // I/O read
            BUS = ioBuffer[ADDR];
        } else if (RW == 1 && OE == 1) {  // I/O write
            ioBuffer[ADDR] = BUS;
        }
    }
}

// Initialize memory with instructions and data
void initMemory(void) {
    printf("\nInitializing Main Memory...\n\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        dataMemory[i] = 0xFF; // Initialize with a non-zero value for debugging
    }
    dataMemory[0x000] = 0x30; dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C; dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14; dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19; dataMemory[0x007] = 0x2A;
    dataMemory[0x12A] = 0x38; dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28; dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8; dataMemory[0x12F] = 0x00;
}

// Control Unit function
int CU(void) {
    uint16_t PC = 0x000, IR = 0x0000, MAR = 0x000;
    uint8_t MBR = 0x00, IOBR = 0x00;
    uint16_t IOAR = 0x000;
    while (1) {
        printf("****************************\nPC: 0x%03X\nFetching instruction...\n", PC);
        ADDR = PC; OE = 1; RW = 0; IOM = 1; MainMemory();
        printf("DEBUG: Fetching upper byte at ADDR 0x%03X, BUS = 0x%02X\n", ADDR, BUS);
        IR = (uint16_t)BUS << 8; PC++;
        ADDR = PC; OE = 1; RW = 0; IOM = 1; MainMemory();
        printf("DEBUG: Fetching lower byte at ADDR 0x%03X, BUS = 0x%02X\n", ADDR, BUS);
        IR |= BUS; PC++;
        uint8_t inst_code = IR >> 11; uint16_t operand = IR & 0x07FF;
        printf("IR: 0x%04X\nInstruction Code: 0x%02X\nOperand: 0x%03X\n", IR, inst_code, operand);
        switch (inst_code) {
            case 0x06: MBR = operand & 0xFF;
                printf("Instruction: WB\nLoading data to MBR...\nMBR: 0x%02X\n", MBR);
                break;
            case 0x01: MAR = operand; BUS = MBR; RW = 1; OE = 1; IOM = 1; MainMemory();
                printf("Instruction: WM\nWriting data to memory...\nADDR: 0x%03X, BUS: 0x%02X\n", MAR, BUS);
                break;
            case 0x02: MAR = operand; RW = 0; OE = 1; IOM = 1; MainMemory(); MBR = BUS;
                printf("Instruction: RM\nReading data from memory...\nMBR: 0x%02X\n", MBR);
                break;
            case 0x03: PC = operand;
                printf("Instruction: BR\nBranch to 0x%03X on next cycle.\n", PC);
                break;
            case 0x07: IOBR = operand & 0xFF;
                printf("Instruction: WIB\nLoading data to IOBR...\nIOBR: 0x%02X\n", IOBR);
                break;
            case 0x05: IOAR = operand; BUS = IOBR; RW = 1; OE = 1; IOM = 0; IOMemory();
                printf("Instruction: WIO\nWriting to IO buffer...\nADDR: 0x%03X, BUS: 0x%02X\n", IOAR, BUS);
                break;
            case 0x1F:
                printf("Instruction: EOP\nEnd of program.\nProgram run successfully!\n");
                return 1;
            default:
                printf("Unknown instruction code: 0x%02X\n", inst_code);
                return 0;
        }
    }
}

// Main function
int main(void) {
    initMemory();
    return CU();
}
