#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void bytt(int *i, int *j) {
	int k = *j;
	*j = *i;
	*i = k;
}

void boblesort(int *t, int v, int h) {
	for (int i = h; i > v; i--) {
		for (int j = v; j < i; j++) {
			if (t[j] > t[j + 1]) {
				bytt(&t[j], &t[j + 1]);
			}
		}
	}
}

int splitt(int *t, int v, int h) {
	int pivot = t[(h + v) / 2];
	int i = v - 1;
	int j = h+1;
	while (1) {
		do { i++; } while (t[i] < pivot);

		do { j--; } while (t[j] > pivot);

		if (i >= j) {
			return j;
		}
		bytt(&t[i], &t[j]);
	}
}

void quicksort(int*t, int v, int h) {
	if (h - v > 2 ) {
		int delepos = splitt(t, v, h);
		quicksort(t, v, delepos);
		quicksort(t, delepos + 1, h);
	} else{
		boblesort(t, v, h);	
	}
}

int main() {
	int size = 50000000;
	int *randomArray = (int*) malloc(size *sizeof(int));

	srand(time(NULL));
	for(int i = 0; i < size; i++) {
		if (i % 2) {
			randomArray[i] = rand() % (size/10);
		} else {
			randomArray[i] = 42;
		}
	}

	int totalBefore = 0;
	for (int i = 0; i < size; i++) {
		totalBefore += randomArray[i];
	}


	printf("Usortert (første 20 elementer):\n");
	for (int i = 0; i < 20; i++) {
		printf("%d ", randomArray[i]);
	}
	printf("\n");

	clock_t start1 = clock();
	quicksort(randomArray, 0, size-1);
	clock_t stop1 = clock();

	double timeSpent1 = (double) (stop1 - start1) / CLOCKS_PER_SEC;

	printf("Brukte %f sekunder. \n", timeSpent1);

	printf("Sortert (første 20 elementer):\n");
	for (int i = 0; i < 20; i++) {
		printf("%d ", randomArray[i]);
	}
	printf("\n");

	for (int i = 1; i < size; i++) {
		if (randomArray[i] < randomArray[i-1]) {
			printf("Sortert feil: %d er mindre enn %d \n", randomArray[i], randomArray[i - 1]);
			break;
		}
	}

	int totalAfter = 0;
	for (int i = 0; i < size; i++) {
		totalAfter += randomArray[i];
	}
	printf("differanse mellom sortert og usortert: %d ", totalBefore-totalAfter);
	printf("\n");

	clock_t start2 = clock();
	quicksort(randomArray, 0, size-1);
	clock_t stop2 = clock();

	double timeSpent2 = (double) (stop2 - start2) / CLOCKS_PER_SEC;
	printf("Brukte %f sekunder på ny sortering.", timeSpent2);
	
	free(randomArray);

	return 0;
}
