#include <iostream>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <string>
#include <map>
#include <list>

#if defined _WIN32
#include <direct.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace std;

typedef struct CSVConfig {
	bool fsize;
	bool fnum;
	bool files;
	CSVConfig():fsize(), fnum(), files() { }
} CSVConfig;

typedef struct CategoryInfo {
	string name;
	unsigned int files;
	unsigned int memory;
	string paths;
	CategoryInfo(string n, unsigned int f, unsigned int m, string p) : name(n), files(f), memory(m), paths(p) {}
} CategoryInfo;

typedef struct FileEntry {
	string name;
	string path;
	unsigned int size;
	FileEntry(string n, string p, unsigned int s) : name(n), path(p), size(s) {}
} FileEntry;

typedef std::list<FileEntry*> FileEntryList;
typedef std::map<string,FileEntryList> DirectoryMap;

// Map from extensions to file entries
DirectoryMap dirinfo;

// Map from flattened filenames to number of files
map<string,int> flattenedMap;

const string PARENT_FOLDER = "..";
const string CURRENT_FOLDER = ".";
const string UNDEFINED_EXT = "undefined";
const char UNDERSCORE = '_';
#if defined _WIN32
const char PATH_SEPERATOR = '\\';
#else
const char PATH_SEPERATOR = '/';
#endif

// Beware: this function only works when the directory is empty..
void removeDirectory(string dir)
{
#if defined _WIN32
    _rmdir(dir.data());
#elif defined __GNUC__
    rmdir(dir.data());
#endif
}

void createDirectory(string dir)
{
#if defined _WIN32
    _mkdir(dir.data());
#elif defined __GNUC__
    mkdir(dir.data(), 0777);
#endif
}

string getFileExtension(const string& filename)
{
    const int pos = filename.find_last_of(".");
    if (pos != string::npos) {
        return filename.substr(pos + 1);
    }
    return UNDEFINED_EXT;
}

void renameDuplicateFile(string& filename, const int& version)
{
    if (version < 2) {
        return;
    }
    const int pos = filename.find_last_of(".");
    filename.insert(pos, to_string(version));
}

CategoryInfo* collectEntries(const string& name, const FileEntryList& fileEntries)
{
	unsigned int memory = 0;
	unsigned int files = 0;
	string paths = "[";

	FileEntryList::const_iterator fileEntry;
	for (fileEntry = fileEntries.begin(); fileEntry != fileEntries.end(); ++fileEntry) {
		paths += "\"" + (*fileEntry)->path + PATH_SEPERATOR + (*fileEntry)->name + "\",";
		memory += (*fileEntry)->size;
		files++;
	}

	paths.erase(paths.end()-1);
	paths += "]";

	return new CategoryInfo(name, files, memory, paths);
}

void printCSV(const DirectoryMap& m, const CSVConfig& config, ostream &out) {
    out << "extension";
    if (config.fsize) {
    	out << ",memory";
    }
    if (config.fnum) {
    	out << ",number";
    }
    if (config.files) {
    	out << ",files";
    }
    out << endl;

    // Print individual categories
    DirectoryMap::const_iterator it;
    for (it = m.begin(); it != m.end(); ++it) {

    	string extension = it->first;
    	FileEntryList entryList = it->second;
    	CategoryInfo* categoryInfo = collectEntries(extension, entryList);

    	out << categoryInfo->name;
    	if (config.fsize) {
	    	out << "," << categoryInfo->memory;
	    }
	    if (config.fnum) {
	    	out << "," << categoryInfo->files;
	    }
	    if (config.files) {
	    	out << "," << categoryInfo->paths;
	    }
	    out << endl;
    }
}

void printCSVToFile(const DirectoryMap& m, const CSVConfig& config, string filename) {
    ofstream output (filename);
    if (output.is_open()) {
        printCSV(m, config, output);
        output.close();
        cout << "CSV file created." << endl;
    } else {
        cout << "Unable to open file." << endl;
    }
}

