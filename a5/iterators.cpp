#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <unordered_set>

using namespace std;

// function template for printing contents of containers to cout
template<class T>
void printContainer(T &container) {
    cout << "{";
    for (auto element : container)
        cout << element << " ";
    cout << "}" << endl;
}


// Merge front-back pairings of elements of inContainer into outContainer. Do *not* use the []-operator.
template<class T>
void front_back_pairing(T &inContainer, T &outContainer) {
    for (auto it = inContainer.begin(), rit = inContainer.rbegin();
         distance(it, inContainer.end()) > inContainer.size() / 2; ++it, ++rit) {
        outContainer.push_back(*it);
        if (&(*it) != &(*rit)) {
            outContainer.push_back(*rit);
        }
    }
}

// Remove all duplicates from the given container. Do *not* use the []-operator.
template<class T>
void remove_duplicates(T &container) {
    unordered_set<typename T::value_type> unique;
    container.erase(remove_if(container.begin(), container.end(), [&](typename T::value_type element) {
        return (unique.find(element) == unique.end()) ? (unique.insert(element), false) : true;
    }), container.end());
}

// Expand the given container by inserting the numerical differences of each element to its neighbors. Do *not* use the []-operator.
template<class T>
void insert_differences(T &container) {
    typename T::value_type previous, next;
    T result;
    previous = *(container.rbegin());
    for (auto it = container.begin(), it2 = ++container.begin();
         it != container.end(); ++it, ++it2) {
        next = (it2 == container.end()) ? *(container.begin()) : *it2;
        result.push_back(previous - *it);
        result.push_back(*it);
        result.push_back(next - *it);
        previous = *it;
    }
    container = result;
}

void testFrontBackPairingFunctionality() {
    // define sample data
    vector<int> sampleDataInt = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    list<string> sampleDataString = {"Die", "eines", "ist", "Gebrauch", "der", "Sprache", "in", "sein",
                                               "Wortes", "Bedeutung"};

    // test for integer vector
    vector<int> helpIntVector;
    front_back_pairing(sampleDataInt, helpIntVector);
    printContainer(sampleDataInt);
    printContainer(helpIntVector);

    // test for string list
    list<string> helpStringList;
    front_back_pairing(sampleDataString, helpStringList);
    printContainer(sampleDataString);
    printContainer(helpStringList);
}

void testRemoveDuplicateFunctionality() {
    // define sample data
    vector<int> sampleDataInt = {10, 11, 10, 13, 14, 15, 15, 15, 18, 19, 12, 11, 17};
    vector<string> sampleDataString = {"Die", "___", "eines", "ist", "Gebrauch", "der", "Sprache", "in",
                                                 "sein", "Wortes", "Bedeutung", "Die", "ist", "blabla"};

    // test for integer vector
    printContainer(sampleDataInt);
    //remove_duplicates(sampleDataInt, less<int>() );
    remove_duplicates(sampleDataInt);
    printContainer(sampleDataInt);

    // test for string vector
    printContainer(sampleDataString);
    remove_duplicates(sampleDataString);
    printContainer(sampleDataString);
}

void testAddDifferenceFunctionality() {
    // define sample data
    vector<int> sampleDataInt = {10, 11, 14, 16, 1, 18};

    // test for integer vector
    printContainer(sampleDataInt);
    insert_differences(sampleDataInt);
    printContainer(sampleDataInt);
}

int main(int argc, char **argv) {
    testFrontBackPairingFunctionality();
    testRemoveDuplicateFunctionality();
    testAddDifferenceFunctionality();

    return 0;
}
