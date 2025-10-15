#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAMES 128 //2^7
#define MAX_NAME_LENGTH 100
#define MAX_NUMBERS 16777216 //2^24

int size = 10000000;
const unsigned A = 2654435769U;

int* numbers;

struct node {
  char name[MAX_NAME_LENGTH];
  struct node* next;
};

int hashName(char name[]) {
  int hashNumber = 0;
  int strsize = strlen(name);
  for (int i = 0; i < strsize;i++) {
    hashNumber += (unsigned char)name[i]*(i+1);
  }
  return hashNumber % MAX_NAMES;
}

int oppg1() {
  char fname[MAX_NAME_LENGTH];
  struct node* names[MAX_NAMES] = {NULL};
  int counter = 0;  

  FILE *file = fopen("navn.txt", "r");
  int collisionCounter = 0;

  while ((fgets(fname, MAX_NAME_LENGTH , file))) {
    fname[strcspn(fname, "\n")] = '\0';
    int hashedName = hashName(fname);
    
    struct node* new = (struct node*)malloc(sizeof(struct node));
    strcpy(new->name, fname);
    if (names[hashedName] == NULL){
      names[hashedName] = new;
    } else {
      new->next = names[hashedName];
      collisionCounter++;
    printf("Kollisjon på %s og %s\n",new->name, names[hashedName]);
    }    
    counter++;
  }

  char searchName[MAX_NAME_LENGTH] =  "Alexander Oddsen,Arne";
  struct node* searchNode = names[hashName(searchName)];
  
  double loadFactor = (double)counter/(double)MAX_NAMES;
  double collisionFactor = (double)collisionCounter/(double)counter;
  
  
  printf("Lastfaktor er: %f \n", loadFactor);
  printf("Kollisjonsfaktor er: %f \n",collisionFactor);
  printf("Du har søkt på navnet %s, og har fått ut navnet %s\n\n\n",searchName, searchNode->name);
  
  
  
  fclose(file);
  return 0;
}

int hashInt(int k, int x) {
  return (k * A) >> (32 - x);
}

int* oppg2_2() {
  int* numbers = (int*)malloc(size * sizeof(int));
  numbers[0] = rand() % size;
  for (int i = 1; i < size; i++) {
    numbers[i] = numbers[i-1] + (rand() % 1000);
  }
  for (int i = 0; i < size; i++) {
    int randomIndex = rand() % size; 
    int temp = numbers[i];
    numbers[i] = numbers[randomIndex];
    numbers[randomIndex] = temp;
  }

  return numbers;
}


int oppg2_3_linear_probe(int* numbers) {
  int* hashTable = (int*)malloc(MAX_NUMBERS * sizeof(int));
  int counter = 0;
  clock_t start = clock();
  for (int i = 0; i< size; i++) {
    int hashedNumber = hashInt(numbers[i], 24);
    hashedNumber = hashedNumber % MAX_NUMBERS;

    while (hashTable[hashedNumber]!= NULL) {
      hashedNumber = (hashedNumber + 1) % MAX_NUMBERS;
      counter++;
    }
    hashTable[hashedNumber] = numbers[i];
  }
  clock_t stop = clock();

  printf("Linear probe takes %f seconds\n", (double)(stop-start)/CLOCKS_PER_SEC);
  printf("Linear probe gives %d collisions\n", counter);

  return 0;
}

int oppg2_3_double_hash(int* numbers) {
  int* hashTable = (int*)malloc(MAX_NUMBERS * sizeof(int));
  int counter = 0;
  clock_t start = clock();


  for (int i = 0; i< size; i++) {
    int hashedNumber = hashInt(numbers[i], 24);
    hashedNumber = hashedNumber % MAX_NUMBERS;

    int step = (numbers[i] % (MAX_NUMBERS - 1)) + 1;
    while (hashTable[hashedNumber]!= NULL) {
      hashedNumber = (hashedNumber + step) % MAX_NUMBERS;
      counter++;
    }
    hashTable[hashedNumber] = numbers[i];
  }

  clock_t stop = clock();

  printf("Double hash takes %f seconds\n", (double)(stop-start)/CLOCKS_PER_SEC);
  printf("Double hash gives %d collisions\n", counter);

  return 0;
}

int main() {
  printf("Del 1\n");
  oppg1();
  printf("Del 2\n");
  numbers = oppg2_2();
  for (int i = 0; i < 5; i++) {
    switch (i) {
      case (0):
      size = size*0.5;
      printf("50%c\n",'%');
      break;
      case (1):
      size = size*0.8;
      printf("80%c\n",'%');
      break;
      case (2):
      size = size*0.9;
      printf("90%c\n",'%');
      break;
      case (3):
      size = size*0.99;
      printf("99%c\n",'%');
      break;
      case (4):
      size = size;
      printf("100%c\n",'%');
      break;
      default:
      break;
    }
    oppg2_3_linear_probe(numbers);
    oppg2_3_double_hash(numbers);
  }
  free(numbers);
  return 0;
}