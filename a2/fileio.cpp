#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

#include <vector>

std::ofstream logFile;		// global variable for fileio.log, the log-file is only created if there is at least one mistake

bool isValueCorrect(const std::string &teststring, const int &column) {

	std::regex regExp;
	
	switch (column)	{

        case 1:		//regExp for column 1, name
            regExp = "[a-zA-Z]+";
            break;

        case 5:		//regExp for column 5, ICAO
            regExp = "\"([A-Z]{4})\"|(\"\")";		// regExp without escaping: "([A-Z]{4})"|("")
            break;

        case 8:		//regExp for column 8, altitude
        	//regExp = "([^-][1-9]([0-9]{0,4}))";	// according to Wikipedia, the highest airport is located at 4411 meters = 14 471.78 feet
        	regExp = "((^(?!-))[1-9]([0-9]{0,4}))";
        	break;

        case 10:	//regExp for column 10, DST
        	regExp = "\"(E|A|S|O|Z|N|U)\"";			// regExp  without escaping: "(E|A|S|O|Z|N|U)"
        	break;

		default:
			regExp = ".*";
			break;
	}
	
	return std::regex_match(teststring, regExp);
}

void readTokensAndLines(char* path) {

	std::string delimiter = ",";
	std::ifstream file;  // File-Handle
	std::string line;


	file.open(path, std::ios::in);

	if (file.is_open()) {
		while (!file.eof() ) {          			// As long as the file isn't empty
			getline(file, line);       				// read the next line
			if (!line.empty()) {

				size_t last = 0;
				size_t next = 0;

				std::vector<std::string> elements;

				while ((next = line.find(delimiter, last)) != std::string::npos) {
					elements.push_back(line.substr(last, next-last));				// write all entries into a vector
					last = next + 1;
				}
				elements.push_back(line.substr(last, next-last));
				std::cout << elements.at(1) << " - " << elements.at(11) << "\n";	// only print out the name and timezone of the airport, that are on position 1 and 11

				if (!isValueCorrect(elements.at(5), 5)) {				// test ICAO
					if (!logFile.is_open()) {
						logFile.open("fileio.log", std::ios::out);
					} else {
						logFile << line << " Wrong ICAO" << "\n";
					}
				}
				if(!isValueCorrect(elements.at(8), 8)) {				// test altitude
					if (!logFile.is_open()) {
						logFile.open("fileio.log", std::ios::out);
					} else {
						logFile << line << " Wrong altitude" << "\n";
					}
				}
				if(!isValueCorrect(elements.at(10), 10)) {				// test DST
					if (!logFile.is_open()) {
						logFile.open("fileio.log", std::ios::out);
					} else {
						logFile << line << " Wrong DST" << "\n";
					}
				}
			}
		}
		file.close();
		logFile.close();
	}
}


int main(int argc, char * argv[]) {

	if(argc != 2) 	{
		std::cout << "not enough arguments - USAGE: fileio [DATASET]" << std::endl;
		return -1;	// invalid number of parameters
	}
	
	std::cout << "Given path to airports.dat: " << argv[1] << std::endl;

	readTokensAndLines(argv[1]);
	return 0;
}
