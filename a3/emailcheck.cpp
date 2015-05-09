#include <iostream>
#include <string>
#include <algorithm>

#define ENTRIES 7

using namespace std;

wstring dataset[ENTRIES][4] = { { L"max.muestermann@bmw.de", L"Max", L"Mustermann", L"SAP" },
    { L"juergen.doellner@hpi.de", L"Jürgen", L"Döllner", L"HPI" },
    { L"soeren.discher@hpi.de", L"Sören", L"Discher", L"HPI" },
    { L"daniel.maeller@hpi.de", L"Daniel", L"Mäller", L"HPI" },
    { L"paul.deissler@hertha.de", L"Sebastian", L"Deißler", L"herTha" },
	{ L"mueller.marga@sap.com", L"Marga", L"Müller", L"SAP" },
    { L"h.boss@service.bayer.com", L"Hugo", L"Boss", L"Bayer" }};

// Replaces [from] with [to] in [str]
void ReplaceAll(wstring& str, const wstring& from, const wstring& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void tokenize(const wstring &str, vector<wstring> &tokens, const char delimiter)
{
    size_t start = str.find_first_not_of(delimiter);
    size_t end = start;

    while (start != string::npos) {
        end = str.find(delimiter, start);
        tokens.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delimiter, end);
    }
}

void caseConvert(wstring& str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void replace(wstring& str) {
    ReplaceAll(str, L"ä", L"ae");
    ReplaceAll(str, L"ö", L"oe");
    ReplaceAll(str, L"ü", L"ue");
    ReplaceAll(str, L"ß", L"ss");
}

bool validateLocal(const wstring &local, const wstring &firstname, const wstring &name)
{
    vector<wstring> tokens;
    tokenize(local, tokens, '.');

    // assume that local part has at least one dot
    if (tokens.size() < 2) {
        return false;
    }

    bool firstnameMatch = false;
    bool nameMatch = false;

    for (auto token : tokens) {
        // full match for last name
        if (token == name) {
            nameMatch = true;
            continue;
        }
        // match initials at least for first name
        if (token.find(firstname[0]) != string::npos) {
            firstnameMatch = true;
        }
    }

    return nameMatch && firstnameMatch;
}

bool validateDomain(const wstring &domain, const wstring &company)
{
    vector<wstring> tokens;
    tokenize(domain, tokens, '.');

    if (tokens.size() < 2) {
        return false;
    }

    // last part before tld should be company name
    return !(tokens.end()[-2] != company);
}

bool emailCheck(wstring mail, wstring firstname, wstring name, wstring company)
{
    vector<wstring> parts;
    tokenize(mail, parts, '@');
    wstring &local = parts[0];
    wstring &domain = parts[1];

    if (local.empty() || domain.empty()) {
        return false;
    }

    bool validLocal = validateLocal(local, firstname, name);
    bool validDomain = validateDomain(domain, company);

    return validLocal && validDomain;
}

int main(int argc, char * argv[])
{
    for (int i = 0; i < ENTRIES; i++)
    {
        for (int j = 1; j < 4; j++)
        {
            replace(dataset[i][j]);
            caseConvert(dataset[i][j]);
        }
        
        cout << emailCheck(dataset[i][0], dataset[i][1], dataset[i][2], dataset[i][3]) << endl;
    }
    
    return 0;
}