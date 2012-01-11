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

vector<Node*> parseFile(string fileName) {
	map<string,Node*> nodeMap;
	vector<Node*> graph;
	Node* aNode;

	ifstream input(fileName.c_str());

	if( input.is_open()){
		string line;
		string aDep;
		size_t found;
		while (getline (input, line)){
			if(line.empty())
				continue;
			//Rule
			found = line.find_first_of(":");
			if(found == string::npos)
				exit(EXIT_FAILURE);
			string nodeName = line.substr(0,found);
			map<string, Node*>::iterator it = nodeMap.find(nodeName);
			aNode = it == nodeMap.end() ? new Node(nodeName) : it->second;
			aNode->setRule(true);
			//Dependencies
			istringstream iss(line.substr(found+1));
			while( iss >> aDep){
				aNode->addDependency(nodeMap.insert(pair<string,Node*>(aDep,new Node(aDep))).first->second);
				nodeMap.find(aDep)->second->addResolves(aNode);
			}
			//Copy rule
			getline(input, line);
			aNode->setCommand(line);
			nodeMap.insert(pair<string,Node*>(aNode->getNodeName(),aNode));
		}
		input.close();
	}

	list<Node*> resolves;
	for(map<string,Node*>::iterator it = nodeMap.begin(); it != nodeMap.end(); it++) {
		aNode = it->second;
		resolves = aNode->getResolves();
		//If it's a terminal update
		if(!aNode->isRule()){
			for(list<Node*>::iterator it = resolves.begin(); it != resolves.end(); it++)
				(*it)->addTerminal(aNode->getNodeName());
		}
		graph.push_back(it->second);
	}
	//Check if updated (marks terminals as finished too)
	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if(checkIfDepUpToDate(aNode->getNodeName(), aNode->getTerminals()))
			aNode->setFinished(true);
	}
	return graph;
}

bool checkIfDepUpToDate(string ruleName, list<string> depNames){
	struct stat statRule, statDep;
	double diff;

	if (stat(ruleName.c_str(), &statRule) == -1)
		return false;

	for (list<string>::iterator it = depNames.begin(); it != depNames.end(); it++){
		if(stat(it->c_str(), &statDep) == -1)
			return false;

		diff = difftime(statDep.st_mtime, statRule.st_mtime);
		if( diff > 0)
			return false;
	}
	return true;
}

int main() {
	return 0;
}
