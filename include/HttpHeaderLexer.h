#pragma once

#include <list>
#include <sstream>


class HttpHeaderLexer
{
public:
	enum eHttpTocken
	{
		Method,
		Separator,
		EndLine,
		Word
	};

public:
	HttpHeaderLexer();
	~HttpHeaderLexer();

	std::list < std::pair < eHttpTocken, std::string > >		&lex(std::string const &);


private:
	void			comsumeSpace();
	std::streampos	readWord(std::string &);
	
	bool			lexMethod();
	bool			lexSeparator();
	bool			lexEndLine();
	bool			lexWord();

	std::stringstream								_stream;
	std::list < std::pair < eHttpTocken, std::string> >	_tockens;
};
