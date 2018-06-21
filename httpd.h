#ifndef HTTPD_H
#define HTTPD_H
#include <string>
#include <pthread.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <sys/wait.h>
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXPENDING 5  
#include "httpd.h"
#include "httpFramer.hpp"
#include "httpParser.hpp"
#define BUF_SIZE 255
#define MALFORMED 0
#define OK 1
#define FORBIDDEN 2
#define NOTFOUND 3
using namespace std;
void* ThreadMain(void* threadArgs);
void start_httpd(unsigned short port, string doc_root);
void createResponse(int mode, string& responseStr);
int sendMessage(int clntSocket, string& responseStr);
void transferToStr(string& inStr, char* buffer);
bool escapeTest(char* abs_path, string rootDir);
void createPerfectResponse(string& responseStr);
string lastModification(char* buffer);
string fileTypeExtractor(string path);
void HandleTCPClient(int clntSocket, string docRoot);
struct ThreadArgs
{
	int clntSock;
	string rootDir;
};
#endif // HTTPD_H
