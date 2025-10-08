#include "BitcoinExchange.hpp"
#include <cstdlib>
#include <limits>


BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string &dbFile)
{
	loadDatabase(dbFile);
}
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
	_exchangeRates = other._exchangeRates;
}
BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		_exchangeRates = other._exchangeRates;
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDatabase(const std::string &dbFile)
{
	std::ifstream file(dbFile.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open database file." <<std::endl;
		// exit(1); // becureull with exit with leak
		return;
	}

	std::string line;
	std::getline(file, line); // skip header
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string date, value;
		if (std::getline(ss, date, ',') && std::getline(ss, value))
		{
			_exchangeRates[trim(date)] = std::strtof(value.c_str(), NULL);
		}
	}
}

bool BitcoinExchange::isValidDate(const std::string &date) const
{
	if (date.length() != 10 || date[4] != '-' || date[7] != '-')
		return false;
	int y, m, d;

	std::sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d);
	if ( y < 2009 )
		return false;

	if (m < 1 || m > 12)
		return false;

	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30 ,31 ,30, 31 };

	// Leap year adjustment for February
	if ( ( y% 4 == 0 && y % 100 != 0) || (y % 400 == 0))
		daysInMonth[1] = 29;

	if (d < 1 || d > daysInMonth[m - 1])
		return false;

	return true;

}

bool BitcoinExchange::isValidValue(const std::string &valueStr) const
{
	char *end;
	float val = std::strtof(valueStr.c_str(), &end);
	if (*end != '\0' || val < 0.0f || val > 1000.0f)
		return false;
	return true;
}

std::string BitcoinExchange::trim (const std::string &str) const{

		size_t start = str.find_first_not_of(" \t");
		size_t end = str.find_last_not_of(" \t");
		return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string BitcoinExchange::findClosesDate(const std::string &date) const
{
	std::map<std::string, float>::const_iterator it = _exchangeRates.upper_bound(date);
	if (it == _exchangeRates.begin())
		return "";
	--it;
	return it->first;
}

void BitcoinExchange::eveluate(const std::string &inputFile)
{
	if (_exchangeRates.size() == 0)
		return;
	std::ifstream file(inputFile.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open file." <<std::endl;
		return;
	}
	std::string line;
	std::getline(file, line);
	while (std::getline(file, line))
	{
		size_t pipePos = line.find('|');
		if (pipePos == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		std::string date = trim(line.substr(0, pipePos));
		std::string valueStr = trim(line.substr(pipePos + 1));

		if (!isValidDate(date))
		{
			std::cerr << "Error: bad input => " << date << std::endl;
			continue;
		}

		if (!isValidValue(valueStr))
		{
			if (valueStr.find('-') != std::string::npos)
				std::cerr << "Error: not a positive number." << std::endl;
			else
				std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		float value = std::strtof(valueStr.c_str(), NULL);
		std::string closesDate = findClosesDate(date);
		if (closesDate.empty())
		{
			std::cerr << "Error: no available exchange rate for date => " << date << std::endl;
			continue;
		}

		float rate = _exchangeRates.find(closesDate)->second;
		std::cout << date << " => " << value << " = " << (value *rate) << std::endl;
	}
}