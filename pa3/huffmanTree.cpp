#include "huffmanTree.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <queue>

/* COMPARATOR TO ENSURE CORRECT HUFFMFAN TREE */
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

/* HUFFMAN TREE BUILDER FUNCTION */
void HuffmanTree::buildHuffmanTree(std::vector<std::pair<char, int>> pairVector)
{
    std::priority_queue<node *, std::vector<node *>, MyComparator> pq;
    int count = 0;

    for (std::pair<char, int> pair : pairVector)
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

/* PRINT HUFFMAN NODE */
void HuffmanTree::printNode(std::string binaryCode)
{
    if (!root)
    {
        return;
    }

    node *curr = root;
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
    std::cout << "Symbol: " << curr->symbol << ", Frequency: " << curr->frequency << ", Code: " << binaryCode << std::endl;
}

/* CONVERTER FUNCTION FROM CODE TO SYMBOL */
char HuffmanTree::binaryCodeToSymbol(std::string binaryCode)
{
    if (!root)
    {
        return '\0';
    }

    node *curr = root;
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