#pragma warning(disable : 4996)
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "PriorityQueue.h"

#define ASCII_MAX 128
#define MAX_TREE_HEIGHT 256

/* ======= START: Debug ======= */
void PrintInorder(BinTreePtr tree)
{
    if (tree == NULL)
        return;
    PrintInorder(tree->left);
    printf("[[%c][%d]] ", tree->info.keys[0], tree->info.repetition);
    PrintInorder(tree->right);
}

void DebugNode(BinTreePtr node, char* msg)
{
    puts(msg);
    printf("Node At :%p [%c] [%d]\n LEFT %p RIGHT %p\n LEFT %d RIGHT %d\n", (void*)node, node->info.keys[0], node->info.repetition, (void*)node->left, (void*)node->right, node->left != NULL, node->right != NULL);
}
/* ======= END: Debug ======= */

void CountCharFrequency(int* freq /*array of length ASCII_MAX initialized to {0}*/, char* path, int* chars)
{
    FILE* fp = fopen(path, "rt");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: File not found in %s at line %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    char currChar;
    while ((currChar = fgetc(fp)) != EOF)
    {
        if (freq[(unsigned char)currChar] == 0)
            (*chars)++;
        freq[(unsigned char)currChar]++;
    }

    fclose(fp);
}

void CastToStruct(int* freq, CharFrequency** charFreqArr, int allocate)
{
    *charFreqArr = (CharFrequency*)malloc(allocate * sizeof(CharFrequency));
    if (*charFreqArr == NULL)
    {
        fprintf(stderr, "ERROR: Memory allocation failed for charFreqArr\n");
        exit(EXIT_FAILURE);
    }

    int lastChar = 0;
    for (int i = 0; i < allocate; i++)
    {
        for (int j = lastChar; j < ASCII_MAX; j++)
        {
            if (freq[j] > 0)
            {
                (*charFreqArr)[i].keys[0] = j;
                (*charFreqArr)[i].repetition = freq[j];
                lastChar = j + 1;
                break;
            }
        }
    }
}

void InsertSorted(PriorityQueue* q, BinTreePtr newNode)
{
    Insert(q, newNode, newNode->info.repetition);
}

BinTreePtr CreateIndependentTree(BinTreePtr left, BinTreePtr right)
{
    Tree_Info rootInfo = { .keys = {0}, .repetition = left->info.repetition + right->info.repetition };
    BinTreePtr newNode = BuildTree(left, right, rootInfo);
    return newNode;
}

BinTreePtr CreateHuffman(PriorityQueue* q)
{
    while (!Queue_Empty(*q) && q->head->next != NULL)
    {
        BinTreePtr left = Remove(q);
        BinTreePtr right = Remove(q);
        BinTreePtr newNode = CreateIndependentTree(left, right);
        InsertSorted(q, newNode);
    }
    return Remove(q);
}

void GenerateCodes(BinTreePtr root, char* code, int depth, char codes[256][MAX_TREE_HEIGHT])
{
    if (root == NULL) {
        return;
    }

    if (root->left) {
        code[depth] = '0';
        GenerateCodes(root->left, code, depth + 1, codes);
    }

    if (root->right) {
        code[depth] = '1'; // Append '1' for right
        GenerateCodes(root->right, code, depth + 1, codes);
    }

    // If it's a leaf node (both children are NULL), save the code
    if (!root->left && !root->right) {
        code[depth] = '\0'; // Null-terminate the code
        strcpy(codes[(unsigned char)root->info.keys[0]], code); // Save the code in the lookup table
    }
}

void SerializeTree(FILE* file, BinTreePtr root) {
    if (root == NULL) {
        return;
    }

    if (!root->left && !root->right) {
        fputc('1', file);
        fputc(root->info.keys[0], file);
        return;
    }

    fputc('0', file);

    SerializeTree(file, root->left);
    SerializeTree(file, root->right);
}

BinTreePtr DeserializeTree(FILE* file) {
    int ch = fgetc(file);

    if (ch == EOF) {
        printf("EOF reached\n");
        return NULL;
    }

    if (ch == '1') {
        BinTreePtr leaf = (BinTreePtr)malloc(sizeof(BinTree));
        if (leaf == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed in DeserializeTree.\n");
            return NULL;
        }
        leaf->info.keys[0] = fgetc(file);
        leaf->info.repetition = 0;
        leaf->left = NULL;
        leaf->right = NULL;
        printf("Deserialized leaf node: [%c]\n", leaf->info.keys[0]);
        return leaf;
    }

    if (ch == '0') {
        BinTreePtr internal = (BinTreePtr)malloc(sizeof(BinTree));
        if (internal == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed in DeserializeTree.\n");
            return NULL;
        }
        printf("Deserializing internal node...\n");
        internal->left = DeserializeTree(file);
        internal->right = DeserializeTree(file);
        internal->info.repetition = 0;
        internal->info.keys[0] = 0;
        return internal;
    }

    return NULL;
}

