#include "PmergeMe.hpp"

void PmergeMe::run(int argc, char** argv)
{
	std::vector<int> input = parseInput(argc, argv);
	std::vector<int> vec = input;

	std::deque<int> deq(input.begin(), input.end());

	display("Before", vec);

	clock_t startVec = clock();

	fordJohnsonSortVector(vec);
	clock_t endVec = clock();

	clock_t startDeq = clock();
	fordJohnsonSortDeque(deq);
	clock_t endDec = clock();

	display("After", vec);

	double timeVec = static_cast<double>(endVec - startVec) / CLOCKS_PER_SEC * 1e6;
	double timedeq = static_cast<double>(endDec - startDeq) / CLOCKS_PER_SEC * 1e6;

	std::cout << "Time to process a range of " << input.size() << " elements with std::vector : " << std::fixed << std::setprecision(5) << timeVec << " us\n";
	std::cout << "Time to process a range of " << input.size() << " elements with std::deque : " << std::fixed << std::setprecision(5) << timedeq << " us\n";
}

std::vector<int> PmergeMe::parseInput(int argc, char** argv)
{
	std::vector<int> result;

	for (int i = 1; i < argc; ++i)
	{
		validateNumber(argv[i]);
		int value = std::stoi(argv[i]);
		result.push_back(value);
	}
	return result;
}

void PmergeMe::validateNumber(const std::string& str)
{
	if (str.empty() || !std::all_of(str.begin(), str.end(), ::isdigit))
		throw std::runtime_error("Error: Invalid input -> " + str);
	long num = std::stol(str);
	if (num < 0 || num > INT32_MAX)
		throw std::runtime_error("Error: Out of range -> " + str);
}

void PmergeMe::display(const std::string& label, const std::vector<int>& vec)
{
	std::cout << label << ": ";
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << vec[i];
		if (i + 1 < vec.size())
			std::cout << " ";
	}
	std::cout << std::endl;
}

std::vector<int> PmergeMe::fordJohnsonSortVector(const std::vector<int>& vec)
{
	if (vec.size() <= 1)
		return vec;
	
	std::vector<int> S, L;

	for(size_t i = 0; i < vec.size(); i +=2)
	{
		if (i + 1 < vec.size())
		{
			if (vec[i] < vec[i + 1])
			{
				S.push_back(vec[i]);
				L.push_back(vec[i + 1]);
			}
			else
			{
				S.push_back(vec[i + 1]);
				L.push_back(vec[i]);
			}
		}
		else{
			S.push_back(vec[i + 1]);
		}
	}
	L = fordJohnsonSortVector(L);
	// for (size_t i = 0; i < S.size(); ++i)
	// 	L.insert(std::lower_bound(L.begin(), L.end(), S[i]),S[i]);
	// vec = L;
	return mergeInsert(S, L);
}

std::vector<int> PmergeMe::mergeInsert(const std::vector<int> &S, std::vector<int> L)
{
	std::vector<size_t> order = jacobsthalSequance(S.size());
	for (size_t i = 0; i < order.size(); ++i)
	{
		size_t index = order[i];
		if (index < S.size())
		{
			int value = S[index];
			L.insert(std::lower_bound(L.begin(), L.end(), value), value);
		}
	}
	return L;
}
std::vector<size_t> PmergeMe::jacobsthalSequance(size_t n)
{
	std::vector<size_t> seq;
	if(n == 0) return seq;
	seq.push_back(0);
	if (n == 1) return seq;

	size_t j1 = 1, j2 = 0;
	while (true)
	{
		size_t next = j1 + 2 * j2;
		if (next >= n) break;
		seq.push_back(next);
		j2 = j1;
		j1 = next;
	}

	for (size_t i = 0; i < n; ++i)
		if (std::find(seq.begin(), seq.end(), i) == seq.end())
			seq.push_back(i);
	return seq;
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& deq)
{
	if (deq.size() <= 1)
		return;
	
	std::deque<int> left,  right;
	
	for (size_t i = 0; i < deq.size(); i += 2)
	{
		if (i + 1 < deq.size())
		{
			if (deq[i] < deq[i + 1])
			{
				left.push_back(deq[i]);
				right.push_back(deq[i + 1]);
			}
			else{
				left.push_back(deq[i + 1]);
				right.push_back(deq[i]);
			}
		}
		else 
		{
			left.push_back(deq[i]);
		}
	}
	fordJohnsonSortDeque(right);
	for (size_t i = 0; i < left.size(); ++i)
		right.insert(std::lower_bound(right.begin(), right.end(), left[i]), left[i]);
	deq = right;
}