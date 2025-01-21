#include <stdio.h>
#include <stdlib.h>
#define MAX_BUFFER_SIZE 100000

enum brackets{
    CURLY = '}',
    ROUNDED = ')',
    SQUARE = ']',
    POINTY = '>'
};

int checkStack(char* stack) {
    int index = 0, errorCount = 0;
    char c;

    while (scanf("%c", &stack[index]) == 1 && stack[index] != '\n') {
        c = stack[index];
        if (index == 0 && (c == CURLY || c == ROUNDED || c == SQUARE || c == POINTY)) {
            errorCount++;
            continue;
        }

        switch (c) {
            case CURLY:
                if (stack[index - 1] != '{') {
                    errorCount++;
                }
                stack[index] = '\0';
                while (index >= 0 && stack[index] == '\0') {
                    index--;
                }
                break;

            case ROUNDED:
                if (stack[index - 1] != '(') {
                    errorCount++;
                }
                stack[index] = '\0';
                while (index >= 0 && stack[index] == '\0') {
                    index--;
                }
                break;

            case SQUARE:
                if (stack[index - 1] != '[') {
                    errorCount++;
                }
                stack[index] = '\0';
                while (index >= 0 && stack[index] == '\0') {
                    index--;
                }
                break;

            case POINTY:
                if (stack[index - 1] != '<') {
                    errorCount++;
                }
                stack[index] = '\0';
                while (index >= 0 && stack[index] == '\0') {
                    index--;
                }
                break;

            default:
                index++;
                break;
        }
        index++; // Increment index after storing the character
    }
    return errorCount;
}


int main(void){
    int inputSize;
    char stack[MAX_BUFFER_SIZE];
    scanf("%d", &inputSize);
    for (int i = 0; i < inputSize; i++)
    {
        (i != (inputSize-1))?printf("%d\n", checkStack(stack)):printf("%d", checkStack(stack));
    }
    /*
    Example of input:
    5
    {[]([]{}){[]{}}}
    ((([]()){}]}
    [{([}]}
    {[]
    ({})]

    This should be the output:
    0
    2
    3
    1
    1
    */
    return 0;
}