#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include "QElapsedTimer"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


using namespace std;

MinHeapNode* newNode(char data, unsigned freq) {
    MinHeapNode* temp = new MinHeapNode;
    temp->left = temp->right = nullptr;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
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

MinHeapNode* extractMin(struct MinHeap* minHeap) {
    if (minHeap->array.empty()) {
        cerr << "Min heap is empty, cannot extract minimum node." << endl;
        return nullptr;
    }
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->array.size() - 1];
    minHeap->array.pop_back();
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    minHeap->array.push_back(minHeapNode);
    int i = minHeap->array.size() - 1;

    while (i > 0 && minHeap->array[(i - 1) / 2]->freq > minHeap->array[i]->freq) {
        swap(minHeap->array[i], minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->array.size() - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

MinHeap* createAndBuildMinHeap(const unordered_map<char, unsigned>& freqmap) {
    struct MinHeap* minHeap = new MinHeap;
    for (auto& pair : freqmap) {
        MinHeapNode* temp = newNode(pair.first, pair.second);
        minHeap->array.push_back(temp);
    }
    buildMinHeap(minHeap);
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
void MainWindow::compressFile(const std::string& inputFile, const std::string& outputFile) {
    // Your compression code here
    // Read the input file and calculate frequencies of characters
    ifstream inFile (inputFile , ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        ui->info->setText(ui->info->text()+"Error opening input file: "+QString::fromStdString(inputFile)+"\n");
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
        ui->info->setText(ui->info->text()+"Error reopening compressed file: "+QString::fromStdString(outputFile)+"\n");

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
        ui->info->setText(ui->info->text()+"Error reopening compressed file: "+QString::fromStdString(inputFile)+"\n");

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
    ui->info->setText("File compressed successfully! \n");
}

// Decompress the input file using Huffman coding
void MainWindow::decompressFile (const string& inputFile , const string& outputFile) {
    // Read Huffman codes from the compressed file
    ifstream inFile (inputFile , ios::binary);
    if (!inFile) {
        cerr << "Error opening input file: " << inputFile << endl;
        ui->info->setText(ui->info->text()+"Error opening input file: "+QString::fromStdString(inputFile)+"\n");

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
         ui->info->setText(ui->info->text()+"Error opening output file: "+QString::fromStdString(outputFile)+"\n");
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
    ui->info->setText(ui->info->text()+"File decompressed successfully! \n");

}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_compressButton_clicked()
{
    QElapsedTimer Timer;
    ui->info->setText("");
    QString infileName = ui->lineEdit->text();
    QString comp = ui->lineEdit_2->text();
    QString decomp = ui->lineEdit_3->text();
    string infile=infileName.toStdString();
    string compressed=comp.toStdString();
    string decompressed=decomp.toStdString();
    Timer.start();
    compressFile(infile,compressed);
    auto compress_duration = Timer.elapsed();
    Timer.restart();
    decompressFile(compressed,decompressed);
    auto decompress_duration=Timer.elapsed();
    ui->info->setText(ui->info->text()+"Compression time: "+QString::number(compress_duration)+"milliseconds\n"+"Decompression time: "+QString::number(decompress_duration)+"milliseconds\n");
}



