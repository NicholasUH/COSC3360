#include "huffmanTree.h"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <thread>
#include <queue>

// HIGHER PRIORITY - HIGHER FREQUENCY,HIGH ASCII VALUE, OLDER NODE
//https://www.geeksforgeeks.org/comparator-class-in-c-with-examples/
struct MyComparator
{
    bool operator()(node *left, node *right)
    {
        if (left->frequency == right->frequency)
        {
            if (left->data == right->data)
            {
                return left->age < right->age;
            }
            return left->data > right->data;
        }
        return left->frequency > right->frequency;
    }
};

node *HuffmanTree::getRoot()
{
    return this->root;
}

/*
    TRAVERSE HUFFMAN TREE AS LONG AS NOT NULLPTR OR A NODE WITH A SYMBOL
*/
void HuffmanTree::printHuffmanTreeInfo(node *curr, std::string code)
{
    if (!curr)
    {
        return;
    }
    if (curr->data != '\0')
    {
        std::cout << "Symbol: " << curr->data << ", Frequency: " << curr->frequency << ", Code: " << code << std::endl;
        return;
    }
    printHuffmanTreeInfo(curr->left, code += "0");
    code = code.substr(0, code.length() - 1);
    printHuffmanTreeInfo(curr->right, code += "1");
}

/*
    PUSH ALL PAIRS AS NODES INTO PRIORITY QUEUE
    POP TOP TWO NODES, MAKE NEW NODE WITH ADDED FREQUENCY AND DEFAULT VALUE, AND
    PUSH BACK INTO PRIORITY QUEUE UNTIL ONE NODE LEFT BEING THE ROOT
*/

void HuffmanTree::buildHuffmanTree(std::vector<std::pair<char, int> > inputVector)
{
    std::priority_queue<node *, std::vector<node *>, MyComparator> pq;
    int count = 0;

    for (auto x : inputVector){
        pq.push(new node(x.first, x.second, nullptr, nullptr,count));
        count++;
    }

    while (pq.size() > 1)
    {
        node *left = pq.top();
        pq.pop();
        node *right = pq.top();
        pq.pop();
        pq.push(new node('\0', left->frequency + right->frequency, left, right,count++));
    }
    root = pq.top();
}