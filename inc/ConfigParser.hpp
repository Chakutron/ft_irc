#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <map>

class ConfigParser
{
public:
	ConfigParser(const std::string& filename);
	std::string get(const std::string& key, const std::string& default_value = "") const;
	int getInt(const std::string& key, int default_value = 0) const;
	void set(const std::string& key, const std::string& value);
    void save() const;

private:
	std::map<std::string, std::string> config;
	void parse(const std::string& filename);
};

#endif // CONFIG_PARSER_HPP