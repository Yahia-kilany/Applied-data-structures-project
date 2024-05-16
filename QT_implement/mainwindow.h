#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include <bitset>
#include <queue>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode* left, * right;
};

struct MinHeap {
    std::vector<MinHeapNode*> array;
};

MinHeapNode* newNode(char data, unsigned freq);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(struct MinHeap* minHeap, int idx);
MinHeapNode* extractMin(struct MinHeap* minHeap);
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode);
void buildMinHeap(struct MinHeap* minHeap);
MinHeap* createAndBuildMinHeap(const std::unordered_map<char, unsigned>& freqmap);
MinHeapNode* buildHuffmanTree(const std::unordered_map<char, unsigned>& freqmap);
void generateHuffmanCodes(MinHeapNode* root, std::string code, std::unordered_map<char, std::string>& huffmanCodes);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void compressFile(const std::string& inputFile, const std::string& outputFile);
    void decompressFile(const std::string& inputFile, const std::string& outputFile);

private slots:


    void on_compressButton_clicked();


private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
