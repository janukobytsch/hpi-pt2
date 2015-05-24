#include <functional>
#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>

struct CommandLineParameter
{
	CommandLineParameter(int argc, char* argv[])
		: m_fibonacci(false)
		, m_centeredTriangular(false)
		, m_powSum(false)
	{
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-fib"))
			{
				m_fibonacci = true;
			}
			else if (!strcmp(argv[i], "-ct"))
			{
				m_centeredTriangular = true;
			}
			else if (!strcmp(argv[i], "-pow"))
			{
				m_powSum = true;
			}
		}
	}

	bool m_fibonacci;
	bool m_centeredTriangular;
	bool m_powSum;
};

int h_fibonacci(int n, int f1, int f2)
{
	if (n <= 1) {
		return f1 + f2;
	}
	return h_fibonacci(n - 1, f2, f1+f2);
}

int fibonacci(int number)
{
	if (number < 1) {
		return 0;
	}
	return h_fibonacci(number - 1, 0, 1);
}

int centeredTriangular(int number)
{
	return (3 * number * number + 3 * number + 2) / 2;
}

int powSum(int number, int exp)
{
	int sum = 0;
	for (int i = 1; i <= number; i++) {
		sum += pow(i, exp);
	}
	return sum;
}

// ToDo 4.2c - Call each function inside the functions vector. Print the results via std::cout
void applyFuntions(std::vector<std::function<int(int)>> & functions, int number)
{
	std::cout << std::endl;
}

int main(int argc, char * argv[])
{
	std::vector<std::function<int(int)>> functions;
	CommandLineParameter cmd(argc, argv);
	
	if (cmd.m_fibonacci)
	{
		// ToDo 4.2b - Add the fibonacci function directly to the functions vector
	}
	if (cmd.m_centeredTriangular)
	{
		// ToDo 4.2b - Add a lambda expression that calls the centeredTriangular function to the functions vector
	}
	if (cmd.m_powSum)
	{
		// ToDo 4.2b - Add the powSum function using a std::bind expression to the functions vector. Use a constant value (e.g., 2) for the exp argument 
	}

	for (int i = 1; i < 45; i++)
	{
		applyFuntions(functions, i);
	}
}
