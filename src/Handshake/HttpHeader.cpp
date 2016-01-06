#include "HttpHeader.h"

HttpHeader::HttpHeader()
{
}

HttpHeader::~HttpHeader()
{
}

std::string const	&HttpHeader::getMethod() const
{
	return (this->_method);
}

std::string const	&HttpHeader::getRequestUri() const
{
	return (this->_requestUri);
}

std::string const	&HttpHeader::getHttpVersion() const
{
	return (this->_httpVersion);
}

bool				HttpHeader::headerFieldExist(std::string const &key) const
{
	return (!(this->_headerFields.find(key) == this->_headerFields.end()));
}

std::string const	&HttpHeader::getHeaderField(std::string const &key)
{
	return (this->_headerFields[key]);
}


void				HttpHeader::setMethod(std::string const &method)
{
	this->_method = method;
}

void				HttpHeader::setRequestUri(std::string const &uri)
{
	this->_requestUri = uri;
}

void				HttpHeader::setHttpVersion(std::string const &version)
{
	this->_httpVersion = version;
}

void				HttpHeader::clearHeaderField()
{
	this->_headerFields.clear();
}

void				HttpHeader::addHeaderField(std::string const &key, std::string const &value)
{
	this->_headerFields[key] = value;
}