void PrintCompressedData(char* inputPath, BinTreePtr huffmanTree, char codes[256][MAX_TREE_HEIGHT]) {
    printf("Starting compression...\n");

    FILE* inputFile = fopen(inputPath, "rt");
    if (inputFile == NULL) {
        fprintf(stderr, "ERROR: Failed to open input file %s at line %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int totalChars = 0;
    while (fgetc(inputFile) != EOF) {
        totalChars++;
    }
    rewind(inputFile);

    printf("Total characters: %d\n", totalChars);
    printf("Compressed data: ");

    unsigned char buffer = 0;
    int bitCount = 0;
    char ch;

    while ((ch = fgetc(inputFile)) != EOF) {
        const char* code = codes[(unsigned char)ch];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer = (buffer << 1) | (code[i] - '0');
            bitCount++;

            if (bitCount == 8) {
                printf("%02X ", buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    // Print any remaining bits in the buffer
    if (bitCount > 0) {
        buffer = buffer << (8 - bitCount);
        printf("%02X ", buffer);
    }

    printf("\n");
    fclose(inputFile);
}

void PrintDecompressedData(char* inputPath) {
    printf("Starting decompression...\n");

    FILE* inputFile = fopen(inputPath, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "ERROR: Failed to open input file %s at line %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int totalChars;
    fread(&totalChars, sizeof(int), 1, inputFile);

    BinTreePtr huffmanTree = NULL;
    int ch;

    // Read and deserialize the tree until the delimiter '|'
    while ((ch = fgetc(inputFile)) != '|') {
        if (ch == EOF) {
            fprintf(stderr, "ERROR: Invalid file format, delimiter not found.\n");
            fclose(inputFile);
            exit(EXIT_FAILURE);
        }
        ungetc(ch, inputFile);
        huffmanTree = DeserializeTree(inputFile);
    }

    // Decode the compressed data
    BinTreePtr current = huffmanTree;
    int decodedChars = 0;

    printf("Decompressed data: ");

    while ((ch = fgetc(inputFile)) != EOF && decodedChars < totalChars) {
        for (int i = 7; i >= 0; i--) {
            int bit = (ch >> i) & 1;
            current = bit ? current->right : current->left;

            if (!current->left && !current->right) {
                printf("%c", current->info.keys[0]);
                decodedChars++;
                if (decodedChars == totalChars) {
                    break;
                }
                current = huffmanTree;
            }
        }
    }

    printf("\n");
    fclose(inputFile);
}

void DoCompressing(char* textFile) {
    int freq[ASCII_MAX] = { 0 };
    int chars = 0;
    CharFrequency* arr;
    PriorityQueue pq;

    CountCharFrequency(freq, textFile, &chars);
    CastToStruct(freq, &arr, chars);

    Queue_Init(&pq);
    for (int i = 0; i < chars; i++) {
        BinTreePtr newNode;
        InitTree(&newNode);
        newNode->info.keys[0] = arr[i].keys[0];
        newNode->info.repetition = arr[i].repetition;
        InsertSorted(&pq, newNode);
    }

    BinTreePtr huff = CreateHuffman(&pq);

    char codes[256][MAX_TREE_HEIGHT] = { 0 }; // Lookup table for codes
    char code[MAX_TREE_HEIGHT];               // Temporary buffer
    GenerateCodes(huff, code, 0, codes);

    PrintCompressedData(textFile, huff, codes);
}

void DoDecompress(char* huffFile) {
    PrintDecompressedData(huffFile);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Invalid number of parameters!\nUsage: Huffman <path> <option>\n");
        return 1;
    }

    if (strcmp(argv[2], "c") == 0)
    {
        printf("Compressing\n");
        DoCompressing(argv[1]);
    }
    else if (strcmp(argv[2], "d") == 0)
    {
        printf("Decompressing\n");
        DoDecompress(argv[1]);
    }
    else
    {
        printf("Invalid option! Use 'c' for compress or 'd' for decompress.\n");
    }

    return 0;
}