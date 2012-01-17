#include "utils.h"

vector<Node*> parseFile(string fileName) {
	map<string,Node*> nodeMap;
	pair<map<string,Node*>::iterator,bool> retInsert;
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
			string nodeName = cleanWhiteSpaces(line.substr(0,found));
			map<string, Node*>::iterator it = nodeMap.find(nodeName);
			aNode = it == nodeMap.end() ? new Node(nodeName) : it->second;
			aNode->setRule(true);
			//Dependencies
			istringstream iss(line.substr(found+1));
			while( iss >> aDep){
				aDep = cleanWhiteSpaces(aDep);
				retInsert = nodeMap.insert(pair<string,Node*>(aDep,new Node(aDep)));
				aNode->addDependency();
				retInsert.first->second->addResolves(aNode);
			}
			//Copy rule
			getline(input, line);
			aNode->setCommand(line);
			nodeMap.insert(pair<string,Node*>(aNode->getNodeName(),aNode));
		}
		input.close();
	}

	list<Node*> resolves;
	for(map<string,Node*>::iterator itMap = nodeMap.begin(); itMap != nodeMap.end(); itMap++) {
		resolves = itMap->second->getResolves();
		//If it's a terminal update
		if(!itMap->second->isRule()){
			for(list<Node*>::iterator itList = resolves.begin(); itList != resolves.end(); itList++){ 
				(*itList)->addTerminal(itMap->second->getNodeName());
				(*itList)->remDependency();
			}
		}
		graph.push_back(itMap->second);
	}

	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if(checkIfDepUpToDate((*it)->getNodeName(), (*it)->getTerminals()))
			(*it)->setFinished(true);
	}
	return graph;
}

string cleanWhiteSpaces(string str) {
	unsigned int beg, end;
	for(beg = 0; beg < str.length() && str[beg] == ' '; beg++);
	for(end = beg; end < str.length() && str[end] != ' '; end++);
	return str.substr(beg,end-beg);
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


/*
Returns the elements from base that are no present in toCompare.
Elements need to be ordered
*/
std::list<std::string>& diffLists(std::list<std::string>& base, const std::list<std::string>& toCompare){
	int compareResult;
	std::list<std::string>::iterator itBase;
	std::list<std::string>::const_iterator itCompare;

	itBase = base.begin(); itCompare = toCompare.begin();
	while( itBase != base.end() && itCompare != toCompare.end() ){
		compareResult = itBase->compare(*itCompare);
		if(compareResult == 0){
			itBase = base.erase(itBase);
			itCompare++;
		}
		else if ( compareResult < 0)
			itBase++;
		else
			itCompare++;
	}
	return base;
}

std::list<std::string> getFilesInDir(std::string dir){
	using namespace std;
	list<string> files;
	DIR *dp;
	struct dirent *ep;
	
	dp = opendir(dir.c_str());
	if( dp != NULL ){
		while ( (ep = readdir(dp)) )
			if( ep->d_type == DT_REG ) //If its a regular file
				files.push_back(string(ep->d_name));
		closedir( dp );
	}
	return files;
}
