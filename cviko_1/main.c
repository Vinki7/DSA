#include <stdio.h>
#include <stdlib.h>

long long unsigned int euclid(long long unsigned higherNumber, long long unsigned int lowerNumber){
    long long unsigned int temp;
    while(lowerNumber != 0){
        temp  = lowerNumber;
        lowerNumber = higherNumber%lowerNumber;
        higherNumber = temp;
    }
    return higherNumber;
}

int main(void){
    int count, i;
    long long unsigned int higherNumber, lowerNumber;
    scanf("%d", &count);
    for (i = 0; i < count; i++){
        scanf("%llu %llu", &higherNumber, &lowerNumber);
        if(i == (count-1)){
            (higherNumber < lowerNumber)? printf("%llu", euclid(lowerNumber, higherNumber)):printf("%llu", euclid(higherNumber, lowerNumber));

        }else{
            (higherNumber < lowerNumber)? printf("%llu\n", euclid(lowerNumber, higherNumber)):printf("%llu\n", euclid(higherNumber, lowerNumber));
        }
    }
    return 0;
}