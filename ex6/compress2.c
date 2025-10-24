#include <stdio.h>
#include <stdlib.h>

struct Node {
  unsigned sequence;
  unsigned frequency;
  struct Node *left, *right;
};

struct MinHeap {
	unsigned size;
	unsigned capacity;
	struct MinheapNode** array;
};



struct Node* newNode(unsigned sequence, unsigned frequency) {
  struct Node* node = (struct Node*)malloc(sizeof(struct Node));
  node->sequence = sequence;
  node->frequency = frequency;
  node->left = node->right=NULL;
  return node;
}


int main() {
	FILE *fp = fopen("diverse.lyx", "rb");

	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	rewind(fp);

	unsigned char *buffer = (unsigned char *)malloc(filesize);

	fread(buffer, 1, filesize, fp);
	fclose(fp);
	printf("Filstørrelse: %ld bytes\n", filesize);

	unsigned int frequency[256] = {0};
	for (long i = 0; i < filesize; i++) {
			frequency[buffer[i]]++;
	}

	int uniqueBytes = 0;
	for (int i = 0; i<256; i++) {
		if (frequency[i]>0) {
			uniqueBytes++;
		}
	}

	struct Node** nodes = (struct Node**)malloc(uniqueBytes * sizeof(struct Node*));
	int nodeIndex = 0;

	for (int i = 0; i < 256; i++) {
			if (frequency[i] > 0) {
					nodes[nodeIndex] = newNode(i, frequency[i]);
					nodeIndex++;
			}
	}

  for (int i = 0; i < uniqueBytes; i++) {
    free(nodes[i]);
  }
  free(buffer);
  return 0;
}
