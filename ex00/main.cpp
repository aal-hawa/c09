#include "BitcoinExchange.hpp"

int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		std ::cerr << "Usage: ./btc <input_file>" << std::endl;
		return 1;
	}

	BitcoinExchange btc("data.csv");
	btc.eveluate(argv[1]);
	return 0;
}