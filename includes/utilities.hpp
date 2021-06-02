#ifndef UTILITIES_HPP
# define UTILITIES_HPP


namespace ft
{
	std::string& ltrim(std::string& s, std::string delim = " \t");
	std::vector<std::string> split(std::string str, std::string delim = " \t\n");
	std::string trim_char(std::string str, char delim);
	int stoi(std::string number, const std::string base = "0123456789");
	bool ends_with(std::string const & value, std::string const & ending);
	std::vector<std::string> get_lines(std::string file);
	char *strdup(char *str);
	std::string to_string(int val);
	void str_add(string &res, string to_push);
}

#endif
