#include <header.hpp>
#include <iostream>
#include <exception>
#include <map>
#include <ctime>
#include <stdlib.h>

using namespace std;

void	Header::Parse_request(string request)
{
	vector<string> parsed = ft::split(request);

	if (parsed.size() != 3)
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	try
	{
		_method = parsed[0];
		if (parsed[1].find("?") != string::npos)
		{
			vector<string> tmp = ft::split(parsed[1], "?");
			_path = tmp[0];
			_query = tmp[1];
		}
		else
			_path = parsed[1];
		if (_path.find('.') != string::npos)
			_extension = _path.substr(_path.find_last_of('.'), string::npos);
		_http_version = parsed[2];
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	}
}

void	Header::load_headers_in(vector<string> in)
{
	for (size_t i = 0; i < in.size() - 1; i++)
		_headers_in[in[i].substr(0, in[i].find(':'))] = in[i].substr(in[i].find(':') + 2, string::npos);
}

Header::Header() {}

Header	&Header::operator=(const Header &h)
{
	this->_method		= h._method;
	this->_path			= h._path;
	this->_query		= h._query;
	this->_extension	= h._extension;
	this->_http_version	= h._http_version;
	this->_chonky		= h._chonky;
	this->_headers_in 	= h._headers_in;
	this->_headers_out	= h._headers_out;
	return *this;
}

Header::Header(const Header &h)
{
	*this = h;
}

Header::Header(vector<string> in)
{
	if (in.size() == 0)
		throw Plebception(ERR_INVALID_VECTOR, "empty", "");
	Parse_request(in[0]);
	in.erase(in.begin());
	load_headers_in(in);
	_chonky = (_headers_in["Transfer-Encoding"] == "chunked") ? true : false; 
}

string	Header::content_type_switch()
{
	if (_extension == ".html")
		return ("text/html; charset=utf-8");
	if (_extension == ".css")
		return ("text/css; charset=utf-8");
	if (_extension == ".jpg")
		return ("image/jpeg");
	if (_extension == ".js")
		return ("text/javascript; charset=utf-8");
	if (_extension == ".png")
		return ("image/png");
	if (_extension == ".json")
		return ("application/json; charset=utf-8");
	if (_extension == ".svg")
		return ("image/svg+xml");
	return ("text/html; charset=utf-8");
}

string create_date()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buf[100];

	time (&rawtime);
	timeinfo = gmtime(&rawtime);

	strftime(buf, 100, "%a, %d %b %G %H:%M:%S GMT", timeinfo);
	return (string(buf));
}

void	Header::add_to_header_out(string val, string key)
{
	// if (!_headers_out[val].empty())
	// 	_headers_out_len -= _headers_out[val].size() - val.size() - 3;
	_headers_out[val] = key;
	// _headers_out_len += val.size() + key.size() + 3;
}

void	Header::add_to_header_out(vector<string> head)
{
	for (size_t i = 0; i < head.size(); i++)
	{
		size_t pos = head[i].find(":");
		if (pos != string::npos)
		{
			string val = head[i].substr(0, pos);
			string key = head[i].substr(pos + 2);
			_headers_out[val] = key;
		}
		else
			throw Plebception(ERR_TOO_FEW_ARG, "add_to_header", head[i]);
	}
}


string Header::create_header(int response_code, int body_length, map<int, string> &status_text)
{
	string res;

	if (!_headers_out["Status"].empty())
		res = "HTTP/1.1 " + _headers_out["Status"] + " " + status_text[atoi(_headers_out["Status"].c_str())] +"\r\n";
	else
		res = "HTTP/1.1 " + ft::to_string(response_code) + " " + status_text[response_code] +"\r\n";

	add_to_header_out("Date", create_date());
	add_to_header_out("Server", "Plebserv/1.3.29 (Unix)");
	add_to_header_out("Connection", "keep-alive");
	add_to_header_out("Content-Type", content_type_switch());
	add_to_header_out("Content-Length", ft::to_string(body_length));

	for (map<string, string>::const_iterator i = _headers_out.begin(); i != _headers_out.end(); i++)
		res += i->first + ": " + i->second + "\r\n";
	res += "\r\n";
	return (res);
}

std::ostream &operator<<(std::ostream &out, Header const &value)
{
	out << "------------ HEADER --------" << std::endl;

	out << std::setw(15) << "REQUEST LINE | " << "[" << value._method << "] [" << value._path << "]" << std::endl;
	out << std::setw(15) << "QUERY | " << value._query << std::endl;
	out << std::setw(15) << "EXTENSION | " << value._extension << std::endl;
	out << std::setw(15) << "CHONKY | " << value._chonky << std::endl;
	out << std::setw(15) << "OTHER HEADERS:\n";
	for (map<string, string>::const_iterator i = value._headers_in.begin(); i != value._headers_in.end(); i++)
		out << i->first << ": " <<  i->second << std::endl;

	out << "------------ DONE ----------" << std::endl;
	return (out);
}
