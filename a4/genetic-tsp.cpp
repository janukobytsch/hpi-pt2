#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <random>
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <ctime>

using namespace std;

// array printing
ostream &operator<<(ostream &os, const vector<int> &A) {
    os << A[0];
    for (int i = 1; i < A.size(); ++i) os << "," << A[i];
    return os;
}

// random number in [a,b] range
inline float frand(float a, float b) {
    return a + ((b - a) * (float) rand() / (float) RAND_MAX);
}

// TODO tourset struct

// length of sequence, i.e., number of cities
const int numCities = 20;

// population size per generation, i.e., no of tours
const int populationSize = 20;

// null tour with indices initialized to -1
const vector<int> NullTour(numCities, -1);

// globale minimum of tour length, over all generations
int minTourLength = numeric_limits<int>::max();

// globale maximum of tour length, over all generations
int maxTourLength = numeric_limits<int>::min();

// real data for N=20 (German city distances)
const vector<string> cityName{
        "Berlin", "Hamburg", "München", "Köln", "Frankfurt am Main", "Stuttgart", "Düsseldorf", "Dortmund", "Essen",
        "Bremen", "Dresden", "Leipzig", "Hannover", "Nürnberg", "Duisburg", "Bochum", "Wuppertal", "Bielefeld", "Bonn",
        "Münster"
};

const int distance_table[numCities][numCities] = {
        {000, 255, 504, 477, 424, 512, 477, 422, 453, 315, 165, 149, 249, 378, 471, 439, 452, 336, 479, 398},
        {255, 000, 612, 356, 393, 534, 338, 284, 308, 95,  377, 294, 132, 462, 321, 297, 320, 196, 370, 237},
        {504, 612, 000, 456, 304, 190, 486, 477, 493, 582, 359, 360, 488, 150, 503, 486, 470, 483, 433, 510},
        {477, 356, 456, 000, 152, 288, 34,  72,  57,  269, 474, 380, 249, 336, 56,  62,  37,  162, 25,  123},
        {424, 393, 304, 152, 000, 152, 182, 177, 190, 329, 371, 293, 261, 187, 200, 184, 167, 212, 131, 218},
        {512, 534, 190, 288, 152, 000, 322, 328, 335, 478, 412, 364, 401, 157, 342, 332, 311, 364, 264, 371},
        {477, 338, 486, 34,  182, 322, 000, 57,  30,  248, 485, 389, 240, 363, 23,  41,  26,  150, 59,  101},
        {422, 284, 477, 72,  177, 328, 57,  000, 32,  196, 439, 340, 182, 343, 49,  17,  36,  93,  90,  51},
        {453, 308, 493, 57,  190, 335, 30,  32,  000, 217, 470, 371, 212, 364, 18,  14,  24,  122, 81,  70},
        {315, 95,  582, 269, 329, 478, 248, 196, 217, 000, 405, 310, 100, 433, 229, 207, 231, 118, 285, 150},
        {165, 377, 359, 474, 371, 412, 485, 439, 470, 405, 000, 100, 312, 259, 487, 456, 460, 375, 467, 434},
        {149, 294, 360, 380, 293, 364, 389, 340, 371, 310, 100, 000, 214, 229, 389, 358, 363, 275, 375, 334},
        {249, 132, 488, 249, 261, 401, 240, 182, 212, 100, 312, 214, 000, 338, 229, 199, 217, 90,  258, 151},
        {378, 462, 150, 336, 187, 157, 363, 343, 364, 433, 259, 229, 338, 000, 377, 355, 343, 337, 318, 370},
        {471, 321, 503, 56,  200, 342, 23,  49,  18,  229, 487, 389, 229, 377, 000, 32,  33,  138, 81,  84},
        {439, 297, 486, 62,  184, 332, 41,  17,  14,  207, 456, 358, 199, 355, 32,  000, 26,  109, 84,  61},
        {452, 320, 470, 37,  167, 311, 26,  36,  24,  231, 460, 363, 217, 343, 33,  26,  000, 128, 58,  85},
        {336, 196, 483, 162, 212, 364, 150, 93,  122, 118, 375, 275, 90,  337, 138, 109, 128, 000, 175, 62},
        {479, 370, 433, 25,  131, 264, 59,  90,  81,  285, 467, 375, 258, 318, 81,  84,  58,  175, 000, 142},
        {398, 237, 510, 123, 218, 371, 101, 51,  70,  150, 434, 334, 151, 370, 84,  61,  85,  62,  142, 000}
};

