#pragma once

#include <map>
#include <string>


class HttpHeader
{
public:
	HttpHeader();
	~HttpHeader();

	std::string const	&getMethod() const;
	std::string const	&getRequestUri() const;
	std::string const	&getHttpVersion() const;
	
	bool				headerFieldExist(std::string const &) const;
	std::string const	&getHeaderField(std::string const &);

	void				setMethod(std::string const &);
	void				setRequestUri(std::string const &);
	void				setHttpVersion(std::string const &);
	void				clearHeaderField();
	void				addHeaderField(std::string const &, std::string const &);

private:
	std::string			_method;
	std::string			_requestUri;
	std::string			_httpVersion;

	std::map<std::string, std::string>	_headerFields;
};