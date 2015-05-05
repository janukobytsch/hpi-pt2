
#include <cstdlib>
#include <iostream>
#include <string>
#include <limits>

using namespace std;

int int_max = std::numeric_limits<int>::max();

long triangular(int number)
{
	long result = (static_cast<long>(number)*(number+1))/2;
	if (result > int_max){
		return 0;
	}
	return result;
}

string pretty_format(long decimal)
{
	string str = to_string(decimal);
	int pos = str.length() - 3;

	while (pos > 0) {
		str.insert(pos, ".");
		pos -= 3;
	}
	return str;
}

void pretty_print(int decimal)
{
	std::cout << pretty_format(decimal) << "\n";
}

int main(int argc, char * argv[])
{
	if(argc != 2)
		return 1;	// invalid number of parameters

	int n = std::atoi(argv[1]);
	long tri = triangular(n);

	// in domain
	if (tri != 0) {
		pretty_print(tri);
	} else {
		int dmin = 1;
		int dmax = 65535; // by solving gaussian sum for imax
		long cdmin = triangular(dmin);
		long cdmax = triangular(dmax);

		if (tri > dmax || tri < dmin) {
			std::cout << "domain = [" << pretty_format(dmin) << ";" << pretty_format(dmax) << "],";
			std::cout << "codomain = [" << pretty_format(cdmin) << ";" << pretty_format(cdmax) << "]" << std::endl;
		}
		return 2;
	}

	return 0;
}
