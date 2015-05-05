
#include <cstdlib>
#include <iostream>
#include <limits>

int int_max = std::numeric_limits<int>::max();
int num_calculations = 0;

int fibonacci(int number)
{
	if (number < 1) {
		return 0;
	}
	int fib1 = 0;
	int fib2 = 1;

	for (int i = 2; i < number; i++) {
		if ((long) fib1 + fib2 > int_max) {
			return 0;
		}
		int temp = fib2;
		fib2 = fib2 + fib1;
		fib1 = temp;
		num_calculations++;
	}

	return fib1 + fib2;
}

int main(int argc, char * argv[])
{
	if(argc != 2)
		return 1;	// invalid number of parameters

	int n = std::atoi(argv[1]);
	int fib = fibonacci(n);
	std::cout << n << " : " << fib << " : #" << num_calculations << std::endl;

	return 0;
}