bool isUninitialized(const vector<vector<int> > &tourSet) {
    return all_of(tourSet.begin(), tourSet.end(), [](vector<int> tour) -> bool {
        return all_of(tour.begin(), tour.end(), [](int city) -> bool { return city == -1; });
    });
}

bool isValidCity(const int city) {
    return city >= 0 && city < numCities;
}

// check if city is in tour
bool hasCity(const vector<int> &tour, int city) {
    assert(isValidCity(city));
    assert(tour.size() == numCities);

    // a valid tour always returns true as the tour must include all cities
    // during evolution, tours temporarily may have undefined slots however
    for (int i = 0; i < numCities; ++i) {
        if (tour[i] == city) return true;
    }
    return false;
}

// check if a tour is completely defined
bool isDefinedTour(const vector<int> &tour) {
    // all tours have N distinct cities
    // if a tour has undefined slots, they are marked by -1
    for (int i = 0; i < numCities; ++i) {
        if (tour[i] == -1) return false;
    }
    return true;
}

// check that all cities are included in a complete tour
bool isValidTour(const vector<int> &tour) {
    for (int i = 0; i < numCities; ++i) {
        if (!hasCity(tour, i)) return false;
    }
    return true;
}

// returns distance between two cities based on the distance table
int cityDistance(int city1, int city2) {
    assert(isValidCity(city1));
    assert(isValidCity(city2));

    return distance_table[city1][city2];
}

// calculate the length for a given tour, assuming a round trip
int tourLength(const vector<int> &T) {
    assert(isDefinedTour(T));
    assert(isValidTour(T));

    int tourLength = 0;

    for (int i = 0; i < numCities; i++) {
        tourLength += cityDistance(T[i], T[i + 1]);
    }
    tourLength += cityDistance(T[numCities], T[0]);

    if (tourLength < minTourLength) {
        minTourLength = tourLength;
    }

    if (tourLength > maxTourLength) {
        maxTourLength = tourLength;
    }

    return tourLength;
}

//  print city names of a tour
void printTourCityNames(const vector<int> &T) {
    assert(isValidTour(T));
    for (int i = 0; i < numCities; ++i) cout << (i == 0 ? "" : "-") << cityName[T[i]];
    cout << endl;
}

// inserts a city in an incomplete tour, using the next free slot
bool insertCity(vector<int> &tour, int city) {
    assert(isValidCity(city));
    for (int & stop : tour) {
        if (stop != -1) continue;
        stop = city;
        return true;
    }
    return false;
}

void initializeTour(vector<int> & tour) {
    random_device rd;
    mt19937 g(rd());

    vector<int> cities(numCities);
    for (int i = 0; i < numCities; ++i) {
        cities[i] = i;
    }
    shuffle(cities.begin(), cities.end(), g);
    tour = cities;
}

// generate initial population and store them in the tour set
void initializeTours(vector<vector<int> > &tourSet) {
    assert(isUninitialized(tourSet));

    for (auto &tour : tourSet) {
        initializeTour(tour);
    }
}

// take two (good) parent tours, and build a new one by the gens of both
void crossover(const vector<int> &parent1, const vector<int> &parent2, vector<int> &child) {
    assert(isValidTour(parent1));
    assert(isValidTour(parent2));
    assert(&parent1 != &child);
    assert(&parent2 != &child);

    const int minSliceSize = (int const) (numCities * frand(0.3, 0.7));

    child = NullTour; // reset child since we don't keep anything

    int sliceStart = (int) frand(0, numCities - minSliceSize);
    int sliceEnd = (int) frand(sliceStart, numCities);

    // copy over slice from first parent
    for (int i = sliceStart; i < sliceEnd; ++i) {
        child[i] = parent1[i];
    }

    // fill gaps with cities from second parent
    for (auto & city : parent2) {
        if (!hasCity(child, city)) {
            if (!insertCity(child, city)) {
                // no more space
                break;
            }
        }
    }

    assert(isValidTour(child));
}

