#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <iterator>

class PmergeMe
{
	public:
		static void run(int argc, char** arhgv);
	
	private:
		static std::vector<int> parseInput(int argc, char** argv);
		static void validateNumber(const std::string& str);
		static void fordJohnsonSortVector(std::vector<int>& vec);
		static void fordJohnsonSortDeque(std::deque<int>& deq);
		static void display(const std::string& label, const std::vector<int>& vec);
		
};

#endif