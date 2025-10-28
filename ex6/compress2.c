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

struct MinheapNode* extractMin(struct MinHeap* minHeap) {
    struct MinheapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinheapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->node->frequency < minHeap->array[(i - 1) / 2]->node->frequency) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
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

struct Node* buildHuffmanTree(unsigned sequences[], unsigned freq[], int size) {
    struct Node *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newMinheapNode(newNode(sequences[i], freq[i]));

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap)->node;
        right = extractMin(minHeap)->node;

        top = newNode(0, left->frequency + right->frequency);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, newMinheapNode(top));
    }

    return extractMin(minHeap)->node;
}

void countFrequencies(const char* filename, unsigned freq[256]) {
    FILE* file = fopen(filename, "rb");
    if (!file) return;

    int c;
    while ((c = fgetc(file)) != EOF) {
        freq[c]++;
    }
    fclose(file);
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

char* encodeFile(const char* filename, char codes[][256], long* encodedLength) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* encoded = (char*)malloc(fileSize * 256);
    *encodedLength = 0;

    int c;
    while ((c = fgetc(file)) != EOF) {
        char* code = codes[c];
        for (int i = 0; code[i] != '\0'; i++) {
            encoded[*encodedLength] = code[i];
            (*encodedLength)++;
        }
    }
    encoded[*encodedLength] = '\0';

    fclose(file);
    return encoded;
}

void writeTree(struct Node* root, FILE* file) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        fputc('1', file);
        fputc(root->sequence, file);
    } else {
        fputc('0', file);
        writeTree(root->left, file);
        writeTree(root->right, file);
    }
}

void writeCompressedFile(const char* outputFile, struct Node* root, const char* encoded, long encodedLength) {
    FILE* file = fopen(outputFile, "wb");
    if (!file) return;
    
    writeTree(root, file);
    
    fputc('\n', file);
    
    fwrite(&encodedLength, sizeof(long), 1, file);
    
    unsigned char byte = 0;
    int bitCount = 0;
    
    for (long i = 0; i < encodedLength; i++) {
        byte = (byte << 1) | (encoded[i] - '0');
        bitCount++;
        
        if (bitCount == 8) {
            fputc(byte, file);
            byte = 0;
            bitCount = 0;
        }
    }
    
    if (bitCount > 0) {
        byte = byte << (8 - bitCount);  
        fputc(byte, file);
    }
    
    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* originalFile = fopen(argv[1], "rb");
    fseek(originalFile, 0, SEEK_END);
    long originalSize = ftell(originalFile);
    fclose(originalFile);

    unsigned freq[256] = {0};
    countFrequencies(argv[1], freq);

    unsigned sequences[256];
    unsigned frequencies[256];
    int size = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            sequences[size] = i;
            frequencies[size] = freq[i];
            size++;
        }
    }

    struct Node* root = buildHuffmanTree(sequences, frequencies, size);

    char codes[256][256] = {0};
    int arr[100];
    int top = 0;
    storeCodes(root, arr, top, codes);

    long encodedLength;
    char* encoded = encodeFile(argv[1], codes, &encodedLength);

    writeCompressedFile("output.huff", root, encoded, encodedLength);

    FILE* compressedFile = fopen("output.huff", "rb");
    fseek(compressedFile, 0, SEEK_END);
    long compressedSize = ftell(compressedFile);
    fclose(compressedFile);

    printf("Original file size: %ld bytes\n", originalSize);
    printf("Compressed file size: %ld bytes\n", compressedSize);
    printf("Compression ratio: %.2f%%\n", (1.0 - (double)compressedSize / originalSize) * 100);

    free(encoded);
    return 0;
}