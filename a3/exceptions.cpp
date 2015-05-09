#include <iostream>
#include <fstream>
#include <string>
#include <cfloat>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <assert.h>

using namespace std;

static const string LOG_FILE = "log.txt";
static const char FIELD_DELIMITER = ';';

struct FormatException
{
    int m_actLine;
    string m_actFields;
    FormatException(int line): m_actLine(line), m_actFields("") {}
};

// transforms a string to a date. Throws a logic_error if year is *not* between 2005 and 2015
tm stringToTime(string date) throw(logic_error)
{
    tm t;
    strptime(date.c_str(), "%d.%m.%Y", &t);
    
    if (t.tm_year < 105 || t.tm_year > 115) {
        throw logic_error("Year should be between 2005 and 2015");
    }

    return t;
}

void tokenize(const string str, vector<string> &tokens, const char delimiter)
{
    size_t start = str.find_first_not_of(delimiter);
    size_t end = start;

    while (start != string::npos) {
        end = str.find(delimiter, start);
        tokens.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delimiter, end);
    }
}

void parseLine(string line, int lineNum) throw(FormatException)
{
    if (lineNum <= 0) {
        return;
    }

    const string fieldNames[3] = {"Date", "Temperature", "Rainfall"};
    vector<string> fieldValues;
    FormatException formatException(lineNum);

    tokenize(line, fieldValues, FIELD_DELIMITER);
    assert(fieldValues.size() == 3);

    try {
        stringToTime(fieldValues[0]);
    } catch (logic_error e) {
        formatException.m_actFields += fieldNames[0] + " ";
    }

    try {
        stof(fieldValues[1]);
    } catch (...) {
        formatException.m_actFields += fieldNames[1] + " ";
    }

    try {
        stof(fieldValues[2]);
    } catch (...) {
        formatException.m_actFields += fieldNames[2] + " ";
    }

    if (!formatException.m_actFields.empty()) {
        throw formatException;
    }

    // Higher-order functions approach
    // Any way to circumvent ambiguous overload of stof?
    /*int fieldNum = 0;
    array<function<void(string)>,3> fieldFormatters = {stringToTime, std::stof, std::stof};
    FormatException formatException;
    try {
        stringstream ss(line);
        while (ss.good()) {
            string fieldValue;
            getline(ss, fieldValue, DATA_DELIMITER);
            try {
                fieldFormatters[fieldNum](fieldValue);
            } catch (...) {
                formatException.m_actLine = fieldNum;
                formatException.m_actFields += fieldNames[fieldNum];
            }
            ++fieldNum;
        }
    } catch (stringstream::failure e) {
        std::cerr << "Exception parsing line " << lineNum << endl;
    }*/
}

void writeOutFormatException(const FormatException & e)
{
    try {
        ofstream output(LOG_FILE, ofstream::app);
        if (output.is_open()) {
            output << "Line: " << e.m_actLine << " ";
            output << "Invalid fields: " << e.m_actFields << endl;
            output.close();
        }
    } catch (ofstream::failure e) {
        cerr << "Exception opening/writing/closing file" << endl;
    }
}

void checkData(string path)
{
	int validLines = 0;
    int invalidLines = 0;
    int lineNumber = 0;
    ifstream file;

    try {
        file.open(path);
        string line;
        while (getline(file, line)) {
            try {
                parseLine(line, ++lineNumber);
                ++validLines;
            } catch (FormatException e) {
                writeOutFormatException(e);
                ++invalidLines;
            }
        }
        file.close();
    } catch (ifstream::failure e) {
        cerr << "Exception opening/reading/closing file" << endl;
    }
    cout << "valid lines: " << validLines << " - invalid lines: " << invalidLines << endl;

    if (invalidLines > 0) {
        cout << "invalid lines logged to " + LOG_FILE << endl;
    }
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        cout << "Invalid number of arguments - USAGE: exceptions [DATASET]" << endl;
        return -1;
    }
    
    checkData(argv[1]);

	return 0;
}
