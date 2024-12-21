#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Library for character handling functions

// Function prototypes
int romanToDecimal(char *roman); // Convert Roman numerals to decimal integers
int performOperation(int num1, int num2, char operation); // Perform basic arithmetic operations
void numberToWords(int num, char *output); // Convert numbers to their word representation
void processInputFile(const char *inputFile, const char *outputFile); // Process input and output files
void processInputFileWithLogging(const char *inputFile, const char *outputFile); // Process files with additional logging
void writeOutput(FILE *outputFile, int result); // Write the result to the output file
int isValidRoman(char *roman); // Validate a string as a Roman numeral
void cleanString(char *str); // Remove unwanted characters from a string

// Main function
int main() {
    const char *inputFile = "Input.txt"; // Input file name
    const char *outputFile = "Output.txt"; // Output file name

    // Call the function to process the input file and generate output
    processInputFileWithLogging(inputFile, outputFile);

    printf("Processing completed! Check %s for results.\n", outputFile); // Notify user of completion
    return 0;
}

// Convert Roman numeral to decimal
int romanToDecimal(char *roman) {
    int total = 0, current = 0, next = 0;
    int length = strlen(roman);

    for (int i = 0; i < length; i++) { // Loop through each character of the Roman numeral
        switch (toupper(roman[i])) { // Determine the value of the current Roman numeral character
            case 'I': current = 1; break;
            case 'V': current = 5; break;
            case 'X': current = 10; break;
            case 'L': current = 50; break;
            case 'C': current = 100; break;
            case 'D': current = 500; break;
            case 'M': current = 1000; break;
            default: return -1; // Return -1 for invalid characters
        }

        // Check the value of the next Roman numeral character
        if (i + 1 < length) {
            switch (toupper(roman[i + 1])) {
                case 'I': next = 1; break;
                case 'V': next = 5; break;
                case 'X': next = 10; break;
                case 'L': next = 50; break;
                case 'C': next = 100; break;
                case 'D': next = 500; break;
                case 'M': next = 1000; break;
                default: next = 0;
            }
        } else {
            next = 0; // No next character, set to 0
        }

        if (current < next) {
            total -= current; // Subtract if current value is smaller than next
        } else {
            total += current; // Otherwise, add the current value
        }
    }
    return total;
}

// Perform arithmetic operations
int performOperation(int num1, int num2, char operation) {
    switch (operation) {
        case '+': return num1 + num2; // Addition
        case '-': return num1 - num2; // Subtraction
        case '*': return num1 * num2; // Multiplication
        case '/': return (num2 != 0) ? num1 / num2 : -1; // Division, avoiding division by zero
        default: return -1; // Invalid operation
    }
}

