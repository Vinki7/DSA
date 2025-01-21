#include <stdio.h>
#include <stdlib.h>
#define MAX_FLOAT_LEN 11
#define MAX_BUFFER_SIZE 22

typedef struct dataLog{
    int negative;
    long long int wholePart;
    long long int floatingPart;
}Number;

long long int customAtoll(char arr[], int *negative, long int *dotIndex){
    long long int result = 0;
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

    if (arr[i] == ',')
    {
        *dotIndex = i;
    }

    if (negativeFlag) {// Negative sign is handled here
        result = -result;
    }

    *negative = negativeFlag;
    return result;
}

void fillBuffer(char *floatBuffer, char *buffer, int dotIndex){
    floatBuffer[MAX_FLOAT_LEN - 1] = '\0';// \0 at the end
    char c; // assist var
    int endFlag = 0;
    for (int i = 0; i < (MAX_FLOAT_LEN-1); i++)// range of index <0,9>
    {
        if (endFlag)
        {
            floatBuffer[i] = '0';
        }else{
            c = buffer[dotIndex+i+1];
            if (c == '\0')
            {
                endFlag = 1;
                i--;
            }else{
                floatBuffer[i] = c;
            }
        }
    }
}
void numberFormating(Number *number){
    //char bufferFloating[MAX_FLOAT_LEN];
    char buffer[MAX_BUFFER_SIZE];
    char floatBuffer[MAX_FLOAT_LEN];
    long int dotIndex = -1;
    int tmp;
    long int tmp2;
    scanf("%s", buffer);// whole loaded as String
    number->wholePart = customAtoll(buffer, &number->negative, &dotIndex);
    if (dotIndex == -1 || buffer[dotIndex+1] == '\0')
    {
        number->floatingPart = 0;
    }else{
        fillBuffer(floatBuffer, buffer, dotIndex);
        number->floatingPart = (number->negative)?customAtoll(floatBuffer, &tmp, &tmp2)*(-1):customAtoll(floatBuffer, &tmp, &tmp2);
    }
    //printf("%lld,%lld\n", number->wholePart, number->floatingPart);
}

int compareNumbers(Number numberFromInterval, Number searchedNumber){
    if (searchedNumber.wholePart < numberFromInterval.wholePart)
    {
        return -1;
    }else if(searchedNumber.wholePart > numberFromInterval.wholePart){
        return 1;
    }else{
        if (searchedNumber.floatingPart > numberFromInterval.floatingPart)
        {
            return 1;
        }else if (searchedNumber.floatingPart < numberFromInterval.floatingPart)
        {
            return -1;
        }else{
            return 0;
        }
    }
}

int binarySearch(Number *listOfNumbers, Number searchedNumber, long long int sizeOfList){
    long int leftEdge = 0, rightEdge = (sizeOfList - 1), middle;
    int comparison;

    while (leftEdge <= rightEdge)
    {
        middle = (leftEdge + rightEdge)/2;
        comparison = compareNumbers(listOfNumbers[middle], searchedNumber);

        if (comparison < 0)// lower than middle
        {
            rightEdge = middle - 1;
        }else if (comparison > 0)// higher than middle
        {
            leftEdge = middle + 1;
        }else{
            return middle + 1;// Equal!
        }
    }
    return 0;
}

int main (void){
    long int inputSize, searchSize;

    scanf("%ld", &inputSize);
    Number* numbers = (Number*)malloc(sizeof(Number)*inputSize);
    for (long int i = 0; i < inputSize; i++)
    {
        numberFormating(&numbers[i]);
    }
    scanf("%ld", &searchSize);
    for (long int i = 0; i < searchSize; i++)
    {
        Number searchedNumber;
        numberFormating(&searchedNumber);
        //printf("%d\n", compareNumbers(numbers[i], searchedNumber));
        (i < (searchSize - 1))?printf("%d\n", binarySearch(numbers, searchedNumber, inputSize)):printf("%d", binarySearch(numbers, searchedNumber, inputSize));  
    }
    
    free(numbers);
    return 0;
}