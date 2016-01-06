#include "HttpHeaderParser.h"
#include <iostream>

HttpHeaderParser::HttpHeaderParser()
{
}

HttpHeaderParser::~HttpHeaderParser()
{

}


HttpHeader		&HttpHeaderParser::parse(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > &lex, bool &valid)
{
	std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator it = lex.begin();
	std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator end = lex.end();

	if (!this->fillRequestLine(it, end) || !this->fillFields(it, end))
		valid = false;
	else
		valid = true;
	return (this->_header);
}

bool			HttpHeaderParser::fillRequestLine(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &it,
	std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > >::iterator &end)
{
	if (it == end || it->first != HttpHeaderLexer::Method)
		return (false);
	this->_header.setMethod(it->second);
	++it;
	if (it == end || it->first != HttpHeaderLexer::Word)
		return (false);
	this->_header.setRequestUri(it->second);
	++it;
	if (it == end || it->first != HttpHeaderLexer::Word)
		return (false);
	this->_header.setHttpVersion(it->second);
	++it;
	if (it == end || it->first != HttpHeaderLexer::EndLine)
		return (false);
	++it;
	return (true);
}

bool			HttpHeaderParser::fillFields(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > >::iterator &it,
	std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &end)
{
	while (it != end)
	{
		if (!this->fillField(it, end))
		  {
			return (false);
		  }
		while (it != end && it->first == HttpHeaderLexer::EndLine)
		  ++it;
	}
	return (true);
}

bool		HttpHeaderParser::fillField(std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator & it,
	std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > ::iterator &end)
{
	std::string		key;
	std::string		value;

	while (it != end && it->first != HttpHeaderLexer::Separator && it->first != HttpHeaderLexer::EndLine)
	{
		if (it->first != HttpHeaderLexer::Word)
			return (false);
		key = key.append(it->second);
		++it;
	}
	if (it == end || it->first == HttpHeaderLexer::EndLine || key == "")
		return (false);
	++it;
	while (it != end && it->first != HttpHeaderLexer::EndLine)
	{
		if (it->first != HttpHeaderLexer::Word && it->first != HttpHeaderLexer::Separator)
			return (false);
		value = value.append(it->second);
		++it;
	}
	this->_header.addHeaderField(key, value);
	return (true);
}
