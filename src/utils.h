#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <list>
#include "Node.h"
#include "Master.h"

//For Dependency TimeStamp Checking
#include <sys/stat.h>
#include <time.h>

//For Directory checking
#include <dirent.h>

//Dependant of target system
#ifdef UBUNTU
const static char colNameLS[] = "8";
const static char colDataNameLS[] = "7";
const static char colDataDateLS[] = "6";
#elif defined REDHAT
const static char colNameLS[] = "9";
#endif

using namespace std;

pair<char*, unsigned long> readFile(std::string fileName);
void writeFile(char* buffer, int size, std::string fileName);
bool checkIfDepUpToDate(string ruleName, list<string> depNames);
string& trim(string& str);
list<Node*> parseFile(string fileName, string startingRule);
std::string getFormattedLS();
char* getFilesAndSizes(const std::list<std::string>& fileNames);
std::list<std::string>& diffLists(std::list<std::string>& base, const std::list<std::string>& toCompare);

#endif //UTILS_H
