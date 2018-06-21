#ifndef HTTPFRAMER_HPP
#define HTTPFRAMER_HPP
#include <iostream>
#include <queue>
class httpFramer 
{
public:
	void append(std::string chars);

	// Does this buffer contain at least one complete message?
	bool hasMessage() const;

	// Returns the first instruction
	std::string topMessage() const;

	// Removes the first instruction
	void popMessage();

	// prints the string to an ostream (useful for debugging)
	void printToStream(std::ostream& stream) const;

protected:
	std::queue<std::string> m_queue;
	std::string m_currStr;
};

#endif // HTTPRAMER_HPP
