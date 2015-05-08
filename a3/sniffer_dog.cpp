#include <climits>
#include <iostream>
#include <vector>
#include <algorithm>

int minTimeDogTraining(int searchValueX, int searchValueY, const std::vector<int> &boxes)
{
	bool foundX = false;
	bool foundY = false;

	for (int i = 0; i < boxes.size() / 2; ++i) {
		int left = boxes.at(i);
		int right = boxes.at(boxes.size() - i - 1);

		if (left == searchValueX || right == searchValueX) {
			foundX = true;
		}

		if (left == searchValueY || right == searchValueY) {
			foundY = true;
		}

		if (foundX && foundY) {
			return i + 1;
		}
	}

	return -1;
}

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		std::cout << "Missing parameters. Usage: <searchValue1> <searchValue2> [box1] [box2] [...]" << std::endl;
		return 0;
	}

	const int searchValueX = std::atoi(argv[1]);
	const int searchValueY = std::atoi(argv[2]);
	if(searchValueX < 1 || searchValueY < 1)
	{
		std::cerr << "Search values must be greater than zero!";
	}
	
	if (searchValueX == searchValueY)
	{
		std::cerr << "Search values must be different!";
		return 1;
	}

	std::vector<int> boxes;
	for (int i = 3; i < argc; i++)
	{
		int value = std::atoi(argv[i]);
		if (value < 1)
		{
			std::cerr << "Box value must be greater than zero!";
			return 2;
		}

		boxes.push_back(std::atoi(argv[i]));
	}

	int minTime = minTimeDogTraining(searchValueX, searchValueY, boxes);
	if (minTime == -1)
	{
		std::cout << "There exists no two boxes with the given search values!" << std::endl;
	}
	else
	{
		std::cout << "The coach will have to wait at least " << minTime << " seconds!" << std::endl;
	}
	
	return 0;
}