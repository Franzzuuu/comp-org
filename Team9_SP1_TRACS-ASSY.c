/*************************************************************************************************
 *
 * Author: TRACS Assembler Implementation
 * Course: CpE 3202 Computer Organization and Architecture
 * Description: A two-pass assembler for TRACS assembly language
 * Created: May 8, 2025
 *
 *************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Constants
#define MAX_LINE_LENGTH 255
#define MAX_LINES 1000
#define MAX_LABEL_LENGTH 32
#define MAX_INSTRUCTION_LENGTH 32
#define MAX_OPERAND_LENGTH 32
#define DATA_MEMORY_START 0x400
#define DATA_MEMORY_END 0x7FF
#define IO_MEMORY_START 0x000
#define IO_MEMORY_END 0x01F

// Structure to store parsed instructions
typedef struct
{
    char label[MAX_LABEL_LENGTH];
    char instruction[MAX_INSTRUCTION_LENGTH];
    char operand[MAX_OPERAND_LENGTH];
    int lineNumber;
    int address;
} Instruction;

// Structure to store label information
typedef struct
{
    char name[MAX_LABEL_LENGTH];
    int address;
} Label;

// Global variables
Instruction instructions[MAX_LINES];
Label labels[MAX_LINES];
int instructionCount = 0;
int labelCount = 0;
int currentAddress = 0;
bool hasEOP = false;

// Error flags
bool hasError = false;
bool invalidOperandError = false;
bool invalidAddressError = false;
bool unknownInstructionError = false;

// Function prototypes
void readTxtFile(const char *filename);
void firstPass();
void secondPass(const char *outputFilename);
bool isValidInstruction(const char *instruction);
int getInstructionCode(const char *instruction);
bool requiresOperand(const char *instruction);
void parseLine(char *line, int lineNumber);
int parseOperand(const char *operand);
int findLabelAddress(const char *label);
void trim(char *str);
bool isValidAddress(int address, const char *instruction);
void reportErrors();

int main(int argc, char *argv[])
{
    const char *inputFilename;
    const char *outputFilename;

    // Check command line arguments
    if (argc < 3)
    {
        printf("Usage: %s <input_file.txt> <output_file.txt>\n", argv[0]);
        printf("Using default filenames: Team9_SP1_TRACS-ASSY-Source.txt and output.txt\n");
        inputFilename = "Team9_SP1_TRACS-ASSY-Source.txt";
        outputFilename = "output.txt";
    }

    else
    {
        inputFilename = argv[1];
        outputFilename = argv[2];
    }

    // Execute the two-pass assembly process
    readTxtFile(inputFilename);
    firstPass();
    secondPass(outputFilename);
    reportErrors();

    return 0;
}

// Read the assembly file and store each line
void readTxtFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open input file '%s'\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    int lineNumber = 1;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        // Remove newline character
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        // Parse the line and store the instruction
        parseLine(line, lineNumber);
        lineNumber++;
    }

    fclose(file);
}

// Parse a line of assembly code
void parseLine(char *line, int lineNumber)
{
    // Skip empty lines
    trim(line);
    if (strlen(line) == 0)
    {
        return;
    }

    // Extract comment if present
    char *comment = strchr(line, ';');
    if (comment != NULL)
    {
        *comment = '\0'; // Remove the comment
        trim(line);
        if (strlen(line) == 0)
        {
            return; // Line was just a comment
        }
    }

    // Check if the line contains a label
    char *labelEnd = strchr(line, ':');
    if (labelEnd != NULL)
    {
        // Extract label
        size_t labelLength = labelEnd - line;
        if (labelLength > 0 && labelLength < MAX_LABEL_LENGTH)
        {
            strncpy(instructions[instructionCount].label, line, labelLength);
            instructions[instructionCount].label[labelLength] = '\0';
            trim(instructions[instructionCount].label);

            // Move past the label
            line = labelEnd + 1;
            trim(line);
        }
        // Skip if line is empty after label (label-only line)
        if (strlen(line) == 0)
        {
            // Only a label is on this line; still store it
            instructions[instructionCount].instruction[0] = '\0';
            instructions[instructionCount].operand[0] = '\0';
            instructions[instructionCount].lineNumber = lineNumber;

            // Store label into label table
            if (strlen(instructions[instructionCount].label) > 0)
            {
                strcpy(labels[labelCount].name, instructions[instructionCount].label);
                labels[labelCount].address = currentAddress;
                labelCount++;
            }

            instructionCount++;
            return;
        }
    }
    else
    {
        // No label
        instructions[instructionCount].label[0] = '\0';
    }

    // Extract instruction and operand
    char *operandStart = NULL;
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (isspace(line[i]))
        {
            operandStart = &line[i];
            break;
        }
    }

    if (operandStart != NULL)
    {
        // Have both instruction and operand
        size_t instructionLength = operandStart - line;
        strncpy(instructions[instructionCount].instruction, line, instructionLength);
        instructions[instructionCount].instruction[instructionLength] = '\0';
        trim(instructions[instructionCount].instruction);

        // Get operand
        strcpy(instructions[instructionCount].operand, operandStart);
        trim(instructions[instructionCount].operand);
    }
    else
    {
        // Only instruction, no operand
        strcpy(instructions[instructionCount].instruction, line);
        trim(instructions[instructionCount].instruction);
        instructions[instructionCount].operand[0] = '\0';
    }

    // Store line number
    instructions[instructionCount].lineNumber = lineNumber;

    // Check for EOP instruction
    if (strcmp(instructions[instructionCount].instruction, "EOP") == 0)
    {
        hasEOP = true;
    }

    instructionCount++;
}

// Trim whitespace from the beginning and end of a string
void trim(char *str)
{
    if (str == NULL)
    {
        return;
    }

    // Trim leading whitespace
    char *start = str;
    while (isspace(*start))
    {
        start++;
    }

    // If all spaces, return empty string
    if (*start == '\0')
    {
        *str = '\0';
        return;
    }

    // Trim trailing whitespace
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end))
    {
        end--;
    }

    // Write new null terminator
    *(end + 1) = '\0';

    // Move the trimmed string to the original pointer if needed
    if (start != str)
    {
        memmove(str, start, (end - start) + 2);
    }
}

// First pass: calculate addresses for all instructions and collect labels
void firstPass()
{
    currentAddress = 0;

    for (int i = 0; i < instructionCount; i++)
    {
        // Check if it's ORG directive
        if (strcmp(instructions[i].instruction, "ORG") == 0)
        {
            // Set address based on operand
            currentAddress = parseOperand(instructions[i].operand);
            instructions[i].address = currentAddress;
        }
        else
        {
            // Set the address for this instruction
            instructions[i].address = currentAddress;

            // Check if the instruction is valid
            if (!isValidInstruction(instructions[i].instruction))
            {
                printf("Error: Unknown instruction '%s' at line %d\n",
                       instructions[i].instruction, instructions[i].lineNumber);
                unknownInstructionError = true;
                hasError = true;
            }

            // Check if operand is required but missing or present but not required
            bool needsOperand = requiresOperand(instructions[i].instruction);
            bool hasOperand = strlen(instructions[i].operand) > 0;

            if (needsOperand && !hasOperand)
            {
                printf("Error: Missing operand for instruction '%s' at line %d\n",
                       instructions[i].instruction, instructions[i].lineNumber);
                invalidOperandError = true;
                hasError = true;
            }
            else if (!needsOperand && hasOperand && strcmp(instructions[i].instruction, "ORG") != 0)
            {
                printf("Error: Illegal operand for instruction '%s' at line %d\n",
                       instructions[i].instruction, instructions[i].lineNumber);
                invalidOperandError = true;
                hasError = true;
            }

            // Store label if present
            if (strlen(instructions[i].label) > 0)
            {
                strcpy(labels[labelCount].name, instructions[i].label);
                labels[labelCount].address = currentAddress;
                labelCount++;
            }

            // Increment address (all TRACS instructions use 2 bytes)
            currentAddress += 2;
        }
    }

    // Check if the program has an EOP instruction
    if (!hasEOP)
    {
        printf("Error: EOP instruction not found\n");
        hasError = true;
    }
}

// Second pass: generate machine code and write to output file
void secondPass(const char *outputFilename)
{
    FILE *file = fopen(outputFilename, "w");
    if (file == NULL)
    {
        printf("Error: Cannot open output file '%s'\n", outputFilename);
        exit(1);
    }

    for (int i = 0; i < instructionCount; i++)
    {
        // Skip ORG directives for output
        if (strcmp(instructions[i].instruction, "ORG") == 0)
        {
            continue;
        }

        int instructionCode = getInstructionCode(instructions[i].instruction);
        int operandValue = 0;

        // If instruction requires an operand, process it
        if (requiresOperand(instructions[i].instruction))
        {
            // Check if operand is a label
            if (instructions[i].operand[0] != '0' || instructions[i].operand[1] != 'x')
            {
                // Operand is a label, look up its address
                operandValue = findLabelAddress(instructions[i].operand);
            }
            else
            {
                // Operand is a hex value
                operandValue = parseOperand(instructions[i].operand);
            }

            // Check if address is valid for memory operations
            if ((strcmp(instructions[i].instruction, "WM") == 0 ||
                 strcmp(instructions[i].instruction, "RM") == 0) &&
                !isValidAddress(operandValue, instructions[i].instruction))
            {
                printf("Error: Invalid address 0x%X for instruction '%s' at line %d\n",
                       operandValue, instructions[i].instruction, instructions[i].lineNumber);
                invalidAddressError = true;
                hasError = true;
            }
        }

        // Write instruction byte to file
        fprintf(file, "%03X: %02X ", instructions[i].address, instructionCode);

        // Write operand byte(s) to file
        if (requiresOperand(instructions[i].instruction))
        {
            // Check if it's a branch instruction (requires 11-bit address)
            if (strncmp(instructions[i].instruction, "BR", 2) == 0)
            {
                fprintf(file, "%03X\n", operandValue);
            }
            else
            {
                fprintf(file, "%02X\n", operandValue);
            }
        }
        else
        {
            fprintf(file, "00\n");
        }
    }

    fclose(file);
}

// Check if an instruction is valid
bool isValidInstruction(const char *instruction)
{
    // List of valid TRACS instructions
    const char *validInstructions[] = {
        "ADD", "SUB", "MUL", "AND", "OR", "NOT", "XOR", "SHL", "SHR",
        "WM", "RM", "RIO", "WIO", "WB", "WIB", "WACC", "RACC", "SWAP",
        "BR", "BRE", "BRNE", "BRGT", "BRLT", "EOP", "ORG", NULL};

    for (int i = 0; validInstructions[i] != NULL; i++)
    {
        if (strcmp(instruction, validInstructions[i]) == 0)
        {
            return true;
        }
    }

    return false;
}

// Get the instruction code for a given instruction
int getInstructionCode(const char *instruction)
{
    // Map instructions to their codes
    if (strcmp(instruction, "ADD") == 0)
        return 0xF0;
    if (strcmp(instruction, "SUB") == 0)
        return 0xE8;
    if (strcmp(instruction, "MUL") == 0)
        return 0xD8;
    if (strcmp(instruction, "AND") == 0)
        return 0xD0;
    if (strcmp(instruction, "OR") == 0)
        return 0xC8;
    if (strcmp(instruction, "NOT") == 0)
        return 0xC0;
    if (strcmp(instruction, "XOR") == 0)
        return 0xB8;
    if (strcmp(instruction, "SHL") == 0)
        return 0xB0;
    if (strcmp(instruction, "SHR") == 0)
        return 0xA8;
    if (strcmp(instruction, "WM") == 0)
        return 0x0C;
    if (strcmp(instruction, "RM") == 0)
        return 0x14;
    if (strcmp(instruction, "RIO") == 0)
        return 0x20;
    if (strcmp(instruction, "WIO") == 0)
        return 0x28;
    if (strcmp(instruction, "WB") == 0)
        return 0x30;
    if (strcmp(instruction, "WIB") == 0)
        return 0x38;
    if (strcmp(instruction, "WACC") == 0)
        return 0x48;
    if (strcmp(instruction, "RACC") == 0)
        return 0x58;
    if (strcmp(instruction, "SWAP") == 0)
        return 0x70;
    if (strcmp(instruction, "BR") == 0)
        return 0x18;
    if (strcmp(instruction, "BRE") == 0)
        return 0xA0;
    if (strcmp(instruction, "BRNE") == 0)
        return 0x91;
    if (strcmp(instruction, "BRGT") == 0)
        return 0x90;
    if (strcmp(instruction, "BRLT") == 0)
        return 0x88;
    if (strcmp(instruction, "EOP") == 0)
        return 0xF8;

    return 0; // Default (should not reach here if instruction is valid)
}

// Check if an instruction requires an operand
bool requiresOperand(const char *instruction)
{
    // Instructions that require operands
    const char *operandInstructions[] = {
        "WM", "RM", "RIO", "WIO", "WB", "WIB", "BR", "BRE", "BRNE", "BRGT", "BRLT", "ORG", NULL};

    for (int i = 0; operandInstructions[i] != NULL; i++)
    {
        if (strcmp(instruction, operandInstructions[i]) == 0)
        {
            return true;
        }
    }

    return false;
}

// Parse an operand value (hex format)
int parseOperand(const char *operand)
{
    // Check if the operand starts with "0x"
    if (strncmp(operand, "0x", 2) == 0 || strncmp(operand, "0X", 2) == 0)
    {
        return (int)strtol(operand + 2, NULL, 16);
    }

    // Otherwise, treat as decimal
    return (int)strtol(operand, NULL, 10);
}

// Find the address of a label
int findLabelAddress(const char *label)
{
    for (int i = 0; i < labelCount; i++)
    {
        if (strcmp(labels[i].name, label) == 0)
        {
            return labels[i].address;
        }
    }

    // Label not found, report error
    printf("Error: Undefined label '%s'\n", label);
    hasError = true;
    return 0;
}

// Check if an address is valid for memory instructions
bool isValidAddress(int address, const char *instruction)
{
    // Data memory range check
    if ((strcmp(instruction, "WM") == 0 || strcmp(instruction, "RM") == 0) &&
        (address < DATA_MEMORY_START || address > DATA_MEMORY_END))
    {
        return false;
    }

    // I/O memory range check
    if ((strcmp(instruction, "RIO") == 0 || strcmp(instruction, "WIO") == 0) &&
        (address < IO_MEMORY_START || address > IO_MEMORY_END))
    {
        return false;
    }

    return true;
}

// Report any errors found during assembly
void reportErrors()
{
    if (!hasError)
    {
        printf("Build successful! No errors found.\n");
    }
    else
    {
        printf("\nBuild failed with errors:\n");

        if (!hasEOP)
        {
            printf("- EOP instruction not found\n");
        }

        if (invalidOperandError)
        {
            printf("- Invalid operand(s) found\n");
        }

        if (invalidAddressError)
        {
            printf("- Invalid address(es) found\n");
        }

        if (unknownInstructionError)
        {
            printf("- Unknown instruction(s) found\n");
        }
    }
}