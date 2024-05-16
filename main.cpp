#include "HuffmanCoding.h" // Include the header file for HuffmanCoding class

int main() {
    HuffmanCoding huffman;

    string inputFile, compressedFile, decompressedFile;

    // Get file names from user
    cout << "Enter input file name: ";
    cin >> inputFile;
    cout << "Enter compressed file name: ";
    cin >> compressedFile;
    cout << "Enter decompressed file name: ";
    cin >> decompressedFile;

    // Compress the input file and then decompress it
    huffman.compressFile(inputFile, compressedFile);
    huffman.decompressFile(compressedFile, decompressedFile);

    return 0;
}