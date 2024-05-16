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
    struct MinHeapNode* left , * right; // Left and right child of this node 
};

// Min Heap: Collection of min-heap (or Huffman tree) nodes 
struct MinHeap {
    vector<MinHeapNode*> array; // Array of minheap node pointers 
};

// Function declarations
MinHeapNode* newNode (char data , unsigned freq);
void swapMinHeapNode (MinHeapNode** a , MinHeapNode** b);
void minHeapify (struct MinHeap* minHeap , int idx);
MinHeapNode* extractMin (struct MinHeap* minHeap);
void insertMinHeap (struct MinHeap* minHeap , struct MinHeapNode* minHeapNode);
void buildMinHeap (struct MinHeap* minHeap);
MinHeap* createAndBuildMinHeap (const unordered_map<char , unsigned>& freqmap);
MinHeapNode* buildHuffmanTree (const unordered_map<char , unsigned>& freqmap);
void generateHuffmanCodes (MinHeapNode* root , string code , unordered_map<char , string>& huffmanCodes);
void compressFile (const string& inputFile , const string& outputFile);
void decompressFile (const string& inputFile , const string& outputFile);

int main () {
    string inputFile , compressedFile , decompressedFile;

    // Get file names from user
    cout << "Enter input file name: ";
    cin >> inputFile;
    cout << "Enter compressed file name: ";
    cin >> compressedFile;
    cout << "Enter decompressed file name: ";
    cin >> decompressedFile;

    // Compress the input file and then decompress it
    compressFile (inputFile , compressedFile);
    decompressFile (compressedFile , decompressedFile);

    return 0;
}

// Function definitions

