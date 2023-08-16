#include "huffmanTree.h"
#include <iostream>
#include <utility>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>

/* FIREMAN FUNCTION*/
void fireman(int)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    /* DATA STRUCTURES */
    HuffmanTree huffmanTree;
    std::vector<std::pair<char, int>> charFrequencies;

    /* VARIABLES */
    int sockfd, sockfd_RW, portno, clilen, numOfBytes, connections = 0;
    int bufferSize = 256;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[bufferSize];
    std::string inputFileName, input = "";
    signal(SIGCHLD, fireman);

    /*SOCKET COMMUNICATION INITIALIZATION*/
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided" << std::endl;
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket from server" << std::endl;
        exit(1);
    }

    /*SOCKET BINDING*/
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR binding socket from server" << std::endl;
        exit(1);
    }
    clilen = sizeof(cli_addr);

    /* PAIR(SYMBOL, FREQUENCY) CREATION */
    while (getline(std::cin, input))
    {
        charFrequencies.push_back(std::make_pair(input[0], input[2] - '0'));
        connections++;
    }

    /* HUFFMANTREE OPERATIONS */
    huffmanTree.buildHuffmanTree(charFrequencies);
    huffmanTree.printHuffmanTree();

    listen(sockfd, 5);

    while(true)
    {
        /* ACCEPT CONNECTION TO CLIENT*/
        sockfd_RW = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if (sockfd_RW < 0)
        {
            std::cerr << "ERROR on accept from server" << std::endl;
            exit(1);
        }

        /* NEW PROCESS*/
        if (fork() == 0)
        {
            /* READ BINARY CODE FROM CLIENT */
            numOfBytes = read(sockfd_RW, buffer, bufferSize);
            if (numOfBytes < 0)
            {
                std::cerr << "ERROR reading from socket from server" << std::endl;
                exit(1);
            }
            std::string code(buffer);
            bzero(buffer, bufferSize);

            /* WRITE SYMBOL TO CLIENT*/
            (buffer)[0] = huffmanTree.binaryCodeToSymbol(code);

            numOfBytes = write(sockfd_RW, buffer, bufferSize);
            if (numOfBytes < 0)
            {
                std::cerr << "ERROR writing to socket from server" << std::endl;
                exit(1);
            }
            close(sockfd_RW);
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}