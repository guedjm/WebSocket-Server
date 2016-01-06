#pragma once

#include <list>
#include "HttpHeader.h"
#include "HttpHeaderLexer.h"

class HttpHeaderParser
{
public:
	HttpHeaderParser();
	virtual ~HttpHeaderParser();

	HttpHeader	&parse(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > &, bool &);

private:
	bool		fillRequestLine(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &,
		std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &);
	bool		fillFields(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &,
		std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &);
	
	bool		fillField(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &,
		std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &);


	HttpHeader	_header;
};