// Convert number to words
void numberToWords(int num, char *output) {
    // Arrays for units, tens, teens, and thousands place names
    const char *units[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    if (num == 0) { // Special case for zero
        strcpy(output, "Zero");
        return;
    }

    char buffer[256] = ""; // Buffer to store words as we build them
    char temp[256] = ""; // Temporary buffer for constructing words
    int isNegative = (num < 0); // Check if the number is negative
    num = abs(num); // Convert number to positive for processing

    int chunk, place = 0;

    while (num > 0) {
        chunk = num % 1000; // Process the last three digits of the number

        if (chunk > 0) {
            char chunkWords[256] = "";

            if (chunk / 100 > 0) { // Process hundreds place
                strcat(chunkWords, units[chunk / 100]);
                strcat(chunkWords, " Hundred ");
            }

            int remainder = chunk % 100; // Process remainder (tens and units)
            if (remainder > 10 && remainder < 20) { // Special case for teens
                strcat(chunkWords, teens[remainder - 11]);
            } else {
                if (remainder / 10 > 0) { // Process tens place
                    strcat(chunkWords, tens[remainder / 10]);
                    strcat(chunkWords, " ");
                }
                if (remainder % 10 > 0) { // Process units place
                    strcat(chunkWords, units[remainder % 10]);
                }
            }

            if (place > 0) { // Add thousand/million/billion as necessary
                strcat(chunkWords, " ");
                strcat(chunkWords, thousands[place]);
            }

            snprintf(temp, sizeof(temp), "%s %s", chunkWords, buffer); // Combine with previous chunks
            strcpy(buffer, temp);
        }

        num /= 1000; // Move to the next three digits
        place++;
    }

    snprintf(output, 256, "%s%s", isNegative ? "Negative " : "", buffer); // Add "Negative" if necessary
    output[strcspn(output, "\n")] = '\0'; // Remove trailing newline if present
}

// Process input file
void processInputFile(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r"); // Open input file
    FILE *output = fopen(outputFile, "w"); // Open output file

    if (!input || !output) { // Check for file opening errors
        printf("Error opening file(s).\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), input)) { // Read each line from the input file
        cleanString(line); // Remove unwanted characters
        char roman1[50], roman2[50], operation;
        int num1, num2, result;

        if (sscanf(line, "%49s %c %49s", roman1, &operation, roman2) == 3) { // Parse line into Roman numerals and operation
            if (!isValidRoman(roman1) || !isValidRoman(roman2)) { // Validate Roman numerals
                fprintf(output, "Invalid Roman numeral in line: %s\n", line); // Write error to output
                continue;
            }

            num1 = romanToDecimal(roman1); // Convert first Roman numeral to decimal
            num2 = romanToDecimal(roman2); // Convert second Roman numeral to decimal

            result = performOperation(num1, num2, operation); // Perform the operation

            if (result == -1) { // Check for invalid operation
                fprintf(output, "Invalid operation in line: %s\n", line);
            } else {
                writeOutput(output, result); // Write the result to the output file
            }
        } else {
            fprintf(output, "Invalid input format in line: %s\n", line); // Write format error to output
        }
    }

    fclose(input); // Close input file
    fclose(output); // Close output file
}

// Process input file with logging
void processInputFileWithLogging(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r"); // Open input file
    FILE *output = fopen(outputFile, "w"); // Open output file
    FILE *processLog = fopen("Process.txt", "w"); // Open log file

    if (!input || !output || !processLog) { // Check for file opening errors
        printf("Error opening file(s).\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), input)) { // Read each line from the input file
        cleanString(line); // Clean the input line
        char roman1[50], roman2[50], operation;
        int num1, num2, result;

        fprintf(processLog, "Processing line: %s", line); // Log the processing line

        if (sscanf(line, "%49s %c %49s", roman1, &operation, roman2) == 3) { // Parse line into components
            if (!isValidRoman(roman1) || !isValidRoman(roman2)) { // Validate Roman numerals
                fprintf(output, "Invalid Roman numeral in line: %s\n", line);
                fprintf(processLog, "Error: Invalid Roman numeral in line.\n");
                continue;
            }

            num1 = romanToDecimal(roman1); // Convert first Roman numeral to decimal
            num2 = romanToDecimal(roman2); // Convert second Roman numeral to decimal

            fprintf(processLog, "Converted: %s -> %d, %s -> %d\n", roman1, num1, roman2, num2); // Log conversions

            result = performOperation(num1, num2, operation); // Perform operation

            if (result == -1) { // Check for invalid operation
                fprintf(output, "Invalid operation in line: %s\n", line);
                fprintf(processLog, "Error: Invalid operation '%c'.\n", operation);
            } else {
                writeOutput(output, result); // Write result to output file
                fprintf(processLog, "Result: %d\n", result); // Log the result
            }
        } else {
            fprintf(output, "Invalid input format in line: %s\n", line); // Write format error to output
            fprintf(processLog, "Error: Invalid input format.\n"); // Log format error
        }
    }

    fclose(input); // Close input file
    fclose(output); // Close output file
    fclose(processLog); // Close log file
}

// Validate Roman numerals
int isValidRoman(char *roman) {
    for (int i = 0; roman[i]; i++) { // Iterate over each character in the Roman numeral
        char c = toupper(roman[i]); // Convert character to uppercase
        if (c != 'I' && c != 'V' && c != 'X' && c != 'L' && c != 'C' && c != 'D' && c != 'M') { // Check for invalid characters
            return 0; // Return false if an invalid character is found
        }
    }
    return 1; // Return true if all characters are valid
}

// Clean input strings
void cleanString(char *str) {
    char *ptr = str;
    while (*ptr) { // Iterate through the string
        if (isprint(*ptr) || isspace(*ptr)) { // Keep printable or whitespace characters
            ptr++;
        } else {
            *ptr = '\0'; // Terminate the string at the first unwanted character
            break;
        }
    }
}

// Write output to file
void writeOutput(FILE *outputFile, int result) {
    char resultInWords[256]; // Buffer to store the result in words
    numberToWords(result, resultInWords); // Convert result to words
    fprintf(outputFile, "%s\n", resultInWords); // Write the result in words to output file
}
