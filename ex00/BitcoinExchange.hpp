#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class BitcoinExchange
{
	public:
		BitcoinExchange(const std::string &dbFile);
		~BitcoinExchange();

		void eveluate(const std::string &inputFile);

	private:
		std::map<std::string, float> _exchangeRates;

		bool isValidDate(const std::string &date) const;
		bool isValidValue(const std::string &valueStr) const;
		std::string trim (const std::string &str) const;
		std::string findClosesDate(const std::string &date) const;
		void loadDatabase(const std::string &dbFile);
};

#endif