#include <stdio.h>
#include <stdlib.h>
#define STRING_BUFFER_SIZE 11
#define ASSIST_BUFFER 20

typedef struct dataLoad
{
    long int wholePart;
    char floatingPart[STRING_BUFFER_SIZE];
    int negative;
}Number;

int findDot(char array[], char *breakPoint){
    int i = 0;
    char c = array[i];
    while (c != ','){
        c = array[i];
        if (c == '\0')// end of buffer, dot was not found
        {
            return -1;// string terminated
        }
        i++;   
    }
    *breakPoint = array[i];// solves the dot problem => outputs the char after dot
    return i-1;// position of the dot in the string
}

long int customAtol(char arr[], int *negative){
    int result = 0;
    int i = 0;
    char c = arr[i];
    int negativeFlag = 0;

    if (c == '\0' || c == ',') {// This handles an empty string
        return 0;
    }

    if (c == '-') {// this handles negative sign with flag
        negativeFlag = 1;
        i++;
    }
    
    while (arr[i] != '\0' && arr[i] != ',') {//  Processing digits from the String
        if (arr[i] >= '0' && arr[i] <= '9') {// Char validation
            result = result * 10 + (arr[i] - '0');
        } else {// Character out of interval, thus is invalid
            return -1;
        }
        i++;
    }

    if (negativeFlag) {// Negative sign is handled here
        result = -result;
    }

    *negative = negativeFlag;
    return result;
}

void formatNumber(Number *number){
    char buffer[ASSIST_BUFFER];// Just a buffer
    scanf("%s", buffer);
    char breakPoint;
    long int dotIndex = findDot(buffer, &breakPoint);// finds dot in the String
    
    number->wholePart = customAtol(buffer, &number->negative);// whole part can be stored
    
    if ((dotIndex == -1)||(dotIndex > 0 && breakPoint == '\0')){// checking the floating part
        for (int i = 0; i < (STRING_BUFFER_SIZE-1); i++)// filling the floating part with '0'
        {
            number->floatingPart[i] = '0';
        }
    }else 
    {
        int endFlag = 0;
        char c;
        for (int i = 0; i < (STRING_BUFFER_SIZE-1); i++)// filling the floating part
        {
            if (endFlag == 1)// if buffer has terminated, fill remainding chars with '0'
            {
                number->floatingPart[i] = '0';
            }else // if buffer has not terminated yet, fill with chars from the buffer
            {
                c = buffer[dotIndex+i+1];
                if (c == '\0')
                {
                    endFlag = 1;
                    i--;
                }else{
                    number->floatingPart[i] = c; 
                } 
            }
        }
    }
    number->floatingPart[10] = '\0';
}

int stringCompare(char *firstString, char *secondString, int firstNegative, int secondNegative) {// here maybe is error
    if (secondNegative > firstNegative)
    {
        return -1;
    }
    if (secondNegative < firstNegative)
    {
        return +1;
    }

    for(int i = 0; i < STRING_BUFFER_SIZE; i++){ //search the difference
        if (firstString[i] != secondString[i])// Differ? compare the value at the differing position
        {
            if (firstNegative & secondNegative)// If both are true = both are negative
            {
                if (secondString[i] > firstString[i])// searched has higher value? then searched is lower ex.: -0,025 < -0,023
                {
                    return -1;// Searched is lower!
                }else{// interval number has higher value, thus searched is bigger ex.: -1,23 > -1,28
                    return +1;// Searched is higher!
                }
            }else{
                if (secondString[i] < firstString[i])// interval has higher value? then searched is lower ex.: 0,025 > 0,023
                {
                    return -1;// Searched is lower!
                }else{// interval number has lower value, thus searched is bigger ex.: 1,23 < 1,28
                    return +1;// Searched is higher!
                }
            }
        }else{
            return 0;// Equal!
        }
    }   
}

int binarySearchAlgorithm(long long int size, Number *numbers, Number searchFor){//error in binary search
    long int leftEdge = 0, rightEdge = (size-1), middle;

    while (leftEdge <= rightEdge)
    {
        middle = (leftEdge + rightEdge)/2; //middle of the interval
        long int wholePart = numbers[middle].wholePart;// vybera stred na intervale => numbers[middle].wholePart
        if(searchFor.wholePart < wholePart){
            rightEdge = middle -1;// Adjusted edge - eliminate left side of the previous interval
        }else if (searchFor.wholePart > wholePart)
        {
            leftEdge = middle + 1;// Adjusted edge - eliminate right side of the previous interval
        }else{
            int comparison = stringCompare(numbers[middle].floatingPart, searchFor.floatingPart, numbers[middle].negative, searchFor.negative);
            if (comparison == 0)
            {
                return middle + 1;// Found!
            }else if (comparison == 1)
            {
                leftEdge = middle + 1;
            }else{
                rightEdge = middle - 1;
            }
        }
    }
    
/*
    while (leftEdge <= rightEdge)// if is higher or equal, number doesn't exist
    {
        middle = (leftEdge + rightEdge)/2;// middle of the interval
        long int wholePart = numbers[middle].wholePart;
        int comparison = stringCompare(numbers[middle].floatingPart, searchFor.floatingPart, numbers[middle].negative, searchFor.negative);
        
        if (wholePart == searchFor.wholePart && comparison == 1 && numbers[middle].negative == searchFor.negative){// equal in whole and floating part
            return middle + 1; // Found!
        }else if ((wholePart < searchFor.wholePart) || (wholePart == searchFor.wholePart  && comparison == 0)){// number is higher than searched one //edited condition
            leftEdge = middle + 1; // Adjust edge for the left half
        }else {// number is lower than the searched one
            rightEdge = middle - 1; // Adjust edge for the right half
        }
    }
*/    
    return 0;
}

int main(void){
    long int inputSize, searchSize;
    scanf("%lld", &inputSize);
    Number *numbers = (Number*)malloc(sizeof(Number)*inputSize);
    for (int i = 0; i < inputSize; i++)
    {
        formatNumber(&numbers[i]);
        //printf("%ld,%d_%s\n", numbers[i].wholePart, numbers[i].negative, numbers[i].floatingPart);
    }
    
    scanf("%lld", &searchSize);
    for (int i = 0; i < searchSize; i++)
    {
        Number searchFor;
        formatNumber(&searchFor);
        //printf("%lld,%s", searchFor.wholePart, searchFor.floatingPart);
        printf("%d\n", stringCompare(numbers[i].floatingPart, searchFor.floatingPart, numbers[i].negative, searchFor.negative));
        (i < (searchSize-1))?printf("%d\n", binarySearchAlgorithm(inputSize, numbers, searchFor)):printf("%d", binarySearchAlgorithm(inputSize, numbers, searchFor));
    }

    free(numbers);
    return 0;
}