/*Algdat øving 2, av Erland Sundsdal,
 Solfrid Emblem Holte, Martha Luise Vogel og 
 Stian Closs Walmann*/
#include <stdio.h>
#include <time.h>

//method one
double methodOne(double x, int n) {
	if (n == 1) {
		return x;
	} else {
		return x + methodOne(x, n - 1);
	}
}

/*
	Denne metoden er lineær i forhold til n
	Den kjører n ganger
	Θ(n)
	kjøretid ved n=100000 er 0.002325 sekunder
*/

//method two
double methodTwo(double x, int n) {
	if (n == 1) {
		return x;
	} else if (n % 2 == 0) {
		return methodTwo(x + x, n / 2);
	} else {
		return x + methodTwo(x + x,(n - 1) / 2);	
	}
}

/*
	Denne metoden er logaritmisk i forhold til n
	Den kjører antall ganger du kan halvere n før man når 1
	Θ(log(n))
	kjøretid ved n=100000 er 0.000001 sekunder
*/


int main() {
	double x = 1.23;
	int n = 100000;

	clock_t start1 = clock();
	double methodOneResult = methodOne(x,n);
	clock_t stop1 = clock();

	double time1 = (double)(stop1-start1)/ CLOCKS_PER_SEC;

	clock_t start2 = clock();
	double methodTwoResult = methodTwo(x,n);
	clock_t stop2 = clock();

	double time2 = (double)(stop2-start2)/ CLOCKS_PER_SEC;


	printf("Method 1: %f \nTime taken: %f seconds\n", methodOneResult, time1);
	printf("Method 2: %f \nTime taken: %f seconds\n", methodTwoResult, time2);
	return 0;
}
