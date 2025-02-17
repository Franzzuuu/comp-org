/*
Authors: Skyler Jason P. Banzon & Franz Ivan D. Omega
Title: Lab Exercise 1: The ALU with Booth's Algorithm
Date:
*/

#include <stdio.h>
#include <stdlib.h>

// Flag registers
unsigned char CF = 0, ZF = 0, OF = 0, SF = 0;
unsigned int ACC = 0; // Accumulator
unsigned char control_signals;
unsigned char operand1, operand2;

// Function to print binary representation
void printBin(int num, unsigned char bits)
{
    for (int i = bits - 1; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
    }
}

unsigned char twosComp(unsigned data){
	return  ~(data) + 1;
}

// Booth's Multiplication Algorithm
int boothMultiplication(int M, int Q, int bit_size)
{
    int A = 0, Qm1 = 0;
    int count = bit_size;
    int neg_M = -M; // Store -M for subtraction step

    printf("\nBooth's Multiplication Steps:\n");
    printf("Step |        A        |     Q     | Qm1 |      M       | Operation\n");
    printf("------------------------------------------------------------------\n");

    while (count > 0)
    {
        printf(" %2d   |    ", bit_size - count + 1);
        printBin(A, bit_size); // Print A in binary
        printf("    |  ");
        printBin(Q, bit_size); // Print Q in binary
        printf("  |  %d  | ", Qm1);
        printBin(M, bit_size); // Print M in binary1
        printf(" | ");

        // Booth’s algorithm conditions
        if ((Q & 1) == 1 && Qm1 == 0)
        {
            A += neg_M; // A = A - M
            printf("A = A - M");
        }
        else if ((Q & 1) == 0 && Qm1 == 1)
        {
            A += M; // A = A + M
            printf("A = A + M");
        }
        else
        {
            printf("No Operation");
        }

        printf("\n");

        // Arithmetic right shift (A, Q, Qm1)
        int MSB_A = (A >> (bit_size - 1)) & 1; // Get MSB of A before shifting
        int LSB_Q = Q & 1;                     // Save LSB of Q before shifting

        Q = (Q >> 1) | ((A & 1) << (bit_size - 1)); // LSB of A moves to MSB of Q
        A = (A >> 1) | (MSB_A << (bit_size - 1));   // Maintain sign extension
        Qm1 = LSB_Q;                                // Update Q-1

        count--;
    }

    int product = (A << bit_size) | Q; // Combine A and Q for the final product

    printf("\nFinal Product (Binary): ");
    printBin(product, 2 * bit_size);
    printf("\nFinal Product (Decimal): %d\n", product);
    return product;
}

// Function to set flags based on the result
void setFlags(unsigned int ACC){
	if(ACC == 0x0000){
		ZF = 1;
	}
	
	if((ACC & 0x0080) == 0x0080){
		SF = 1;
	}
	
	if(ACC > 0x7F){
		OF = 1;
		if (ACC > 0xFF){
			CF = 1;
		} 
	}
	return;
}	


// ALU function to perform operations
int ALU(int op1, int op2, unsigned char control)
{

    signed short ACC = 0x00; // Accumulator

    int tempOp1 = 0x00;
    int tempOp2 = 0x00;
    
    printf("\n**************************************\n");
    printf("Fetching Operands...\n");
    printf("OP1 = "); printBin(op1,8); printf("\n");
    printf("OP2 = "); printBin(op2,8); printf("\n");

    printf("Operation = ");
    
    switch (control){
        case 1: // Addition
            if((op1 & 0x80)== 0x80){
                tempOp1 = twosComp(op1);
            } else if((op2 & 0x80)== 0x80){
                tempOp2 = twosComp(op2);
            }
            else{
                tempOp1 = op1;
                tempOp2 = op2;
            }
            printf("ADD\n");
            printf("2's complement of OP1\n");
            printf("2's complement of OP2\n");
            printf("Adding OP2 to OP1\n");
            ACC = tempOp1 + tempOp2;
            break;
        case 2: // Subtraction using 2's complement
            printf("SUB\n");
            printf("2's complement of OP2\n");
            printf("Adding OP2 to OP1\n");
            ACC = op1 - twosComp(op2);
            break;
        case 0x03:  // Booth's Multiplication
            printf("MUL\n");

            signed short regA = 0;
            signed short regQ = op1;
            signed short regM = op2;
            int lastBitQ = 0;
            int stepCount = 8;

            printf("Initial values:\n");
            printf("A: "); printBin(regA, 8);
            printf("\tQ: "); printBin(regQ, 8);
            printf("\tM: "); printBin(regM, 8);
            printf("\n");

            while (stepCount > 0) {
                // Booth's condition checks
                if ((regQ & 1) == 1 && lastBitQ == 0) {
                    regA = regA - regM;
                } else if ((regQ & 1) == 0 && lastBitQ == 1) {
                    regA = regA + regM;
                }

                // Arithmetic Right Shift
                lastBitQ = regQ & 1;  // Save Q0 before shifting
                regQ = (regQ >> 1) | ((regA & 1) << 7); // Shift regQ right, bringing A0
                regA = (regA & 0x80) | (regA >> 1); // Preserve sign bit in A

                stepCount--;
            }

            // Correct ACC computation
            ACC = ((regA & 0xFF) << 8) | (regQ & 0xFF);
            break;
        case 4: // Bitwise AND
            ACC = op1 & op2;
            break;
        case 5: // Bitwise OR
            ACC = op1 | op2;
            break;
        case 6: // Negation
            ACC = ~op1;
            op2 = 0x00;
            break;
        case 7: // XOR
            ACC = op1 ^ op2;
            break;
        case 8: // Right Shift
            ACC  = op1 >> 1;
            op2 = 0;
            break;
        case 9: // Left Shift
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
    ALU(0xC0,0x02,0x03); // 11000000 * 00000010 = (Multiply)
}
