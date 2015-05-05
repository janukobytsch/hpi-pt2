
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>
#include <map>

using namespace std;

map<int,int> change(const int due, const int paid)
{
    array<int,15> coins = {1,2,5,10,20,50,100,200,500,1000,2000,5000,10000,20000,50000};
    map<int,int> changes;
    int diff = paid-due;

    // greedy algorithm will suffice for canonical coin systems
    for (int i=coins.size()-1;i>=0;i--) {
        int num = floor(diff/coins[i]);
        if (num > 0) {
            // map from denomination to amount
            changes[coins[i]] = num;
            diff -= coins[i]*num;
        }
        if (diff <= 0) {
            break;
        }
    }

    return changes;
}

void print_csv(const map<int,int>& m, ostream &out) {
    map<int,int>::const_reverse_iterator it;
    out << "coin,num" << endl;
    for (it=m.rbegin(); it!=m.rend(); ++it) {
        out << "" << it->first << "," << it->second << endl;
    }
}

void print_file(const map<int,int>& m, const string filename) {
    ofstream output (filename);
    if (output.is_open())
    {
        print_csv(m, output);
        output.close();
        cout << "File created." << endl;
    } else {
        cout << "Unable to open file" << endl;
    }
}

int main(int argc, char * argv[])
{
	if(argc != 3 && argc != 5)
		return 1;	// invalid number of parameters

    if (argc == 5) {
        if (string(argv[3]) != "-o") {
            return 2;
        }
    }

	const int due  = atoi(argv[1]);
	const int paid = atoi(argv[2]);

    if (due < 0 || paid < 0
            || paid-due < 0) {
        return 2;
    }

	map<int,int> changes = change(due, paid);

    if (argc != 5) {
        print_csv(changes, cout);
    } else {
        const string filename = string(argv[4]);
        print_file(changes, filename);
    }

	return 0;
}
