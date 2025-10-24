#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

// Structure for a node in the Huffman tree
struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// Structure for a min heap (priority queue)
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// Function to create a new min heap node
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Function to create a min heap
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Function to swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Standard function to heapify at given index
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Function to check if size of heap is 1 or not
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Function to extract minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Function to insert a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Function to build min heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Function to create and build min heap
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

// Function to check if this node is leaf
int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

// Function to build Huffman Tree
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;

    // Create a min heap of capacity equal to size
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {
        // Extract the two minimum freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // Create a new internal node with frequency equal to the
        // sum of the two nodes frequencies. Make the two extracted
        // node as its left and right children and add this node
        // to the min heap
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }

    // The remaining node is the root node and the tree is complete
    return extractMin(minHeap);
}

// Function to store Huffman codes in an array
void storeCodes(struct MinHeapNode* root, int arr[], int top, char codes[][MAX_TREE_HT]) {
    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }

    // Assign 1 to right edge and recur
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }

    // If this is a leaf node, then it contains one of the input
    // characters, store the code
    if (isLeaf(root)) {
        int i;
        for (i = 0; i < top; ++i) {
            codes[root->data][i] = arr[i] + '0';
        }
        codes[root->data][i] = '\0';
    }
}

// Function to print codes from the root of Huffman Tree
void printCodes(struct MinHeapNode* root, int arr[], int top) {
    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    // Assign 1 to right edge and recur
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    // If this is a leaf node, then it contains one of the input
    // characters, print the character and its code from arr[]
    if (isLeaf(root)) {
        printf("'%c': ", root->data);
        for (int i = 0; i < top; ++i)
            printf("%d", arr[i]);
        printf("\n");
    }
}

// Function to encode a string using Huffman codes
char* encode(char* text, char codes[][MAX_TREE_HT]) {
    int len = strlen(text);
    char* encoded = (char*)malloc(len * MAX_TREE_HT * sizeof(char));
    encoded[0] = '\0';
    
    for (int i = 0; i < len; i++) {
        if (strlen(codes[(unsigned char)text[i]]) > 0) {
            strcat(encoded, codes[(unsigned char)text[i]]);
        }
    }
    return encoded;
}

// Function to decode the encoded string
char* decode(struct MinHeapNode* root, char* encodedString) {
    int len = strlen(encodedString);
    char* decoded = (char*)malloc((len + 1) * sizeof(char));
    int index = 0;
    struct MinHeapNode* current = root;
    
    for (int i = 0; i < len; i++) {
        if (encodedString[i] == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (isLeaf(current)) {
            decoded[index++] = current->data;
            current = root;
        }
    }
    decoded[index] = '\0';
    return decoded;
}

// Main function that constructs Huffman tree and prints codes
void HuffmanCodes(char data[], int freq[], int size) {
    // Construct Huffman Tree
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

    // Print Huffman codes using the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;
    printf("\nHuffman Codes:\n");
    printCodes(root, arr, top);
    
    // Store codes in array for encoding/decoding
    char codes[256][MAX_TREE_HT];
    for (int i = 0; i < 256; i++) {
        codes[i][0] = '\0';
    }
    storeCodes(root, arr, 0, codes);
    
    // Create a test string using the characters we have codes for
    char test_string[100] = "";
    for (int i = 0; i < size && i < 10; i++) {
        char temp[2] = {data[i], '\0'};
        strcat(test_string, temp);
    }
    
    if (strlen(test_string) > 0) {
        printf("\nTest string: %s\n", test_string);
        
        char* encoded = encode(test_string, codes);
        printf("Encoded: %s\n", encoded);
        
        char* decoded = decode(root, encoded);
        printf("Decoded: %s\n", decoded);
        
        if (strlen(encoded) > 0) {
            printf("Compression ratio: %.2f%%\n", 
                   (1.0 - (double)strlen(encoded) / (strlen(test_string) * 8)) * 100);
        }
        
        free(encoded);
        free(decoded);
    }
}

// Driver program to test above functions
int main() {
    printf("Huffman Coding Algorithm\n");
    printf("========================\n");
    
    // Example 1: Simple characters with frequencies
    char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    int freq[] = { 5, 9, 12, 13, 16, 45 };
    int size = sizeof(arr) / sizeof(arr[0]);

    printf("Example 1 - Characters and their frequencies:\n");
    for (int i = 0; i < size; i++) {
        printf("'%c': %d\n", arr[i], freq[i]);
    }
    
    HuffmanCodes(arr, freq, size);
    
    printf("\n==================================================\n");
    
    // Example 2: Text analysis
    char text[] = "this is an example of huffman coding";
    printf("Example 2 - Analyzing text: \"%s\"\n", text);
    
    // Count character frequencies
    int charFreq[256] = {0};
    int textLen = strlen(text);
    
    for (int i = 0; i < textLen; i++) {
        charFreq[(unsigned char)text[i]]++;
    }
    
    // Create arrays for characters and their frequencies
    char chars[256];
    int freqs[256];
    int uniqueChars = 0;
    
    for (int i = 0; i < 256; i++) {
        if (charFreq[i] > 0) {
            chars[uniqueChars] = (char)i;
            freqs[uniqueChars] = charFreq[i];
            uniqueChars++;
        }
    }
    
    printf("Character frequencies in the text:\n");
    for (int i = 0; i < uniqueChars; i++) {
        if (chars[i] == ' ') {
            printf("'space': %d\n", freqs[i]);
        } else {
            printf("'%c': %d\n", chars[i], freqs[i]);
        }
    }
    
    HuffmanCodes(chars, freqs, uniqueChars);
    
    // Demonstrate full text encoding/decoding
    printf("\n==================================================\n");
    printf("Full text encoding/decoding demonstration:\n");
    
    struct MinHeapNode* root = buildHuffmanTree(chars, freqs, uniqueChars);
    char codes[256][MAX_TREE_HT];
    for (int i = 0; i < 256; i++) {
        codes[i][0] = '\0';
    }
    int codeArr[MAX_TREE_HT];
    storeCodes(root, codeArr, 0, codes);
    
    printf("Original: \"%s\"\n", text);
    char* encoded = encode(text, codes);
    printf("Encoded length: %lu bits\n", strlen(encoded));
    printf("Original length: %d bytes (%d bits)\n", textLen, textLen * 8);
    
    char* decoded = decode(root, encoded);
    printf("Decoded: \"%s\"\n", decoded);
    printf("Encoding successful: %s\n", strcmp(text, decoded) == 0 ? "YES" : "NO");
    
    if (strlen(encoded) > 0) {
        printf("Compression ratio: %.2f%%\n", 
               (1.0 - (double)strlen(encoded) / (textLen * 8)) * 100);
    }
    
    free(encoded);
    free(decoded);
    
    return 0;
}
