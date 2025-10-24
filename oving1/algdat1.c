//Imports
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//main method
int main() {
 //lage random array
        int size = 1000; //10000 og 100000
        int randomArray[size];
        int i;
        for (i = 0; i < size; i++) {
                randomArray[i] = (rand() % 10) -5;
        }

 //convert to fixed values compared to 0

        int fixedArray[size];
        fixedArray[0] = randomArray[0];
        for (i=1; i < size; i++) {
                fixedArray[i] = fixedArray[i-1] + randomArray[i];
        }

 //start timer
        clock_t start = clock();

 //algoritme for å finne største differanse
        int maxDifference = 0;
        int buyDay = 0;
        int sellDay = 0;
        int j;
        int n;
        for (j=0; j < size; j++) {
                for (n=j; n < size; n++) {
                        if (fixedArray[j] - fixedArray[n] > maxDifference) {
                                maxDifference = fixedArray[j] - fixedArray[n];
                                buyDay = j;
                                sellDay = n;
                        }
                }
        }

 //stop timer
        clock_t end = clock();
        double time_spent = (double)(end-start)/ CLOCKS_PER_SEC;
        printf("Time taken; %f seconds\n", time_spent);

 //print result
        printf("The biggest difference is: %d\n", maxDifference);
        printf("Buy on day %d and sell on day %d\n", buyDay, sellDay);
        return 0;

}
/*
Oppgave 2

Kompleksiteten til algoritmen er O(n^2) fordi vi har to nestede løkker som itererer over hele arrayet. 
Dette betyr at for hver dag (j) vi vurderer å kjøpe, må vi sjekke alle påfølgende dager (n) for å finne den beste dagen å selge på. 


Oppgave 3

Vi sjekker tidsbruket til algoritmen ved å endre størrelsen på arrayet med 10x. dette resulterer i en 100x av tidsbruken. Noe som reflekterer teorien i oppgaveteksten. 
*/