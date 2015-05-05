
#include <cstdlib>
#include <iostream>

// memoization array
long fib[100] {0,1};

// recursion + dynamic programming
long combinations(long number)
{
	if (number == 0) {
        return fib[0];
    }
    if (number == 1) {
        return fib[1];
    }

    // calculate and store missing values
    if (fib[number-2] == 0) {
        combinations(number-2);
    }
    if (fib[number-1] == 0) {
        combinations(number-1);
    }

    fib[number] = fib[number-1]+fib[number-2];

    return fib[number];
}

int main(int argc, char * argv[])
{
	if(argc != 2)
		return 1;	// invalid number of parameters

	int n = std::atoi(argv[1]);

	std::cout << combinations(n) << std::endl;

	return 0;
}
