#include "huffmanTree.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <queue>

/* COMPARATOR FUNCTION FOR PRIORITIY QUEUE 
   PRIORITIZES HIGHER FREQUENCY, LOWER ASCII VALUE, LOWER AGE */

/* https://www.geeksforgeeks.org/comparator-class-in-c-with-examples/ */
struct MyComparator
{
    bool operator()(node *newNode, node *oldNode)
    {
        if (newNode->frequency == oldNode->frequency)
        {
            if (newNode->symbol == oldNode->symbol)
            {
                return newNode->age < oldNode->age;
            }
            return newNode->symbol > oldNode->symbol;
        }
        return newNode->frequency > oldNode->frequency;
    }
};

/* HUFFMAN TREE PRINT FUNCTION */
void HuffmanTree::printHuffmanTreeInfo(node *root, std::string code)
{
    if (!root)
    {
        return;
    }
    if (root->symbol != '\0')
    {
        std::cout << "Symbol: " << root->symbol << ", Frequency: " << root->frequency << ", Code: " << code << std::endl;
        return;
    }
    printHuffmanTreeInfo(root->left, code += "0");
    code = code.substr(0, code.length() - 1);
    printHuffmanTreeInfo(root->right, code += "1");
}

/* HUFFMAN TREE CREATION */
void HuffmanTree::buildHuffmanTree(std::vector<std::pair<char, int> > pairVector)
{
    std::priority_queue<node *, std::vector<node *>, MyComparator> pq;
    int count = 0;

    for (std::pair<char,int> pair : pairVector)
    {
        pq.push(new node(pair.first, pair.second, nullptr, nullptr, count++));
    }

    while (pq.size() > 1)
    {
        node *left = pq.top();
        pq.pop();
        node *right = pq.top();
        pq.pop();
        pq.push(new node('\0', left->frequency + right->frequency, left, right, count++));
    }
    root = pq.top();
}

/* BINARY CODE DECODE FUNCTION*/
char HuffmanTree::binaryCodeToSymbol(std::string binaryCode)
{
    node* curr = root;
    for (char binaryChar : binaryCode)
    {
        switch (binaryChar)
        {
        case '0':
            curr = curr->left;
            break;
        case '1':
            curr = curr->right;
            break;
        default:
            break;
        }
    }

    return curr->symbol;
}