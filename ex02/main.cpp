#include "PmergeMe.hpp"

int main(int argc, char** argv)
{
	try
	{
		if (argc < 2)
			throw std::runtime_error("Error: Invalid number of arguments.");
		PmergeMe::run(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}