void traverseDirectory(string path, string flattenDir = "")
{
	DIR *dir;
	struct dirent *ent;

	dir = opendir(path.c_str());

	if (dir != nullptr) {
		while ((ent = readdir(dir)) != nullptr) {
			string name = ent->d_name;
			string fullpath = path + PATH_SEPERATOR + name;

			if (name == CURRENT_FOLDER || name == PARENT_FOLDER) {
				// Stay within folder hierarchy
				continue;
			}

			if (ent->d_type == DT_DIR) {
				traverseDirectory(fullpath, flattenDir);
			} else {
				// Collect data
				string ext = getFileExtension(name);
				struct stat fileinfo;
				unsigned int size = 0;
				if (!stat(fullpath.c_str(), &fileinfo))
				{
					size = fileinfo.st_size;
				}

				// Create new list of entries if necessary
				FileEntryList fileEntries;
				FileEntry *entry = new FileEntry(name, path, size);
				DirectoryMap::iterator it = dirinfo.find(ext);

				if (it != dirinfo.end())
				{
					fileEntries = it->second;
				}
				fileEntries.push_back(entry);

				// Map list to file extension
				dirinfo[ext] = fileEntries;

                // Flatten hierarchy
                if (!flattenDir.empty()) {
                    string flattenedName = fullpath;
                    std::replace(flattenedName.begin(), flattenedName.end(), PATH_SEPERATOR, UNDERSCORE);

                    map<string,int>::iterator iter = flattenedMap.find(flattenedName);
                    int numFiles = 0;

                    if (iter != flattenedMap.end())
                    {
                        numFiles += iter->second;
                    }
                    flattenedMap[flattenedName] = ++numFiles;

                    if (numFiles > 1) {
                        renameDuplicateFile(flattenedName, numFiles);
                    }

                    // Copy file
                    std::ifstream infile(fullpath, std::ios_base::binary);
                    std::ofstream outfile(flattenDir + PATH_SEPERATOR + flattenedName, std::ios_base::binary);
                    outfile << infile.rdbuf();
                }
			}
		}
		closedir(dir);
	} else {
		cout << "Failed to read directory: " << path << endl;
	}
}

void printHelp()
{
	cout << "invalid arguments - USAGE: dirinfo [DIR] -o [OUTPUT.csv] -flat [DIR] FLAGS" << endl;
	cout << "possible FLAGS are:" << endl;
	cout << "-fsize\t\t\tSummarized file size for files with same extension will be exported" << endl;
    cout << "-fnum\t\t\tSummarized number of files with same extension will be exported" << endl;
	cout << "-files\t\t\tRelative paths of files with same extension will be exported" << endl;
}

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printHelp();
		return -1;	// invalid number of parameters
	}

	CSVConfig config;
	string directory, outputFile, flattenDir;
	bool hasOutputFile = false;
    bool hasFlattenDir = false;

	for (int i = 1; i < argc; i++) {
		string arg = string(argv[i]);

		if (arg == "-fsize") {
			config.fsize = true;
		} else if (arg == "-fnum") {
			config.fnum = true;
		} else if (arg == "-files") {
			config.files = true;
		} else if (arg == "-o") {
			hasOutputFile = true;
		} else if (arg == "-flat") {
            hasFlattenDir = true;
        } else {
			// First non-flag argument should be directory
			if (!hasOutputFile && !hasFlattenDir && directory.empty()) {
				directory = arg;
			} else if (hasOutputFile && outputFile.empty()) {
				outputFile = arg;
			} else if (hasFlattenDir && flattenDir.empty()) {
                flattenDir = arg;
            } else {
                printHelp();
                return -1;
            }
		}
	}

    // Ensure that directories are set up correctly
	if (directory.empty() || (hasOutputFile && outputFile.empty())
            || (hasFlattenDir && flattenDir.empty())) {
        cout << directory << outputFile << flattenDir;
		printHelp();
		return -1;
	}

    if (hasFlattenDir) {
        // Ensure that directory is available for flattening
        DIR *dir;
        dir = opendir(flattenDir.c_str());
        if (dir == nullptr) {
            createDirectory(flattenDir);
        }
        traverseDirectory(directory, flattenDir);
    } else {
        traverseDirectory(directory);
    }

	if (hasOutputFile) {
		printCSVToFile(dirinfo, config, outputFile);
	} else {
		printCSV(dirinfo, config, cout);
	}

	return 0;
}
