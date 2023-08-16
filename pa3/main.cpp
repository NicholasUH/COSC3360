#include "huffmanTree.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <pthread.h>

struct threadArgs
{
    pthread_mutex_t *mainSemaphore;   // local variables
    pthread_mutex_t *threadSemaphore; // guarantee correct printing
    pthread_cond_t *waitTurn;
    HuffmanTree *huffmanTree;
    std::vector<char> *message;
    std::string binaryCode;
    std::vector<int> positions;
    int threadNumber, turn = 0;
};

void *printAndDecompress(void *passedArgs)
{
    /* STORE THREAD_LOCAL VARIABLES */
    threadArgs *args = (threadArgs *)passedArgs;
    std::string binaryCode = args->binaryCode;
    int threadNumber = args->threadNumber;
    std::vector<int> positions = args->positions;

    pthread_mutex_unlock(args->mainSemaphore); // END CRITICAL SECTION #1

    pthread_mutex_lock(args->threadSemaphore); // BEGIN CRITICAL SECTION #2

    /* ENSURE CORRECT PRINTING */
    while (threadNumber != args->turn)
    {
        pthread_cond_wait(args->waitTurn, args->threadSemaphore);
    }

    pthread_mutex_unlock(args->threadSemaphore); // END CRITICAL SECTION #2

    /* ASSIGN FINAL MESSAGE'S SYMBOLS & PRINT NODE */
    for (int position : positions)
    {
        (*args->message)[position] = args->huffmanTree->binaryCodeToSymbol(binaryCode);
    }

    args->huffmanTree->printNode(binaryCode);

    pthread_mutex_lock(args->threadSemaphore); // BEGIN CRITICAL SECTION #3
    args->turn++;
    pthread_cond_broadcast(args->waitTurn);
    pthread_mutex_unlock(args->threadSemaphore); // END CRITICAL SECTION #3

    return NULL;
}

void processInput(std::vector<std::pair<char, int>> &charFreq,
                  std::vector<std::pair<std::string, std::vector<int>>> &codePositions,
                  int &numOfSymbols, int &msgLength)
{
    int position;
    std::string input = "", binaryCode = "";
    std::vector<int> positions;

    /* RETRIEVE NUMBER OF SYMBOLS */
    std::cin >> numOfSymbols;
    std::cin.ignore();

    /* RETRIEVE SYMBOLS AND FREQUENCIES */
    for (int x = 0; x < numOfSymbols; x++)
    {
        getline(std::cin, input);
        charFreq.push_back(std::make_pair(input[0], input[2] - '0'));
    }

    /* RETRIEVE CODES AND POSITIONS */
    for (int x = 0; x < numOfSymbols; x++)
    {
        positions.clear();
        getline(std::cin, input);
        std::istringstream ss(input);
        ss >> binaryCode;

        while (ss >> position)
        {
            msgLength++;
            positions.push_back(position);
        }

        codePositions.push_back(std::make_pair(binaryCode, positions));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        std::cerr << "ERROR, no input provided" << std::endl;
        exit(1);
    }

    /*VARIABLE INITIALIZATION*/
    int numOfSymbols, msgLength = 0;
    HuffmanTree huffmanTree;
    std::vector<std::pair<char, int>> charFrequencies;
    std::vector<std::pair<std::string, std::vector<int>>> codePositions;
    std::vector<char> finalMessage;
    std::vector<pthread_t> pThreads;
    threadArgs args;
    static pthread_mutex_t mainSemaphore;
    static pthread_mutex_t threadSemaphore;
    static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&mainSemaphore, NULL);
    pthread_mutex_init(&threadSemaphore, NULL);

    processInput(charFrequencies, codePositions, numOfSymbols, msgLength);

    finalMessage.resize(msgLength);
    huffmanTree.buildHuffmanTree(charFrequencies);

    /* ASSIGN STRUT NON-UNIQUE VARIABLES */
    args.mainSemaphore = &mainSemaphore;
    args.threadSemaphore = &threadSemaphore;
    args.waitTurn = &waitTurn;
    args.huffmanTree = &huffmanTree;
    args.message = &finalMessage;

    /* THREAD CREATION */
    for (int x = 0; x < numOfSymbols; x++)
    {
        pthread_t thread;

        pthread_mutex_lock(&mainSemaphore); // BEGIN CRITICAL SECTION #1

        /* ASSIGN STRUCT UNIQUE VARIABLES */
        args.binaryCode = codePositions[x].first;
        args.positions = codePositions[x].second;
        args.threadNumber = x;

        /* BEGIN THREADS */
        pthread_create(&thread, NULL, printAndDecompress, &args);

        pThreads.push_back(thread);
    }

    /* END THREADS */
    for (int x = 0; x < numOfSymbols; x++)
    {
        pthread_join(pThreads[x], NULL);
    }

    /* PRINT FINAL MESSAGE */
    for (int x = 0; x < msgLength; x++)
    {
        if (x == 0)
        {
            std::cout << "Original message: ";
        }
        std::cout << finalMessage[x];
    }
}