// Create a new Huffman tree node
MinHeapNode* newNode (char data , unsigned freq) {
    MinHeapNode* temp = new MinHeapNode;
    temp->left = temp->right = nullptr;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Swap two nodes of the Min Heap
void swapMinHeapNode (MinHeapNode** a , MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Heapify function to maintain the min heap property
void minHeapify (struct MinHeap* minHeap , int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    // Compare the node with its left and right children
    if (left < minHeap->array.size () && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->array.size () && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    // Swap and continue heapifying if root is not the smallest
    if (smallest != idx) {
        swapMinHeapNode (&minHeap->array[smallest] , &minHeap->array[idx]);
        minHeapify (minHeap , smallest);
    }
}

// Extract the minimum value node from the heap
MinHeapNode* extractMin (struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->array.size () - 1];
    minHeap->array.pop_back ();
    minHeapify (minHeap , 0);
    return temp;
}

// Insert a new node into the min heap
void insertMinHeap (struct MinHeap* minHeap , struct MinHeapNode* minHeapNode) {
    minHeap->array.push_back (minHeapNode);
    int i = minHeap->array.size () - 1;

    // Fix the min heap property if it is violated
    while (i > 0 && minHeap->array[(i - 1) / 2]->freq > minHeap->array[i]->freq) {
        swap (minHeap->array[i] , minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Build a min heap from an unordered map of character frequencies
void buildMinHeap (struct MinHeap* minHeap) {
    int n = minHeap->array.size () - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify (minHeap , i);
}

// Create and build a min heap from the frequency map
MinHeap* createAndBuildMinHeap (const unordered_map<char , unsigned>& freqmap) {
    struct MinHeap* minHeap = new MinHeap;
    for (auto& pair : freqmap) {
        MinHeapNode* temp = newNode (pair.first , pair.second);
        minHeap->array.push_back (temp);
    }
    buildMinHeap (minHeap);
    return minHeap;
}

// Build the Huffman tree from the frequency map
MinHeapNode* buildHuffmanTree (const unordered_map<char , unsigned>& freqmap) {
    struct MinHeapNode* left , * right , * top;
    MinHeap* minHeap = createAndBuildMinHeap (freqmap);

    // Iterate while size of heap doesn't become 1
    while (minHeap->array.size () != 1) {
        left = extractMin (minHeap);
        right = extractMin (minHeap);

        // Create a new internal node with frequency equal to the sum of the two nodes' frequencies
        top = newNode ('$' , left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap (minHeap , top);
    }
    return extractMin (minHeap);
}

// Generate Huffman codes for the characters and store them in a map
void generateHuffmanCodes (MinHeapNode* root , string code , unordered_map<char , string>& huffmanCodes) {
    if (root == nullptr)
        return;

    // If this is a leaf node, store the character and its code
    if (root->left == nullptr && root->right == nullptr) {
        huffmanCodes[root->data] = code;
        return;
    }

    // Traverse left and right subtrees
    generateHuffmanCodes (root->left , code + "0" , huffmanCodes);
    generateHuffmanCodes (root->right , code + "1" , huffmanCodes);
}

// Compress the input file using Huffman coding
void compressFile (const string& inputFile , const string& outputFile) {
    // Read the input file and calculate frequencies of characters
    ifstream inFile (inputFile , ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        return;
    }
    unordered_map<char , unsigned> freqMap;
    char ch;
    while (inFile.get (ch)) {
        if (ch == 13) {
            ch = 10;
        }
        freqMap[ch]++;
    }
    inFile.close ();

    // Build Huffman tree and generate codes
    MinHeapNode* root = buildHuffmanTree (freqMap);
    unordered_map<char , string> huffmanCodes;
    generateHuffmanCodes (root , "" , huffmanCodes);

    // Write compressed data to output file
    ofstream outFile (outputFile , ios::binary);
    if (!outFile) {
        cerr << "Error opening output file: " << outputFile << endl;
        return;
    }
    for (auto& pair : huffmanCodes) {
        outFile << pair.first << pair.second << '\n';
    }
    outFile << huffmanCodes.begin ()->first << "\n";

    // Encode the input file using the Huffman codes
    inFile.open (inputFile);
    if (!inFile) {
        cerr << "Error reopening compressed file: " << inputFile << endl;
        return;
    }
    string encodedText = "";
    while (inFile.get (ch)) {
        encodedText += huffmanCodes[ch];
    }

    // Pad encoded text to make its length a multiple of 8
    int padding = (8 - encodedText.length () % 8) % 8;
    encodedText += string (padding , '0');

    // Write encoded text as bytes to the output file
    for (size_t i = 0; i < encodedText.length (); i += 8) {
        bitset<8> bits (encodedText.substr (i , 8));
        char c = static_cast<char>(bits.to_ulong ());
        outFile.put (c);
    }
    inFile.close ();
    outFile.close ();

    cout << "File compressed successfully!" << endl;
}

// Decompress the input file using Huffman coding
void decompressFile (const string& inputFile , const string& outputFile) {
    // Read Huffman codes from the compressed file
    ifstream inFile (inputFile , ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        return;
    }
    unordered_map<char , string> huffmanCodes;
    char ch;
    string code;
    string line;
    while (getline (inFile , line)) {
        ch = line[0];
        if (ch == 0 || ch == 10 || ch == 13) {
            ch = '\n';
            getline (inFile , line);
            code = line;
        }
        else {
            code = line.substr (1);
        }
        if (huffmanCodes.find (ch) != huffmanCodes.end ()) {
            break;
        }
        huffmanCodes[ch] = code;
    }

    // Reconstruct Huffman tree from the codes
    MinHeapNode* root = newNode ('$' , 0);
    for (auto& pair : huffmanCodes) {
        MinHeapNode* current = root;
        for (char c : pair.second) {
            if (c == '0') {
                if (!current->left) {
                    current->left = newNode ('$' , 0);
                }
                current = current->left;
            }
            else if (c == '1') {
                if (!current->right) {
                    current->right = newNode ('$' , 0);
                }
                current = current->right;
            }
        }
        current->data = pair.first;
    }

    // Decode the encoded text from the compressed file
    ofstream outFile (outputFile);
    if (!outFile) {
        cerr << "Error opening output file: " << outputFile << endl;
        return;
    }

    MinHeapNode* current = root;
    while (inFile.get (ch)) {
        bitset<8> bits (ch);
        for (int i = 7; i >= 0; --i) {
            if (bits[i] == 0) {
                current = current->left;
            }
            else {
                current = current->right;
            }
            if (!current->left && !current->right) {
                outFile << current->data;
                current = root;
            }
        }
    }
    inFile.close ();
    outFile.close ();

    cout << "File decompressed successfully!" << endl;
}