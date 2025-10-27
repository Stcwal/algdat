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
struct MinheapNode {
	struct Node *node;
};

struct MinheapNode* newMinheapNode(struct Node *node) {
	struct MinheapNode *mhnode = (struct MinheapNode*)malloc(sizeof(struct MinheapNode));
	mhnode->node = node;
	return mhnode;
}

struct MinHeap* createMinHeap(unsigned capacity) {
	struct MinHeap *minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array = (struct MinheapNode**)malloc(capacity * sizeof(struct MinheapNode*));
	return minHeap;
}

static void swapMinheapNode(struct MinheapNode **a, struct MinheapNode **b) {
	struct MinheapNode *t = *a;
	*a = *b;
	*b = t;
}

void minHeapify(struct MinHeap *minHeap, int idx) {
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;

	if (left < (int)minHeap->size &&
		minHeap->array[left]->node->frequency < minHeap->array[smallest]->node->frequency)
		smallest = left;

	if (right < (int)minHeap->size &&
		minHeap->array[right]->node->frequency < minHeap->array[smallest]->node->frequency)
		smallest = right;

	if (smallest != idx) {
		swapMinheapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}

int isSizeOne(struct MinHeap *minHeap) {
	return minHeap->size == 1;
}

struct Node* extractMin(struct MinHeap *minHeap) {
	if (minHeap->size == 0) return NULL;
	struct MinheapNode *root = minHeap->array[0];
	struct Node *res = root->node;

	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	minHeap->size--;
	minHeapify(minHeap, 0);

	free(root);
	return res;
}

void insertMinHeap(struct MinHeap *minHeap, struct Node *node) {
	if (minHeap->size == minHeap->capacity) {
		/* reallocate if needed */
		minHeap->capacity *= 2;
		minHeap->array = (struct MinheapNode**)realloc(minHeap->array, minHeap->capacity * sizeof(struct MinheapNode*));
	}

	int i = minHeap->size++;
	minHeap->array[i] = newMinheapNode(node);

	while (i && minHeap->array[(i - 1) / 2]->node->frequency > minHeap->array[i]->node->frequency) {
		swapMinheapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

void buildMinHeap(struct MinHeap *minHeap) {
	int n = minHeap->size;
	for (int i = (n - 1) / 2; i >= 0; --i)
		minHeapify(minHeap, i);
}


struct Node* newNode(unsigned sequence, unsigned frequency) {
  struct Node* node = (struct Node*)malloc(sizeof(struct Node));
  node->sequence = sequence;
  node->frequency = frequency;
  node->left = node->right=NULL;
  return node;
}

void printCodes(struct Node *root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }
    
}

void storeCodes(struct Node *root, int arr[], int top, char codes[][256]) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }
    
    if (!root->left && !root->right) {
        for (int i = 0; i < top; i++) {
            codes[root->sequence][i] = arr[i] + '0';  
        }
        codes[root->sequence][top] = '\0';  
    }
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

    struct MinHeap *minHeap = createMinHeap(uniqueBytes);
    
    for (int i = 0; i < uniqueBytes; i++) {
        insertMinHeap(minHeap, nodes[i]);
    }
    
    while (!isSizeOne(minHeap)) {
        struct Node *left = extractMin(minHeap);
        struct Node *right = extractMin(minHeap);
        
        struct Node *parent = newNode(256, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        
        insertMinHeap(minHeap, parent);
    }
    
    struct Node *root = extractMin(minHeap);
    
    printf("Huffman-tre bygget! Root frequency: %u\n", root->frequency);
    
    // Generer og print koder
    int arr[256];
    printf("\nHuffman-koder:\n");
    printCodes(root, arr, 0);
    
    // Lagre koder for kompresjon
    char codes[256][256] = {0};
    storeCodes(root, arr, 0, codes);
    
    printf("\nEksempel - kode for byte 'A' (65): %s\n", codes[65]);

    free(minHeap->array);
    free(minHeap);
  for (int i = 0; i < uniqueBytes; i++) {
    free(nodes[i]);
  }
  free(buffer);
  return 0;
}
