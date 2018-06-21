#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include <string>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;
class httpParser 
{
public:
	httpParser()
	{
		m_connectionFlag = false;
		m_hostFlag = false;
	}
	httpParser(string rootDir)
	{
		m_connectionFlag = false;
		m_hostFlag = false;
		m_url.assign(rootDir);
	}
	bool parse(std::string insstr);
	bool initialLineInspect(std::string line);
	bool keyValueInspect(std::string line);
	void breakRequest(vector<std::string>& inVector, std::string line);
	void hostHeaderSetter(std::string line);
	void connectionCloseSetter(std::string line);
	string getURL()
	{
		return m_url;
	}
	string getVersion()
	{
		return m_version;
	}
	bool getConnectionFlag()
	{
		return m_connectionFlag;
	}
private:
	bool m_connectionFlag;
	bool m_hostFlag;
	string m_function;
	string m_url;
	string m_version;
};

#endif // HTTPPARSER_HPP
