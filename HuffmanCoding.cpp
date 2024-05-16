#include "HuffmanCoding.h"
void HuffmanCoding::swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void HuffmanCoding::minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->array.size() && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->array.size() && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

MinHeapNode* HuffmanCoding::extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->array.size() - 1];
    minHeap->array.pop_back();
    minHeapify(minHeap, 0);
    return temp;
}

void HuffmanCoding::insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    minHeap->array.push_back(minHeapNode);
    int i = minHeap->array.size() - 1;

    while (i > 0 && minHeap->array[(i - 1) / 2]->freq > minHeap->array[i]->freq) {
        swap(minHeap->array[i], minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void HuffmanCoding::buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->array.size() - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

HuffmanCoding::MinHeap* HuffmanCoding::createAndBuildMinHeap(const unordered_map<char, unsigned>& freqmap) {
    MinHeap* minHeap = new MinHeap;
    for (const auto& pair : freqmap) {
        MinHeapNode* temp = new MinHeapNode(pair.first, pair.second);
        minHeap->array.push_back(temp);
    }
    buildMinHeap(minHeap);
    return minHeap;
}

MinHeapNode* HuffmanCoding::buildHuffmanTree(const unordered_map<char, unsigned>& freqmap) {
    MinHeapNode* left, * right, * top;
    MinHeap* minHeap = createAndBuildMinHeap(freqmap);

    while (minHeap->array.size() != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = new MinHeapNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void HuffmanCoding::generateHuffmanCodes(MinHeapNode* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (!root)
        return;

    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
        return;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

void HuffmanCoding::compressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        return;
    }
    unordered_map<char, unsigned> freqMap;
    char ch;
    while (inFile.get(ch)) {
        if (ch == 13) {
            ch = 10;
        }
        freqMap[ch]++;
    }
    inFile.close();

    MinHeapNode* root = buildHuffmanTree(freqMap);
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    ofstream outFile(outputFile, ios::binary);
    if (!outFile) {
        cerr << "Error opening output file: " << outputFile << endl;
        return;
    }
    for (const auto& pair : huffmanCodes) {
        outFile << pair.first << pair.second << '\n';
    }
    outFile << huffmanCodes.begin()->first << "\n";

    inFile.open(inputFile);
    if (!inFile) {
        cerr << "Error reopening input file: " << inputFile << endl;
        return;
    }
    string encodedText;
    while (inFile.get(ch)) {
        encodedText += huffmanCodes[ch];
    }

    int padding = (8 - encodedText.length() % 8) % 8;
    encodedText += string(padding, '0');

    for (size_t i = 0; i < encodedText.length(); i += 8) {
        bitset<8> bits(encodedText.substr(i, 8));
        char c = static_cast<char>(bits.to_ulong());
        outFile.put(c);
    }
    inFile.close();
    outFile.close();

    cout << "File compressed successfully!" << endl;
}

void HuffmanCoding::decompressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        return;
    }
    unordered_map<char, string> huffmanCodes;
    char ch;
    string code;
    string line;
    while (getline(inFile, line)) {
        ch = line[0];
        if (ch == 0 || ch == 10 || ch == 13) {
            ch = '\n';
            getline(inFile, line);
            code = line;
        } else {
            code = line.substr(1);
        }
        if (huffmanCodes.find(ch) != huffmanCodes.end()) {
            break;
        }
        huffmanCodes[ch] = code;
    }

    MinHeapNode* root = new MinHeapNode('$', 0);
    for (const auto& pair : huffmanCodes) {
        MinHeapNode* current = root;
        for (char c : pair.second) {
            if (c == '0') {
                if (!current->left) {
                    current->left = new MinHeapNode('$', 0);
                }
                current = current->left;
            } else if (c == '1') {
                if (!current->right) {
                    current->right = new MinHeapNode('$', 0);
                }
                current = current->right;
            }
        }
        current->data = pair.first;
    }

    ofstream outFile(outputFile);
    if (!outFile) {
        cerr << "Error opening output file: " << outputFile << endl;
        return;
    }

    MinHeapNode* current = root;
    while (inFile.get(ch)) {
        bitset<8> bits(ch);
        for (int i = 7; i >= 0; --i) {
            if (bits[i] == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            if (!current->left && !current->right) {
                outFile << current->data;
                current = root;
            }
        }
    }
    inFile.close();
    outFile.close();
    cout<<"File decompressed successfully!"<<endl;
}