#include "HttpHeaderLexer.h"

HttpHeaderLexer::HttpHeaderLexer()
{

}

HttpHeaderLexer::~HttpHeaderLexer()
{

}


std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > >	&HttpHeaderLexer::lex(std::string const &buff)
{
	this->_tockens.clear();
	this->_stream.clear();
	this->_stream.str(buff);

	while (!this->_stream.eof())
	{
		if (!this->lexMethod() && !this->lexSeparator() && !this->lexEndLine() && !this->lexWord())
			return (this->_tockens);
		this->comsumeSpace();
	}
	return (this->_tockens);
}

bool					HttpHeaderLexer::lexMethod()
{
	std::string			word;
	std::string			allowed[] = {"OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};

	std::streampos pos = this->readWord(word);
	
	for (int i = 0; i < 8; ++i)
	{
		if (word == allowed[i])
		{
			this->_tockens.push_back(std::pair < eHttpTocken, std::string > (Method, word));
			return true;
		}
	}
	this->_stream.seekg(pos);
	return (false);
}

bool					HttpHeaderLexer::lexSeparator()
{
	if (this->_stream.peek() == ':')
	{
		this->_tockens.push_back(std::pair < eHttpTocken, std::string > (Separator, ":"));
		this->_stream.get();
		return (true);
	}
	return (false);
}

bool					HttpHeaderLexer::lexEndLine()
{
	std::streampos		p = this->_stream.tellg();
	char				c1 = this->_stream.get();
	char				c2 = this->_stream.get();

	if (c1 == '\r' && c2 == '\n')
	{
		this->_tockens.push_back(std::pair < eHttpTocken, std::string > (EndLine, "\r\n"));
		return (true);
	}
	this->_stream.seekg(p);
	return (false);
}

bool					HttpHeaderLexer::lexWord()
{
	std::string			w;
	std::streampos		p = this->readWord(w);

	if (w.empty())
	{
		this->_stream.seekg(p);
		return (false);
	}
	this->_tockens.push_back(std::pair < eHttpTocken, std::string > (Word, w));
	return (true);
}

std::streampos			HttpHeaderLexer::readWord(std::string &word)
{
	char				letter;
	std::string			alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	  "/0123456789.-_+*=,.;()?";
	std::streampos		p = this->_stream.tellg();
	std::stringstream	s;

	letter = this->_stream.peek();
	while (alpha.find(letter) != std::string::npos)
	{
		s << letter;
		this->_stream.get();
		letter = this->_stream.peek();
	}
	word = s.str();
	return (p);
}

void					HttpHeaderLexer::comsumeSpace()
{
	while (this->_stream.peek() == ' ')
		this->_stream.get();
}
