#include <assert.h>
#include "httpParser.hpp"
#include <iostream>
#define VECTOR_SIZE 3
#define FUNC_INDEX 0
#define URL_INDEX 1
#define VERSION_INDEX 2
#define INITIAL_LINE 0
#define KEY_VAL 1
#define MINIMUM_SIZE 1
bool httpParser::parse(std::string insstr)
{
	std::vector<std::string> myVec;
	breakRequest(myVec, insstr); // myVec is filled with separate headers
	//cout << "insstr: " << insstr << endl;
	if(myVec.size() < MINIMUM_SIZE)
	{
		return false;
	}
	if(initialLineInspect(myVec[INITIAL_LINE]) == false)
	{
		return false;
	}
	for(uint i=KEY_VAL; i < myVec.size(); ++i) // Inspect key-value lines
	{
		if(keyValueInspect(myVec[i]) == false)
		{
			return false;
		}
		hostHeaderSetter(myVec[i]);
		connectionCloseSetter(myVec[i]);
	}
	if(m_hostFlag == false) //Host: is REQUIRED.
	{
		return false;
	}
	return true;
}

bool httpParser::initialLineInspect(std::string line) // '\n' is omitted
{
	std::string currStr;
	std::vector<std::string> tempVector;
	for(uint i=0; i < line.length(); ++i)
	{
		if(line[i] == ' ')
		{
			tempVector.push_back(currStr);
			currStr.clear();
		}
		else
		{
			currStr.push_back(line[i]);
		}
	}
	tempVector.push_back(currStr);
	if(tempVector.size() != VECTOR_SIZE)
	{
		return false;
	}
	if(tempVector[FUNC_INDEX].compare("GET") != 0 || tempVector[VERSION_INDEX].compare("HTTP/1.1") != 0 || tempVector[URL_INDEX][0] != '/')
	{
		return false;
	}
	if(tempVector[URL_INDEX].compare("/") == 0)
	{
		tempVector[URL_INDEX].assign("/index.html");
	}
	m_function.assign(tempVector[FUNC_INDEX]);
	m_url.assign(tempVector[URL_INDEX]);
	m_version.assign(tempVector[VERSION_INDEX]);
	return true;
}

bool httpParser::keyValueInspect(std::string line) // '\n' does not need to be cared......
{
	std::size_t found = line.find(":");
	if(found != std::string::npos)
	{
		if(0 < found && found+2 < line.length() && line[found+2] != ' ' && line[found+1] == ' ' && line[found-1] != ' ')
		{
			return true;
		}
	}
	return false;
}

void httpParser::breakRequest(vector<std::string>& inVector, std::string line)
{
	std::string currStr;
	for(uint i=0; i < line.length(); ++i)
	{
		if(line[i] == '\n')
		{
			inVector.push_back(currStr);
			currStr.clear();
		}
		else
		{
			currStr.push_back(line[i]);
		}
	}
	/*
	for(uint i=0; i < inVector.size(); ++i)
	{
		cout << "Vector: " << inVector[i] << endl;
	}
	*/
}

void httpParser::hostHeaderSetter(std::string line)
{
	std::size_t found = line.find("Host: ");
	if(found != std::string::npos)
	{
		m_hostFlag = true;
	}
}

void httpParser::connectionCloseSetter(std::string line)
{
	if(line.compare("Connection: close") == 0)
	{
		//cout << "Connection Closed Set" << endl;
		m_connectionFlag = true;
	}
}
