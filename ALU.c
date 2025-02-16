/*
Authors:
Title: Lab Exercise 1: The ALU with Booth's Algorithm
Date:
*/

#include <stdio.h>
#include <stdlib.h>

// Flag registers
unsigned char C = 0, Z = 0, OF = 0, SF = 0;
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
void setFlags(int result, unsigned char data_width)
{
    Z = (result == 0) ? 1 : 0;
    SF = (result >> (data_width - 1)) & 1;
    int max_positive = (1 << (data_width - 1)) - 1;
    int min_negative = -(1 << (data_width - 1));
    OF = (result > max_positive || result < min_negative) ? 1 : 0;
    C = 0;
}

// ALU function to perform operations
int ALU(int op1, int op2, unsigned char control)
{
    int result = 0;
    switch (control)
    {
    case 1: // Addition
        result = op1 + op2;
        printf("2's complement of OP1\n");
        printf("2's complement of OP2\n");
        printf("Adding OP1 & OP2\n");
        break;
    case 2: // Subtraction using 2's complement
        result = op1 - op2;
        printf("2's complement of OP2\n");
        printf("Adding OP2 to OP1\n");
        break;
    case 3: // Multiplication (Booth's Algorithm)
        result = boothMultiplication(op1, op2, 8);
        printf("Multiplying OP1 & OP2 using Booth's Algorithm\n");
        break;
    case 4: // Bitwise AND
        result = op1 & op2;
        break;
    case 5: // Bitwise OR
        result = op1 | op2;
        break;
    case 6: // Negation
        result = ~op1;
        op2 = 0;
        break;
    case 7: // XOR
        result = op1 ^ op2;
        break;
    case 8: // Right Shift
        result = op1 >> 1;
        op2 = 0;
        break;
    case 9: // Left Shift
        result = op1 << 1;
        op2 = 0;
        break;
    default:
        printf("Invalid operation\n");
        exit(1);
    }
    ACC = result;
    setFlags(result, 8);
    return result;
}

// Main function
int main()
{
    int restart = 10;
    while (restart > 0)
    {
        printf("Select Operation (1-9): ");
        scanf("%hhu", &control_signals);

        printf("Input Operands (two single-digit numbers): \n");
        printf("Input for Operand 1: ");
        scanf(" %c", &operand1);
        printf("Input for Operand 2: ");
        scanf(" %c", &operand2);

        // Convert characters to integers correctly
        operand1 -= '0';
        operand2 -= '0';

        // Display operands in binary
        printf("Operand 1 (Binary): ");
        printBin(operand1, 8);
        printf("\n");
        printf("Operand 2 (Binary): ");
        printBin(operand2, 8);
        printf("\n");

        int result = ALU(operand1, operand2, control_signals);

        // Display result in binary
        printf("ACC: ");
        printBin(result, 8);
        printf("\nFlags: C=%d, Z=%d, SF=%d, OF=%d\n\n", C, Z, SF, OF);
        restart--;
    }
    return 0;
}