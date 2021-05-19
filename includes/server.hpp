#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <location.hpp>

using namespace std;

typedef struct	s_err_page
{
	int		html_error_code;
	string	location;
}	err_page;

class Server
{

	public:
		vector<int>					_port;
		string						_server;
		vector<string>				_server_identifier;
		vector<err_page>			_error_pages;
		unsigned int				_max_body_size;
		vector<Location>			_locations;
		
		Server();

	public:
		~Server();
		Server(vector<string> input);
		Server(const Server &tbc);
		Server & operator=(const Server &tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(vector<string> arr, int i);
		void 	load_ports(vector<string> val);
		void 	load_server_identifier(vector<string> val);
		void 	load_client_max_body_size(vector<string> val);
		void	load_locations(vector<string> val);
};

std::ostream &operator<<(std::ostream &out, Server const &value);


#endif
