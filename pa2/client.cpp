#include <iostream>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <vector>
#include <arpa/inet.h>

/* STRUCT TO HOLD threadArgs FOR THREAD */
struct threadArgs
{
    int portNo;
    char *hostName;
    std::string symbol;
    std::vector<char> message;
    std::vector<int> positions;

    threadArgs(std::string _symbol, std::vector<int> _positions, std::vector<char> _message, int _portNo, char *_hostName)
        : portNo(_portNo), hostName(_hostName), symbol(_symbol), positions(_positions), message(_message) {}
};

/* THREAD FUNCTION */
void *decompressToMessage(void *args)
{
    /* VARIABLES */
    threadArgs *castedArgs = (threadArgs *)args;
    int sockfd, numOfBytes;
    int bufferSize = 256;
    char buffer[bufferSize];
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* SOCKET COMMUNICATION ERROR CHECK */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket from client" << std::endl;
        exit(1);
    }
    server = gethostbyname(castedArgs->hostName);
    if (!server)
    {
        std::cerr << "ERROR, no host from client" << std::endl;
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(castedArgs->portNo);

    /*ACCEPT CONNECTION TO SERVER*/
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting from client" << std::endl;
        exit(1);
    }

    /* WRITE BINARY CODE TO SERVER*/
    bzero(buffer, bufferSize);
    strcpy(buffer, castedArgs->symbol.c_str());
    numOfBytes = write(sockfd, buffer, bufferSize);
    if (numOfBytes < 0)
    {
        std::cerr << "ERROR writing to socket from client" << std::endl;
        exit(1);
    }
    bzero(buffer, bufferSize);

    /* READ CORRESPONDING SYMBOL FROM SERVER */
    numOfBytes = read(sockfd, buffer, bufferSize);
    if (numOfBytes < 0)
    {
        std::cerr << "ERROR reading from socket from client" << std::endl;
        exit(1);
    }

    for (int position : castedArgs->positions)
    {
        castedArgs->message.at(position) = buffer[0];
    }

    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    /* DATA STRUCTURES */
    std::vector<int> positions;
    std::vector<pthread_t> pThreads;
    static std::vector<threadArgs *> threadArguments;
    std::vector<char> finalMessage;

    /* VARIABLES */
    int messageLength, threadCount = 0;
    std::string input, position, code = "";

    /* COMPRESSED FILE PROCESSING */
    while (getline(std::cin, input))
    {
        positions.clear();
        threadCount++;
        std::istringstream inputString(input);
        inputString >> code;
        while (inputString >> position)
        {
            messageLength++;
            positions.push_back(stoi(position));
        }
        threadArgs *newArgs = new threadArgs(code, positions, finalMessage, atoi(argv[2]), argv[1]);
        threadArguments.push_back(newArgs);
    }

    finalMessage.resize(messageLength);

    /* THREAD CREATION*/
    for (int x = 0; x < threadCount; x++)
    {
        threadArguments.at(x)->message.resize(messageLength);
        pthread_t thread;
        pthread_create(&thread, NULL, decompressToMessage, threadArguments.at(x));
        pThreads.push_back(thread);
    }

    /* END THREADS */
    for (int x = 0; x < threadCount; x++)
    {
        pthread_join(pThreads.at(x), NULL);
    }

    /* MERGE MESSAGE VECTORS */
    for (threadArgs *arg : threadArguments)
    {
        for (int position : arg->positions)
        {
            finalMessage.at(position) = arg->message.at(position);
        }
    }

    /* MESSAGE OUTPUT*/
    std::cout << "Original message: ";
    for (int x = 0; x < messageLength; x++)
    {
        std::cout << finalMessage[x];
    }
    return 0;
}