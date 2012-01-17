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

//For Dependency TimeStamp Checking
#include <sys/stat.h>
#include <time.h>

using namespace std;

bool checkIfDepUpToDate(string ruleName, list<string> depNames);
string cleanWhiteSpaces(string str);
vector<Node*> parseFile(string fileName);

#endif //UTILS_H
