#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <assert.h>

struct Route
{
	int airlineId;
	int sourceId;
	int destinationId;
};

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

const int START_ID = 1;
const int END_ID = 9541;

bool operator<(const Route& r1, const Route& r2) {
	return r1.destinationId < r2.destinationId;
}

void importRoutesData(char* path, std::vector<Route>& routes)
{
	std::cout << "Importing routes data.." << std::endl;
	std::ifstream file(path);
	std::string field, line;
	
	while (std::getline(file, line))
	{
		std::istringstream linestream;
		linestream.str(line);
		int fieldNum = 0;
		Route route;
		route.airlineId = route.sourceId = route.destinationId = -1;

		while (std::getline(linestream, field, ';'))
		{
			std::cout << field;
			try
			{
				switch (fieldNum)
				{
					case 1: // airline id
						route.airlineId = std::stoi(field);
						break;
					case 3: // source id
						route.sourceId = std::stoi(field);
						break;
					case 5: // dest id
						route.destinationId = std::stoi(field);
						break;
					default:
						break;
				}
			}
			catch (const std::invalid_argument&)
			{
				//std::cout << "Couldn't convert field correctly (invalid argument)!" << std::endl;
				//std::cout << field << std::endl;
			}
			catch (const std::out_of_range&)
			{
				//std::cout << "Couldn't convert field correctly (out of range)!" << std::endl;
				//std::cout << field << std::endl;
			}

			fieldNum++;
		}

		if (route.airlineId > -1 && route.sourceId > -1 && route.destinationId > -1)
			routes.push_back(route);
	}
}

// Return the number of routes for the given destination id based on a linear search. Count the number of lookups.
int linearSearch(const int destID, const std::vector<Route>& routes, long long& numLookups)
{
	int numRoutes = 0;

	for (auto route : routes) {
		if (route.destinationId == destID) {
			++numRoutes;
		}
		++numLookups;
	}

	return numRoutes;
}

// Evaluate the linearSearch function by calling it for every possible destination id (1..9541).
// Return the number of lookups and the processing time as a pair of long longs.
std::pair<long long, long long> evaluateLinearSearch(std::vector<Route>& routes)
{
	long long numLookups = 0;
	long long duration = 0;

	Clock::time_point t0 = Clock::now();

	for (int i = START_ID; i <= END_ID; ++i) {
		linearSearch(i, routes, numLookups);
	}

	Clock::time_point t1 = Clock::now();
	milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	duration = ms.count();

	return std::make_pair(numLookups, duration);
}

int searchLeftRightFrom(const int mid, const int destID, const std::vector<Route>& routes, long long& numLookups)
{
	int numRoutes = (routes[mid].destinationId == destID) ? 1 : 0;

	bool continueLeft = true;
	bool continueRight = true;
	long left2 = mid - 1;
	long right2 = mid + 1;

	while (continueLeft || continueRight) {
		if (left2 <= 0 || routes[left2].destinationId != destID) {
			continueLeft = false;
		}
		if (right2 >= routes.size() || routes[right2].destinationId != destID) {
			continueRight = false;
		}
		if (routes[left2].destinationId == destID) {
			++numRoutes;
			++numLookups;
			--left2;
		}
		if (routes[right2].destinationId == destID) {
			++numRoutes;
			++numLookups;
			++right2;
		}
	}

	return numRoutes;
}

// Return the number of routes for the given destination id based on a binary search. Count the number of lookups.
// The vector should have been sorted before calling this function.
int binarySearch(const int destID, const std::vector<Route>& routes, long long& numLookups)
{
	int numRoutes = 0;

	long left = 0;
	long right = routes.size();

	while (left <= right) {
		long mid = left + (right - left) / 2;
		if (routes[mid].destinationId == destID) {
			numRoutes += searchLeftRightFrom(mid, destID, routes, numLookups);
			break;
		} else {
			if (routes[mid].destinationId > destID) {
				right = mid - 1;
			} else {
				left = mid + 1;
			}
		}
		++numLookups;
	}

	return numRoutes;
}

// Evaluate the binarySearch function by calling it for every possible destination id (1..9541).
// Return the number of lookups and the processing time as a pair of long longs.
// Use std::chrono for time measurement.
// Attention: sorting is *not* part of the evaluation and should be conducted beforehand.
std::pair<long long, long long> evaluateBinarySearch(std::vector<Route>& routes)
{
	long long numLookups = 0;
	long long duration = 0;

	Clock::time_point t0 = Clock::now();

	for (int i = START_ID; i <= END_ID; ++i) {
		binarySearch(i, routes, numLookups);
	}

	// sorting time is not taken into consideration
	Clock::time_point t1 = Clock::now();
	milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	duration = ms.count();

	return std::make_pair(numLookups, duration);
}

void sort_routes(std::vector<Route>& routes) {
	std::sort(routes.begin(), routes.end(),[](Route route1, Route route2) {
		return route1.destinationId <= route2.destinationId;
	});
}

int main(int argc, char * argv[])
{
	if(argc != 2)
	{
		std::cout << "not enough arguments - USAGE: sort [ROUTE DATASET]" << std::endl;
		return -1;	// invalid number of parameters
	}

	static std::vector<Route> routes;

	std::cout << "Given path to routes.csv: " << argv[1] << std::endl;

	importRoutesData(argv[1], routes);

	auto result1 = evaluateLinearSearch(routes);
	std::cout << result1.first << " lookups - " << result1.second << " milliseconds" << std::endl;

	sort_routes(routes);
	auto result2 = evaluateBinarySearch(routes);
	std::cout << result2.first << " lookups - " << result2.second << " milliseconds" << std::endl;

	return 0;
}
