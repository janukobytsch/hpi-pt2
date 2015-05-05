
#include <cstdlib>
#include <iostream>
#include <limits>

int int_max = std::numeric_limits<int>::max();
int num_calculations = 0;

// naiver ansatz
// int fibonacci(int number)
// {
// 	if (number == 0) {
// 		return 0;
// 	}
// 	if (number == 1) {
// 		return 1;
// 	}

// 	return fibonacci(number-1) + fibonacci(number-2);
// }

// linearisierung mittels endrekursion
int h_fibonacci(int n, int fk1, int fk2) {
	if ((long) fk2+fk1 > int_max) {
		return 0;
	}
	num_calculations++;
	if (n <= 1) {
		return fk1 + fk2;
	}
	return h_fibonacci(n-1, fk2+fk1, fk1);
}

int fibonacci(int number) {
	if (number < 1) {
		return 0;
	}
	return h_fibonacci(number-1, 1, 0);
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
