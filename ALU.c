/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Lab Exercise 1: The ALU with Booth's Algorithm
Date: February 18,2025
*/

#include <stdio.h>
#include <stdlib.h>

// Flag registers
unsigned char CF = 0, ZF = 0, OF = 0, SF = 0;
unsigned int ACC = 0;
unsigned char control_signals;
unsigned char operand1, operand2;

// Print in Binary
void printBin(int num, unsigned char bits)
{
    for (int i = bits - 1; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
    }
}

//apply twos complement on given number
unsigned char twosComp(unsigned data){
	return  ~(data) + 1;
}

// Function to set flags based on the result
void setFlags(unsigned int ACC){ 
    if(ACC==0x0000){                // ZF = 1 if ACC = 0
        ZF = 1;
    } else ZF = 0;
    
    
    if((ACC & 0x0080)==0x0080){     // SF = 1 if ACC is negative
        SF = 1;
    }  else SF = 0;
    

    if(ACC > 0x7F){        // OF = 1 if ACC = 01111111
        OF = 1;
    } else OF = 0;
    
    if(ACC > 0xFF){         // CF = 1 if ACC > 11111111
        CF = 1;
    } else CF = 0;
    
}


// ALU function to perform operations
int ALU(int op1, int op2, unsigned char control)
{

    signed short ACC = 0x00; // Accumulator

    signed char tempOp1 = 0x00;
    signed char tempOp2 = 0x00;
    
    printf("\n**************************************\n");
    printf("Fetching Operands...\n");
    printf("OP1 = "); printBin(op1,8); printf("\n");
    printf("OP2 = "); printBin(op2,8); printf("\n");

    printf("Operation = ");
    
    switch (control){
        case 0x01: // Addition
            if((op1 & 0x80) == 0x80){
                tempOp1 = twosComp((signed char)op1);  // Correct assignment
            } else {
                tempOp1 = (signed char)op1;
            }
            if((op2 & 0x80) == 0x80){
                tempOp2 = twosComp((signed char)op2);
            } else {
                tempOp2 = (signed char)op2;
            }
        
            printf("ADD\n");
            printf("2's complement of OP1\n");
            printf("2's complement of OP2\n");
            printf("Adding OP2 to OP1\n");
            ACC = tempOp1 + tempOp2;
            break;
        case 0x02: // Subtraction using 2's complement
            printf("SUB\n");
            printf("2's complement of OP2\n");
            printf("Adding OP2 to OP1\n");
            ACC = op1 + twosComp(op2);
            break;
        case 0x03:  // Booth's Multiplication
            printf("MUL\n");
        
            signed short regA = 0;
            signed short regQ = op2;  // Q (Multiplicand)
            signed short regM = op1;  // M (Multiplier)
            int lastBitQ = 0;
            int stepCount = 8;
        
            printf("Initial values:\n");
            printf("A: "); printBin(regA, 8); printf("\t");
            printf("Q: "); printBin(regQ, 8); printf("\t");
            printf("M: "); printBin(regM, 8); printf("\n");
        
            while (stepCount > 0) {
                // Booth's condition checks
                if ((regQ & 1) == 1 && lastBitQ == 0) {
                    regA = regA - regM;
                } else if ((regQ & 1) == 0 && lastBitQ == 1) {
                    regA = regA + regM;
                }
        
                // Save LSB of Q before shifting
                lastBitQ = regQ & 1;
        
                // Arithmetic Right Shift (Sign Extension)
                int signBitA = regA & 0x80;
                regQ = (regQ >> 1) | ((regA & 1) << 7);  // Shift regQ right, bringing A0
                regA = (regA >> 1) | signBitA;           // Preserve sign bit in A
        
                stepCount--;
            }
        
            // Correct ACC computation
            ACC = (regA << 8) | (regQ & 0xFF);
            break;
        
        case 0x04: // Bitwise AND
            printf("AND\n");
            ACC = op1 & op2;
            break;
        case 0x05: // Bitwise OR
            printf("OR\n");
            ACC = op1 | op2;
            break;
        case 0x06: // Negation
            printf("NOT\n");
            ACC = ~op1;
            op2 = 0x00;
            break;
        case 0x07: // XOR
            printf("XOR\n");
            ACC = op1 ^ op2;
            break;
        case 0x08: // Right Shift
            printf("SHR\n");
            ACC  = op1 >> 1;
            op2 = 0;
            break;
        case 0x09: // Left Shift
            printf("SHL\n");
            ACC = op1 << 1;
            op2 = 0;
            break;
        default:
            printf("Invalid operation\n");
            exit(1);
    }

    printf("ACC = "); printBin(ACC,16);
    
    setFlags(ACC);
    printf("\nZF = %d, CF = %d, SF = %d, OF = %d\n", ZF, CF, SF, OF);

    return ACC;
}

// Main function
void main()
{
    ALU(0x03,0x05,0x02); // 00000011 - 00000101 (Subtract)
    ALU(0x88,0x85,0x01); // 10001000 + 10000101 (Add)
    ALU(0xC0,0x02,0x03); // 11000000 * 00000010 (Multiply)
    ALU(0x0F,0x0F,0x04); // 00001111 & 00001111 (AND)
    ALU(0x05,0x0A,0x05); // 00000101 | 00001010 (OR)
    ALU(0x0F,0x07,0x06); // ~00001111 (NOT)
    ALU(0x0B,0x0D,0x07); // 00001011 ^ 00001101 (XOR)
    ALU(0x1B,0x07,0x08); // >> 00011011 (SHR)
    ALU(0x1B,0x07,0x09); // << 00011011 (SHL)
}
