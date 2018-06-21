#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "httpd.h"
using namespace std;
void start_httpd(unsigned short port, string doc_root)
{
	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;

    int servSock;                    /* Socket descriptor for server */
    //int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    //unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
	//echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        //DieWithError("socket() failed");
    }
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);      /* Local port */
	/* Bind to the local address */
    if(bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        //DieWithError("bind() failed");
    }
	/* Mark the socket so it will listen for incoming connections */
    if(listen(servSock, MAXPENDING) < 0)
    {
        //DieWithError("listen() failed");
    }
    int optval = 1;
    if(setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    {
    	printf("REUSE ERROR");
    }
	for (;;) /* Run forever */
    {
    	int clntSock;
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
		/* Wait for a client to connect */
        if((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
        {
            //DieWithError("accept() failed");
        }
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        if(setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
        {
        	printf("Sockopt failed\n");
        }
		/* clntSock is connected to a client! */
        struct ThreadArgs* threadArgs = (struct ThreadArgs*)malloc(sizeof(struct ThreadArgs));
        if(threadArgs == NULL)
        {
        	printf("ERROR\n");
        }
        threadArgs->clntSock = clntSock;
        (threadArgs->rootDir).assign(doc_root);
        
        pthread_t threadID;
        int returnValue = pthread_create(&threadID, NULL, ThreadMain, (void*)threadArgs);
        if(returnValue != 0)
        {
        	printf("Thread Failed \n");
        }
    }
}

void* ThreadMain(void* threadArgs)
{
	pthread_detach(pthread_self());
	int clntSock = ((struct ThreadArgs*) threadArgs)->clntSock;
	string docRoot = "";
	docRoot.assign(((struct ThreadArgs*) threadArgs)->rootDir);
	
	HandleTCPClient(clntSock, docRoot);
	free(threadArgs);
	return NULL;
}

void createResponse(int mode, string& responseStr)
{
	responseStr.clear();
	responseStr.assign("HTTP/1.1 ");
	switch(mode)
	{
		case MALFORMED:
			responseStr.append("400 Client Error\r\n");
			break;
		case OK:
			responseStr.append("200 OK\r\n");
			break;
		case FORBIDDEN:
			responseStr.append("403 FORBIDDEN\r\n");
			break;
		case NOTFOUND:
			responseStr.append("404 Not Found\r\n");
			break;
	}
	responseStr.append("Server: Apache\r\n\r\n");
}

int sendMessage(int clntSocket, string& responseStr)
{
	char* charPtr = (char*)malloc(responseStr.length());
	for(uint i=0; i < responseStr.length(); ++i)
	{
		charPtr[i] = 0;
	}
	for(uint i=0; i < responseStr.length(); ++i)
	{
		charPtr[i] = responseStr[i];
	}
	int retBytes = send(clntSocket, charPtr, responseStr.length(), 0);
	free(charPtr);
	return retBytes;
}

void transferToStr(string& inStr, char* buffer)
{
	int index = 0;
	while(buffer[index] != '\0')
	{
		inStr.push_back(buffer[index++]);
	}
}

bool escapeTest(string abs_path, string rootDir)
{
	size_t found = abs_path.find(rootDir);
	if(found != std::string::npos)
	{
		return false;
	}
	return true; // escape? true
}

void createPerfectResponse(string& responseStr, string modificationTime, string fileType, string fileSize)
{
	responseStr.assign("HTTP/1.1 200 OK\r\nServer: Apache\r\n");
	responseStr.append("Last-Modified: ");
	responseStr.append(modificationTime);
	responseStr.append("Content-Type: ");
	responseStr.append(fileType);
	responseStr.append("Content-Length: ");
	responseStr.append(fileSize);
	responseStr.append("\r\n\r\n");
}

string lastModification(string& date) //Implement it later
{
	string currStr;
	vector<string> dateVec;
	for(uint i=0; i < date.length(); ++i)
	{
		if(date[i] == ' ' || i == date.length()-1)
		{
			if(0 < currStr.length())
			{
				dateVec.push_back(currStr);
				currStr.clear();
			}
		}
		else
		{
			currStr.push_back(date[i]);
		}
	}
	date.clear();
	date.assign(dateVec[0]);
	date.append(", ");
	date.append(dateVec[2]);
	date.append(" ");
	date.append(dateVec[1]);
	date.append(" ");
	date.append(dateVec[4]);
	date.append(" ");
	date.append(dateVec[3]);
	date.append(" ");
	date.append("GMT\r\n");
	return date;
}

string fileTypeExtractor(string path)
{
	int pos = 0;
	for(uint i=path.length()-1; 0 < i; --i)
	{
		if(path[i] == '.')
		{
			pos = i;
			break;
		}
	}
	string type;
	for(uint i=pos+1; i < path.length(); ++i)
	{
		type.push_back(path[i]);
	}
	if(type.compare("jpg") == 0)
	{
		type.assign("image/jpeg\r\n");
	}
	else if(type.compare("png") == 0)
	{
		type.assign("image/png\r\n");
	}
	else
	{
		type.assign("text/html\r\n");
	}
	return type;
}

void fillUpRelPath(char*& charPtr, string m_url, string root)
{
	root.erase(root.length()-1, 1);
	string newStr = root.append(m_url);
	//cout << "NewSTR: " << newStr << endl;
	charPtr = new char [newStr.length()+1];
	for(uint i=0; i < newStr.length(); ++i)
	{
		charPtr[i] = newStr[i];
	}
	charPtr[newStr.length()] = '\0';
	//printf("Rel Path: %s", charPtr);
}

void HandleTCPClient(int clntSocket, string docRoot)
{
	httpFramer myFramer;
	char buffer[BUF_SIZE];
	ssize_t numBytesRcvd = recv(clntSocket, buffer, BUF_SIZE, 0);
	string inStr;
	string resMessage;
	int numBytesSent = 0;
	bool closed = false;
	if(numBytesRcvd < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			createResponse(MALFORMED, resMessage);
			numBytesSent = sendMessage(clntSocket, resMessage);
		}
	}
	while(0 < numBytesRcvd)
	{	
		//transferToStr(inStr, buffer);
		inStr = buffer;
		myFramer.append(inStr);
		inStr.clear();
		while(myFramer.hasMessage())
		{
			httpParser myParser;
			if(myParser.parse(myFramer.topMessage()) == false) // request is inproperly formed
			{
				closed = true;
				myFramer.popMessage();
				createResponse(MALFORMED, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
				break;
			}
			closed = myParser.getConnectionFlag();
			myFramer.popMessage();
			char* relPath = NULL;
			fillUpRelPath(relPath, myParser.getURL(), docRoot);
			//printf("%s \n", relPath);
			char* absPath = NULL;
			absPath = realpath(relPath, absPath);
			//printf("Abs: %s\n", absPath);
			string absolutePath;
			if(absPath != NULL)
			{
				absolutePath = absPath;
			}
			free(relPath);
			
			if(absPath == NULL)
			{
				closed = true;
				createResponse(NOTFOUND, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
				free(absPath);
				break;
			}
			
			if(escapeTest(absolutePath, docRoot)) // define Root directory later
			{	
				closed = true;
				createResponse(NOTFOUND, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
				free(absPath);
				break;
			}
			
			struct stat sb;
			stat(absPath, &sb); // It's time to check permission and file type
			unsigned short accessMask = 4;
			if((sb.st_mode&accessMask) != accessMask) // If they are not the same, the read-bit for world is off
			{
				closed = true;
				createResponse(FORBIDDEN, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
				free(absPath);
				break;
			}
			if((sb.st_mode & S_IFMT) != S_IFREG) // It means that it is not a regular type. 
			{
				closed = true;
				createResponse(FORBIDDEN, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
				free(absPath);
				break;
    		}
			int fileDescriptor = open(absPath, O_RDONLY); // Change it later if neccessary
			if(fileDescriptor < 0)
			{
				printf("FILE OPEN ERROR\n");
			}
			//numBytesSent = sendfile(clntSocket, fileDescriptor, 0, sb.st_size);
			string myTime = ctime(&sb.st_mtime);
			createPerfectResponse(resMessage, lastModification(myTime), fileTypeExtractor(absolutePath), std::to_string(sb.st_size));
			numBytesSent = sendMessage(clntSocket, resMessage);
			int n = 0;
			do
			{
				char BUFFER[BUF_SIZE];
				n = read(fileDescriptor, BUFFER, BUF_SIZE);
				numBytesSent = send(clntSocket, BUFFER, n, 0);
				if(numBytesSent < 0)
				{
					//ERROR
				}
			}while(0 < n);
			free(absPath);
			if(closed == true)
			{
				break;
			}
		}
		if(closed == true)
		{
			break;
		}
		numBytesRcvd = recv(clntSocket, buffer, BUF_SIZE, 0);
		if(numBytesRcvd < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				createResponse(MALFORMED, resMessage);
				numBytesSent = sendMessage(clntSocket, resMessage);
			}
		}
	}
    close(clntSocket);    
}

