#include <stdlib.h>
#include <stdio.h>

int decide(int number){
    if ((number % 2 == 0)&&(number != 2)){
        return 0;
    }
    for (int divisor = 3; divisor*divisor <= number; divisor +=2){
        if (number % divisor == 0)
        {
            return 0;
        }
    }
    return 1;
}

int main(){
    int intervalMin, intervalMax, firstPrime = 0;
    scanf("%d %d", &intervalMin, &intervalMax);
    if (intervalMin < 2){
        intervalMin = 2;
    }
    for (int i = intervalMin; i <= intervalMax; i++){
        if (decide(i)){
            (firstPrime == 0)? printf("%d", i):printf(", %d", i);
            firstPrime++;
        }
    }
}