#include <assert.h>
#include "httpFramer.hpp"
using namespace std;
void httpFramer::append(string chars)
{
	for(uint i=0; i < chars.length(); ++i)
	{
		if(chars[i] != '\r') // Just ignore '\r' for convenience
		{
			if(chars[i] == '\n')
			{
				if(m_currStr[m_currStr.length()-1] == '\n') // The end of one request
				{
					m_queue.push(m_currStr); // push one complete request
					m_currStr.clear(); // clear a string to get a new request
				}
				else
				{
					m_currStr.push_back(chars[i]);
				}
			}
			else
			{
				m_currStr.push_back(chars[i]);
			}
		}
	}
}

bool httpFramer::hasMessage() const
{
	return 0 < m_queue.size();
}

string httpFramer::topMessage() const
{
	return m_queue.front();
}

void httpFramer::popMessage()
{
	m_queue.pop();
}
/*
void httpFramer::printToStream(ostream& stream) const
{
	
}
*/
