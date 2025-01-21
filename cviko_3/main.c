#include <stdlib.h>
#include <stdio.h>
#define INPUT_BUFFER_SIZE 50001
#define STRING_BUFFER_SIZE 32

void formatNumber(long long int *wholePart, long long int *floatingPart){
    long long int wholePartTmp = 0;
    long long int floatingPartTmp = 0;
    char buffer[STRING_BUFFER_SIZE];//32 bitov
    int dotFlag = 0;// flag - ak je precitana ciarka v retazci, dotFlag = 1
    int dotIndex = 0;// pocitanie cisel za ciarkou 

    scanf("%s", buffer);// nacitanie celeho cisla do Stringu
    for (int i = 0; i < STRING_BUFFER_SIZE; i++){
        if (buffer[i] == ','){
            dotFlag = 1;
        }
        else if (buffer[i] == '\0'){// koniec retazca
            break;
        }
        else if (dotFlag == 0){
            if (buffer[i] != '-')// preskocenie znamienka
            {
                wholePartTmp = wholePartTmp * 10 + (buffer[i] - '0');
            }
        }
        else{// tu by stacila len else cast
            if (buffer[i] == '0'){
                floatingPartTmp *= 10;
            }else {
                floatingPartTmp = floatingPartTmp * 10 + (buffer[i] - '0');
            }
            dotIndex++;
        }
    }
    for (int i = dotIndex; i < 10; i++){// od pozicie destineho rozvoja sa prihadzuju nuly
        floatingPartTmp *= 10;
    }
    
    if (buffer[0] == '-'){// prehadzovanie na zaporne
        wholePartTmp *= -1;
        floatingPartTmp *= -1;
    }
    *wholePart = wholePartTmp;
    *floatingPart = floatingPartTmp;
}

int binarySearchAlgorithm(int size, long long int wholeArray[], long long int floatingArray[], long long int wholeSearch, long long int floatingSearch){
    int leftEdge = 0, rightEdge = (size-1), middle;
    
    while (leftEdge <= rightEdge)// ak je rovny, hladane cislo na pozicii / neexistuje
    {
        if (rightEdge > (size-1) || leftEdge < 0)
        {
            break;
        }
        middle = (leftEdge + rightEdge)/2;// stred
        if (wholeArray[middle] == wholeSearch && floatingArray[middle] == floatingSearch){// zhoda v celom aj desatinom
            return middle + 1; // Found!
        }else if (wholeArray[middle] > wholeSearch || ((wholeArray[middle] > wholeSearch) && (floatingArray[middle] > floatingSearch))){// cislo vacsie ako hladane cislo
            rightEdge = middle - 1; // Adjust edge for the left half
        }else {// cislo mensie ako hladane cislo
            leftEdge = middle + 1; // Adjust edge for the right half
        }
    }
    
    return 0;
}

int main(void){
    long long int wholeTmp;
    long long int floatingTmp;

    int inputSize = 0;
    scanf("%d", &inputSize);

    long long int *whole = (long long int*)malloc(inputSize * sizeof(long long int));
    long long int *floating = (long long int*)malloc(inputSize * sizeof(long long int));

    for (int i = 0; i < inputSize; i++){
        formatNumber(&whole[i], &floating[i]);
    }
    
    int searchSize = 0;
    scanf("%d", &searchSize);

    for (int i = 0; i < searchSize; i++)
    {
        formatNumber(&wholeTmp, &floatingTmp);
        (i < (searchSize-1))?printf("%d\n", binarySearchAlgorithm(inputSize, whole, floating, wholeTmp, floatingTmp)):printf("%d", binarySearchAlgorithm(inputSize, whole, floating, wholeTmp, floatingTmp));
    }
    
    free(whole);
    free(floating);
    
    return 0;
}
