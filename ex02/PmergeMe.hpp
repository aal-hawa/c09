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
#include <cstdlib>
#include <climits>
#include <cctype>

class PmergeMe
{
	public:
		static void run(int argc, char** arhgv);
	
	private:
		PmergeMe();
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);
		~PmergeMe();


		static std::vector<int> parseInput(int argc, char** argv);
		static void validateNumber(const std::string& str);
		static void display(const std::string& label, const std::vector<int>& vec);
		static std::vector<int> fordJohnsonSortVector(const std::vector<int>& vec);
		static std::vector<int> mergeInsert(const std::vector<int> &S, std::vector<int> L);
		static std::vector<size_t> jacobsthalSequance(size_t n); 


		static std::deque<int> fordJohnsonSortDeque(const std::deque<int>& deq);
		static std::deque<int> mergeInsertDeque(const std::deque<int> &S, std::deque<int> L);

	};
	
#endif