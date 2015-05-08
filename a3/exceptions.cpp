#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cfloat>
#include <iomanip>
#include <vector>

// transforms a string to a date. Throws a logic_error if year is *not* between 2005 and 2015
std::tm stringToTime(std::string date)
{
    std::tm t;
    std::istringstream ss(date);
    ss >> std::get_time(&t, "%d.%m.%Y");
    
    if(t.tm_year < 105 || t.tm_year > 115)
        throw std::logic_error("Year should be between 2005 and 2015");
    
    return t;
}

struct FormatException
{
    int m_actLine;
    std::string m_actFields;
};

void parseLine(std::string line, int lineNum)
{
    const std::string fieldNames[3] = { "Date", "Temperature", "Rainfall" };
    
    // todo 3.2b: parse a given line, check dates by calling stringToTime, check temperature/rainfall by calling std::stof. Catch all exceptions thrown by these methods. If there have been any exceptions, aggregate all necessary information into an instance of FormatException and throw that instance.
    
}

// todo 3.2c..
void writeOutFormatException(const FormatException & e)
{
    // todo 3.2d: export information (i.e., line number + invalid data fields) about exception to a logfile.
    // todo 3.2d: catch ios_base::failure
}

void checkData(std::string path)
{
	int validLines = 0;
    int invalidLines = 0;
    std::ifstream file;
    
    // todo 3.2a: open file + read each line + call parseLine function (catch ifstream::failure)
    // todo 3.2c: read each line + call parseLine function (catch FormatException) + count valid + invalid lines
    
    std::cout << "valid lines: " << validLines << " - invalid lines: " << invalidLines << std::endl;
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cout << "Invalid number of arguments - USAGE: exceptions [DATASET]" << std::endl;
        return -1;
    }
    
    checkData(argv[1]);

	return 0;
}
