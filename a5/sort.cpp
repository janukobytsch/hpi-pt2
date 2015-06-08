#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <cassert>

// function template for printing contents of containers to std::cout
template<class T>
void printContainer(T& container)
{
	std::cout << "{";
	for (auto element : container)
		std::cout << element << " ";
	std::cout << "}" << std::endl;
}

// Merge the given lists [leftIt..midIt) and [midIt..rightIt)
template<class T>
void merge(T leftIt, T midIt, T rightIt)
{
	assert(leftIt <= midIt && midIt <= rightIt);

	const auto leftSize = std::distance(leftIt, midIt);
	const auto rightSize = std::distance(midIt, rightIt);
	int countLeft = 0;
	int countRight = 0;

	std::vector<typename T::value_type> merged(leftSize + rightSize);

	while (countLeft < leftSize && countRight < rightSize) {
		auto leftElem = *(leftIt + countLeft);
		auto rightElem = *(midIt + countRight);
		if (leftElem <= rightElem) {
			merged[countLeft + countRight] = leftElem;
			++countLeft;
		} else {
			merged[countLeft + countRight] = rightElem;
			++countRight;
		}
	}
	while (countLeft < leftSize) {
		auto leftElem = *(leftIt + countLeft);
		merged[countLeft + countRight] = leftElem;
		++countLeft;
	}
	while (countRight < rightSize) {
		auto rightElem = *(midIt + countRight);
		merged[countLeft + countRight] = rightElem;
		++countRight;
	}

	for(int i = 0; i < countLeft + countRight; ++i) {
		*(leftIt + i) = merged[i];
	}
}

// Sort the given container using merge sort.
template<class T>
void mergeSort(T leftIt, T rightIt)
{
	assert(leftIt <= rightIt);

	auto size = std::distance(leftIt, rightIt);
	if (size <= 1) {
		return;
	}

	auto midIt = leftIt + size / 2;
	mergeSort(leftIt, midIt);
	mergeSort(midIt, rightIt);
	merge(leftIt, midIt, rightIt);
}

int main(int argc, char** argv)
{
	// define sample data
	std::vector<int> sampleDataInt = { 10, 1, 12, 33, 24, 5, 6, -7, -2, 19 };
	std::vector<std::string> sampleDataString = { "Die", "eines", "ist", "Gebrauch", "der", "Sprache", "in", "sein", "Wortes", "Bedeutung" };

	// test for integer vector
	printContainer(sampleDataInt);
	mergeSort(sampleDataInt.begin(), sampleDataInt.end());
	printContainer(sampleDataInt);

	// test for string vector
	printContainer(sampleDataString);
	mergeSort(sampleDataString.begin(), sampleDataString.end());
	printContainer(sampleDataString);

	return 0;
}
