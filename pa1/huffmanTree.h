#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <thread>
#include <queue>

struct node
{
    char data;
    int frequency, age;
    node *left, *right;

    node(char _data, int _frequency, node *_left, node *_right, int _age)
        : data(_data), frequency(_frequency), left(_left), right(_right), age(_age) {}
};

class HuffmanTree
{
private:
    node *root;
    void printHuffmanTreeInfo(node *, std::string);

public:
    node *getRoot();
    void printHuffmanTree() { printHuffmanTreeInfo(root, ""); }
    void buildHuffmanTree(std::vector<std::pair<char, int> >);
};