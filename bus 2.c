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
    // Initialize data memory with instructions
    dataMemory[0x000] = 0x30; // WB - write data 0xFF to MBR
    dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C; // WM - write data from MBR to memory address 0x400
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14; // RM - read data from memory address 0x400
    dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19; // BR - branch to memory address 0x12A
    dataMemory[0x007] = 0x2A;
    dataMemory[0x12A] = 0x38; // WIB - write data 0x05 to IOBR
    dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28; // WIO - write data from IOBR to I/O buffer address 0x0A
    dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8; // EOP - end of program
    dataMemory[0x12F] = 0x00;

    // Initialize the rest of memory to 0x00 (optional)
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (dataMemory[i] != 0x00) continue; // Skip already initialized locations
        dataMemory[i] = 0x00;
    }
}

// Control Unit function
int CU(void) {
    uint16_t PC = 0x000;  // Program Counter (11-bit)
    uint16_t IR = 0x0000; // Instruction Register (16-bit)
    uint16_t MAR = 0x000; // Memory Address Register (11-bit)
    uint8_t MBR = 0x00;   // Memory Buffer Register (8-bit)
    uint16_t IOAR = 0x000; // I/O Address Register (11-bit)
    uint8_t IOBR = 0x00;  // I/O Buffer Register (8-bit)

    // Local control signals
    uint8_t Fetch = 0;  // Fetch signal
    uint8_t Memory = 0; // Memory access signal
    uint8_t IO = 0;     // I/O access signal

    // Fetch and execute instructions
    while (1) {
        // Fetch instruction (2 clock cycles)
        Fetch = 1;  // Set Fetch signal
        Memory = 0;
        IO = 0;

        // Fetch upper byte
        ADDR = PC;
        MainMemory();  // Fetch upper byte
        IR = (uint16_t)BUS << 8;
        printf("Fetching upper byte: PC = 0x%03X, ADDR = 0x%03X, BUS = 0x%02X, IR = 0x%04X\n", PC, ADDR, BUS, IR);
        PC++;

        // Fetch lower byte
        ADDR = PC;
        MainMemory();  // Fetch lower byte
        IR |= BUS;
        printf("Fetching lower byte: PC = 0x%03X, ADDR = 0x%03X, BUS = 0x%02X, IR = 0x%04X\n", PC, ADDR, BUS, IR);
        PC++;

        // Decode instruction
        uint8_t inst_code = IR >> 11;  // Extract 5-bit instruction code
        uint16_t operand = IR & 0x07FF; // Extract 11-bit operand
        printf("Decoded instruction: IR = 0x%04X, inst_code = 0x%02X, operand = 0x%03X\n", IR, inst_code, operand);

        // Execute instruction
        switch (inst_code) {
            case 0x01: // WM (Write to Memory)
                MAR = operand;
                Fetch = 0;
                Memory = 1;
                IO = 0;
                CONTROL = inst_code;
                IOM = 1;
                RW = 1;
                OE = 1;
                ADDR = MAR;
                BUS = MBR;
                MainMemory();
                printf("WM: Writing data 0x%02X to memory address 0x%03X\n", MBR, MAR);
                break;

            case 0x02: // RM (Read from Memory)
                MAR = operand;
                Fetch = 0;
                Memory = 1;
                IO = 0;
                CONTROL = inst_code;
                IOM = 1;
                RW = 0;
                OE = 1;
                ADDR = MAR;
                MainMemory();
                MBR = BUS;
                printf("RM: Reading data 0x%02X from memory address 0x%03X\n", MBR, MAR);
                break;

            case 0x03: // BR (Branch)
                PC = operand;
                printf("BR: Branching to address 0x%03X\n", PC);
                break;

            case 0x04: // RIO (Read from I/O Buffer)
                IOAR = operand;
                Fetch = 0;
                Memory = 0;
                IO = 1;
                CONTROL = inst_code;
                IOM = 0;
                RW = 0;
                OE = 1;
                ADDR = IOAR;
                IOMemory();
                IOBR = BUS;
                printf("RIO: Reading data 0x%02X from I/O buffer address 0x%03X\n", IOBR, IOAR);
                break;

            case 0x05: // WIO (Write to I/O Buffer)
                IOAR = operand;
                Fetch = 0;
                Memory = 0;
                IO = 1;
                CONTROL = inst_code;
                IOM = 0;
                RW = 1;
                OE = 1;
                ADDR = IOAR;
                BUS = IOBR;
                IOMemory();
                printf("WIO: Writing data 0x%02X to I/O buffer address 0x%03X\n", IOBR, IOAR);
                break;

            case 0x06: // WB (Write to MBR)
                MBR = operand & 0xFF;
                printf("WB: Writing data 0x%02X to MBR\n", MBR);
                break;

            case 0x07: // WIB (Write to IOBR)
                IOBR = operand & 0xFF;
                printf("WIB: Writing data 0x%02X to IOBR\n", IOBR);
                break;

            case 0x1F: // EOP (End of Program)
                printf("EOP: End of program.\n");
                return 1; // Successful termination

            default:
                printf("Unknown instruction code: 0x%02X\n", inst_code);
                return 0; // Error termination
        }

        // Echo bus and control signals
        printf("ADDR: 0x%03X, BUS: 0x%02X, CONTROL: 0x%02X\n\n", ADDR, BUS, CONTROL);
    }
}

// Main function
int main(void) {
    initMemory(); // Initialize memory with instructions and data

    if (CU() == 1) {
        printf("Program run successfully!\n");
    } else {
        printf("Error encountered, program terminated!\n");
    }

    return 0;
}