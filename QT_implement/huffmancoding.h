#ifndef HUFFMAN_CODING_H
#define HUFFMAN_CODING_H
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bitset>
#include <queue>
using namespace std;

// Huffman tree node 
struct MinHeapNode {
    char data; // One of the input characters 
    unsigned freq; // Frequency of the character 
    MinHeapNode* left , * right; // Left and right child of this node 
    MinHeapNode(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Huffman Coding class
class HuffmanCoding {
public:
    void compressFile(const string& inputFile, const string& outputFile);
    void decompressFile(const string& inputFile, const string& outputFile);

private:
    struct MinHeap {
        vector<MinHeapNode*> array; // Array of minheap node pointers
    };

    MinHeapNode* buildHuffmanTree(const unordered_map<char, unsigned>& freqmap);
    void generateHuffmanCodes(MinHeapNode* root, string code, unordered_map<char, string>& huffmanCodes);
    MinHeap* createAndBuildMinHeap(const unordered_map<char, unsigned>& freqmap);
    void minHeapify(MinHeap* minHeap, int idx);
    MinHeapNode* extractMin(MinHeap* minHeap);
    void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
    void buildMinHeap(MinHeap* minHeap);
    void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
};
#include "HuffmanCoding.cpp" // Include the implementation file for HuffmanCoding class
#endif // HUFFMAN_CODING_H
