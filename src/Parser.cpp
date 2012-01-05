#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#include "Node.h"

vector<Node>& parseFile(string nameFile) {
	map<string,Node*> nodeMap;

	ifstream file;
  	file.open (nameFile);
  	file.close();
}  