// ToDo: Mutate a given tour, swapping cities randomly based on probability. Hint: Use frand and std::swap.
void mutate(vector<int> &tour, const float mutationProbability = 0.01f) {
    assert(isValidTour(tour));

    //const float mutationProbability = 0.01f; // x% probability per city in a tour to get mutated

    for (int i = 0; i < tour.size(); ++i) {
        if (frand(0,1) < mutationProbability) {
            // swap with random city
            int index;
            do {
                index = (int) frand(0, numCities);
            } while (index == i);
            std::swap(tour[i], tour[index]);
        }
    }

    assert(isValidTour(tour));
}

// create a sorted sequence of (tour length, tour index) pairs (ascending order, i.e., the shorter, the better)
vector<pair<int, int>> fitness(const vector<vector<int>> &tourSet) {
    vector<pair<int, int>> fitness(populationSize);

    assert(all_of(tourSet.begin(), tourSet.end(), [](vector<int> tour) { return isValidTour(tour); }));

    for (int i = 0; i < tourSet.size(); ++i) {
        const vector<int>& tour = tourSet[i];
        int length = tourLength(tour);
        pair<int,int> pair(length, i);
        fitness[i] = pair;
    }

    sort(fitness.begin(), fitness.end(), [](const pair<int,int> &left, const pair<int,int> &right) {
        return left.first < right.first;
    });

    return fitness;
}

// evolution step: transform the tour set into the next generation tour set
pair<int, int> evolution(vector<vector<int>> &tourSet, bool elite) {
    assert(tourSet.size() == populationSize);
    pair<int, int> statistics; // used as return values (min/max tour lengths)
    
    const int eliteCount = 2; // number of elite individuals guaranteed to survive

    // compute fitness of tours, store shortest and largest tour length in statistics
    auto score = fitness(tourSet);
    statistics.first = score[0].first; // shortest tour length
    statistics.second = score[populationSize - 1].first; // largest tour length

    // this is our dschingis khan, i.e. top result
    vector<int>& leader = tourSet[score[0].second];

    // replace bottom quarter by top quarter crossed with leader
    int interval = ceil(populationSize / 4);
    for (int i = 0; i < interval; ++i) {
        vector<int>& parent2 = tourSet[score[i+1].second];
        vector<int>& child = tourSet[score[populationSize - 1 - i].second];
        crossover(leader, parent2, child);
    }

    // randomize third quarter
    for (int i = interval; i <interval*2; ++i) {
        mutate(tourSet[i], frand(0.01f, 0.05f));
    }

    // consider elite

    // mutate all other tours (ignore two best trips and the former worst trip (replaced))
    /*for (int i = 2; i < populationSize - 1; ++i) {
        mutate(tourSet[i]);
    }*/



    return statistics;
}


int main(int argc, char **argv) {
    assert(numCities > 0);
    assert(populationSize >= 2);

    // reset random generator
    srand((unsigned int) time(0));

    // report shortest/longest tour length for each evolution iteration in a csv file
    //std::ofstream stats("output.csv", std::ios::binary);
    //stats << "min tour length" << "; " << "max tour length" << endl;

    // generate randomly a tour set with M tours, each one between N cities
    vector<vector<int>> TourSet(populationSize, NullTour);
    initializeTours(TourSet);

    for (auto &tour : TourSet) {
        printTourCityNames(tour);
    }

    // do a fixed number of evolution steps
    for (int e = 0; e < 5000; e++) {
        auto lengths = evolution(TourSet, false);

        // report statistics
        //stats << lengths.first << "; " << lengths.second << endl;
        cout << "Evolution " << e << endl;
        cout << lengths.first << "; " << lengths.second << endl;
    }

    // after evolution iterations, log on last tour
    auto FE = fitness(TourSet);
    cout << "final shortest trip:  " << FE[0].first << "km" << endl;
    printTourCityNames(TourSet[FE[0].second]);
    cout << "over all generations, min dist = " << minTourLength << ", max dist = " << maxTourLength << endl;

    // close the log file (use excel to visualize data)
    //stats.close();

    return 0;
}

