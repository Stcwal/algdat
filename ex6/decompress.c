#include <stdio.h>
#include <stdlib.h>

struct Node {
    unsigned sequence;
    unsigned frequency;
    struct Node *left, *right;
};

struct Node* newNode(unsigned sequence) {
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->left = temp->right = NULL;
    temp->sequence = sequence;
    temp->frequency = 0;
    return temp;
}

struct Node* readTree(FILE* file) {
    int bit = fgetc(file);
    
    if (bit == '1') {
        unsigned char ch = fgetc(file);
        return newNode(ch);
    } else if (bit == '0') {
        struct Node* node = newNode(0);
        node->left = readTree(file);
        node->right = readTree(file);
        return node;
    }
    
    return NULL;
}

void decodeData(struct Node* root, FILE* inputFile, FILE* outputFile, long totalBits) {
    struct Node* current = root;
    unsigned char byte;
    long bitsRead = 0;
    
    while (fread(&byte, 1, 1, inputFile) == 1) {
        for (int i = 7; i >= 0 && bitsRead < totalBits; i--) {
            int bit = (byte >> i) & 1;
            bitsRead++;
            
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            if (!current->left && !current->right) {
                fputc(current->sequence, outputFile);
                current = root; 
            }
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* inputFile = fopen(argv[1], "rb");
    fseek(inputFile, 0, SEEK_END);
    long compressedSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);
    
    struct Node* root = readTree(inputFile);
    
    fgetc(inputFile);
    
    long totalBits;
    fread(&totalBits, sizeof(long), 1, inputFile);
    
    FILE* outputFile = fopen("decompressed.txt", "wb");
    
    decodeData(root, inputFile, outputFile, totalBits);
    
    fclose(inputFile);
    fclose(outputFile);
    
    FILE* decompFile = fopen("decompressed.txt", "rb");
    fseek(decompFile, 0, SEEK_END);
    long decompressedSize = ftell(decompFile);
    fclose(decompFile);
    
    printf("Compressed file size: %ld bytes\n", compressedSize);
    printf("Decompressed file size: %ld bytes\n", decompressedSize);
    
    return 0;
}