#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <plebception.hpp>
#include <server.hpp>
#include <utilities.hpp>

typedef void (Server::*LoadFunction)(vector<string> val);

static map<string, LoadFunction> create_map()
{
	map<string, LoadFunction> m;
	m["location"] 				= &Server::load_locations;
	m["server_name"] 			= &Server::load_server_identifier;
	m["client_max_body_size"]	= &Server::load_client_max_body_size;
	m["listen"]					= &Server::load_ports;

	return m;
}

map<string, LoadFunction> g_server_load_map = create_map();

Server::Server(Server const &tbc)
{
	*this = tbc;
}

Server::~Server() {}

Server	&Server::operator=(Server const &tba)
{
	_port = tba._port;
	_server = tba._server;
	_server_identifier = tba._server_identifier;
	_error_pages = tba._error_pages;
	_max_body_size = tba._max_body_size;
	_locations = tba._locations;
	return (*this);
}

std::ostream &operator<<(std::ostream &out, Server const &value)
{
	out << "------------ SERVER " << value._server << "--------" << std::endl;
	out << std::setw(15) << "BODY SIZE | " << value._max_body_size << std::endl;
	out << std::setw(15) << "LISTEN | ";
	for (size_t i = 0; i < value._port.size(); i++)
		out << value._port[i] << " ";
	out << std::endl << std::setw(15) << "IDENTIFERS | ";
	for (size_t i = 0; i < value._server_identifier.size(); i++)
		out << "{" << value._server_identifier[i] << "} ";
	out << std::endl << std::setw(15) << "LOCATIONS | " << endl;
	for (size_t i = 0; i < value._locations.size(); i++)
		out << value._locations[i] << " ";
	out << std::endl << "---------------- DONE ----------------" << std::endl;
	return (out);
}

Server::Server(vector<string> input) :
	_server(""), _server_identifier(), _max_body_size(160000)
{
	_server_identifier.push_back("");
	_port.push_back(80);
	_port.push_back(8000);
	for (size_t x = 1; x < input.size();)
		x += parse_args(input, x);
}

void Server::call(const string& s, vector<string> val)
{
	LoadFunction func = g_server_load_map[s];
	if (func == 0)
		return;
	(this->*func)(val);
}

void	Server::check_servername(string &val)
{
	vector<string> tokens = ft::split(val, ".");
	if ((tokens.size() == 3 && tokens[0] != "www") || tokens.size() > 3)
		throw Plebception(ERR_INVALID_VALUE, "listen1", val);
	else if(tokens.size() == 3)
		tokens.erase(tokens.begin());
	string res = tokens[0];
	if (tokens.size() == 2)
		res += '.' + tokens[1];
	if (_server != "" && _server != res)
		throw Plebception(ERR_MULTIPLE_DOM, "listen2", res);
}

void	Server::check_port(string &val)
{
	if (val.find_first_not_of("0123456789") != string::npos)
		throw Plebception(ERR_INVALID_VALUE, "listen3", val);
}

vector<string>	Server::check_listen(string &val)
{
	vector<string> tmp = ft::split(val, ":");

	if (tmp.size() > 2)
		throw Plebception(ERR_INVALID_VALUE, "listen4", val);
	else if (tmp.size() == 2)
	{
		check_servername(tmp[0]);			
		check_port(tmp[1]);
	}
	else if (tmp[0].find_first_not_of("0123456789") != string::npos)
		check_servername(tmp[0]);
	else
		check_port(tmp[0]);
	return tmp;
}

void	Server::load_ports(vector<string> val)
{
	_port.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		vector<string> tmp = check_listen(val[i]);
		for (int j = 0; j < tmp.size(); j++)
		{
			if (tmp[j].find_first_not_of("0123456789") != string::npos)
				_server = tmp[j];
			else
				_port.push_back(ft::stoi(tmp[j]));
		}
	}
}

void	Server::load_server_identifier(vector<string> val)
{
	_server_identifier.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		check_servername(val[i]);
		_server_identifier.push_back(val[i]);
	}
}

void	Server::load_client_max_body_size(vector<string> val)
{
	size_t	pos = val[0].find_first_not_of("0123456789");
	size_t	mul = 1;

	if (pos != string::npos)
	{
		if (val[0][pos] != *(val[0].end() - 1))
			throw Plebception(ERR_INVALID_VALUE, "client_max_body_size", val[0]);
		char c = val[0][pos];
		switch (c)
		{
			case 'k':
				mul = 1000;
				break;
			case 'm':
				mul = 1000000;
				break;
			case 'g':
				mul = 1000000000;
				break;
			default:
				throw Plebception(ERR_INVALID_VALUE, "client_max_body_size", val[0]);
		}
	}
	_max_body_size = ft::stoi(val[0]) * mul;
}

void	Server::load_locations(vector<string> val)
{
	_locations.push_back(Location(val));
}

static void check_line(string &s, char delim)
{
	if (s.find(delim) != s.size() - 1)
		throw Plebception(ERR_SEMICOLON, "test", s);
	else
		s = ft::trim_char(s, delim);
}

static bool verify_line(string s, char delim) {
	if (count(s.begin(), s.end(), delim) == 1 && s.find(delim) == s.size() - 1)
		return(true);
	return(false);
}

int Server::parse_args(vector<string> arr, int index)
{
	std::vector<string> tokens;
	std::vector<string> args;
	bool location;

	size_t i;
	for (i = index; i < arr.size(); i++)
	{
		location = false;
		string s = arr[i];
		if (!s.size())
			continue ;
		tokens = ft::split(s);
		if (tokens[0].length() == 1)
			return (1);
		if (!verify_line(s, '{') && !verify_line(s, '}') && !verify_line(s, ';'))
			throw Plebception(ERR_INVALID_TOKEN, tokens[0], s);
		if (tokens[tokens.size() - 1] == "{")
		{
			location = true;
			args.push_back(tokens[1]);
			for(i++; i < arr.size() && arr[i].find('}') == string::npos; i++)
				args.push_back(arr[i]);
		}
		if (location == false)
			for (int j = 1; j < tokens.size(); j++)
				args.push_back(tokens[j]);
		for (int j = 0; j < args.size(); j++)
			if (args[j][args[j].size() - 1] == ';')
				args[j] = ft::trim_char(args[j], ';');
		std::cout << tokens[0] << " ' " << args[0] << std::endl;
		call(tokens[0], args);
		args.clear();
	}
	return(i);
}
