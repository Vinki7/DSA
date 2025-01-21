#include <stdio.h>
#include <stdlib.h>
#define MAX_BUFFER_SIZE 100000
#define MAX_BUFFER_STATIC 13000

enum brackets{
    CURLY = '}',
    ROUNDED = ')',
    SQUARE = ']',
    POINTY = '>'
};

int checkStack(char* stack){
    int koef = 2, index = 0, errorCount = 0, stackPosition = 0;
    scanf("%s", stack);
    char* buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_SIZE/koef));
    do
    {
        buffer[index] = stack[stackPosition];
        if (index == 0 && (buffer[index] == CURLY || buffer[index] == ROUNDED || buffer[index] == SQUARE || buffer[index] == POINTY))
        {
            errorCount++;
        }else{
            switch (buffer[index])
            {
            case CURLY:
                if (buffer[index-1] != '{')
                {
                    errorCount++;
                }
                index--;
                break;
            
            case ROUNDED:
                if (buffer[index-1] != '(')
                {
                    errorCount++;
                }
                index--;
                break;

            case SQUARE:// REVIEW THE CODE FROM HERE AND TEST IT IN DEBUGGER... THEN IN DEIMOS
                if (buffer[index-1] != '[')
                {
                    errorCount++;
                }
                index--;
                break;                

            case POINTY:
                if (buffer[index-1] != '<')
                {
                    errorCount++;
                }
                index--;
                break;

            default:
                index++;
                if (index >= (MAX_BUFFER_SIZE/koef)){
                    buffer = realloc(buffer, MAX_BUFFER_SIZE);
                }
                break;
            }
        }
        stackPosition++;
    }while ( stack[stackPosition] != '\0');
    free(buffer);
    return errorCount + index;
}

int main(void){
    int inputSize;
    char stack[MAX_BUFFER_STATIC];
    scanf("%d", &inputSize);
    for (int i = 0; i < inputSize; i++)
    {
        (i != (inputSize-1))?printf("%d\n", checkStack(stack)):printf("%d", checkStack(stack));
    }
    return 0;
}