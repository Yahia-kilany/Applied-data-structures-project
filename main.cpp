#include "HuffmanCoding.h" // Include the header file for HuffmanCoding class
#include <chrono>
using namespace std::chrono;
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
    auto compress_start = high_resolution_clock::now();
    huffman.compressFile(inputFile, compressedFile);
    auto compress_stop = high_resolution_clock::now();
    auto decompress_start = high_resolution_clock::now();
    huffman.decompressFile(compressedFile, decompressedFile);
    auto decompress_stop = high_resolution_clock::now();
    auto compress_duration = duration_cast<microseconds>(compress_stop - compress_start);
    auto decompress_duration = duration_cast<microseconds>(decompress_stop - decompress_start);
    cout << "Compression time: " << compress_duration.count() << " microseconds" << endl;
    cout << "Decompression time: " << decompress_duration.count() << " microseconds" << endl;

    return 0;
}