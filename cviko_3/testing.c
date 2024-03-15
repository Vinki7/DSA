#include <stdio.h>
#include <stdlib.h>
#define STRING_BUFFER_SIZE 11

typedef struct dataLog{
    long long int wholePart;
    char floatingPart[STRING_BUFFER_SIZE];
}Number;

void formatNumber(Number *number){// pointer to the structure in the array
    char dot;
    char buffer[STRING_BUFFER_SIZE];
    int endFlag = 0;
    scanf("%lld%c", &number->wholePart, &dot);
    if (dot != ',')
    {
        for (int i = 0; i < STRING_BUFFER_SIZE - 1; i++) {
                number->floatingPart[i] = '0'; // Replace with '0'
        }
    }else{
        scanf("%s", buffer);
        for (int i = 0; i < STRING_BUFFER_SIZE - 1; i++) {
            char c = buffer[i];
            if ((buffer[i] == '\0')&&(endFlag == 0)){ // Not a digit
                endFlag = 1;
            }
            if(endFlag == 1){
                number->floatingPart[i] = '0';
            }else{
                number->floatingPart[i] = buffer[i];
            }
        }
    }
    number->floatingPart[STRING_BUFFER_SIZE - 1] = '\0'; // Null-terminate the string
}

int stringCompare(char firstString[], char secondString[]) {
    for(int i = 0; i < STRING_BUFFER_SIZE; i++){
        if (firstString[i] != secondString[i])
        {
            return 0;// Different
        }
    }
    return 1;// Equal
}

int binarySearchAlgorithm(long long int size, Number *numbers, Number searchFor){//error in binary search
    long long int leftEdge = 0, rightEdge = (size-1), middle;

    while (leftEdge <= rightEdge)// if is higher or equal, number doesn't exist
    {
        middle = (leftEdge + rightEdge)/2;// middle of the interval
        long long int wholePart = numbers[middle].wholePart;
        int comparison = stringCompare(numbers[middle].floatingPart, searchFor.floatingPart);
        if (wholePart == searchFor.wholePart && comparison == 1){// equal in whole and floating part
            return middle + 1; // Found!
        }else if (wholePart < searchFor.wholePart || (wholePart == searchFor.wholePart && comparison == 0)){// number is higher than searched one
            leftEdge = middle + 1; // Adjust edge for the left half
        }else {// number is lower than the searched one
            rightEdge = middle - 1; // Adjust edge for the right half
        }
    }
    
    return 0;
}

int main() {
    long long int inputSize, searchSize;
    scanf("%lld", &inputSize);

    Number *numbers = (Number*)malloc(inputSize * sizeof(Number));
    for (int i = 0; i < inputSize; i++)
    {
        formatNumber(&numbers[i]);// input address of the structure in the array
        //printf("%lld,%s\n", numbers[i].wholePart, numbers[i].floatingPart);// delete later on
    }
    scanf("%lld", &searchSize);
    for (int i = 0; i < searchSize; i++)
    {
        Number searchFor;
        formatNumber(&searchFor);
        //printf("%lld,%s", searchFor.wholePart, searchFor.floatingPart);
        //printf("%d", stringCompare(numbers[0].floatingPart, searchFor.floatingPart));
        (i < (searchSize-1))?printf("%d\n", binarySearchAlgorithm(inputSize, numbers, searchFor)):printf("%d", binarySearchAlgorithm(inputSize, numbers, searchFor));
    }

    free(numbers);
    return 0;
}