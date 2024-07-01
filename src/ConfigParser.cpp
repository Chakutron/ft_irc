#include "../inc/ConfigParser.hpp"
#include "../inc/IRCException.hpp"
#include "../inc/Logger.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

ConfigParser::ConfigParser(const std::string& filename)
{
	parse(filename);
}

void ConfigParser::parse(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		throw IRCException("Unable to open config file: " + filename);
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string key, value;
		if (std::getline(iss, key, '=') && std::getline(iss, value))
		{
			config[key] = value;
		}
	}
}

std::string ConfigParser::get(const std::string& key, const std::string& default_value) const
{
	std::map<std::string, std::string>::const_iterator it = config.find(key);
	if (it != config.end())
	{
		return it->second;
	}
	return default_value;
}

int ConfigParser::getInt(const std::string& key, int default_value) const
{
	std::string value = get(key);
	if (!value.empty())
	{
		try
		{
			return std::atoi(value.c_str());
		}
		catch (const std::exception& e)
		{
			LOG_WARNING("Invalid integer value for key: " + key);
		}
	}
	return default_value;
}

void ConfigParser::set(const std::string& key, const std::string& value)
{
    config[key] = value;
}

void ConfigParser::save() const
{
	std::string filename;
    std::ofstream configFile(filename.c_str());
    if (configFile.is_open())
	{
        for (std::map<std::string, std::string>::const_iterator it = config.begin(); it != config.end(); ++it)
		{
            configFile << it->first << "=" << it->second << std::endl;
        }
        configFile.close();
    }
}