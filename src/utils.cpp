#include "utils.h"

list<Node*> parseFile(string fileName, string startingRule) {
	map<string,Node*> nodeMap;
	pair<map<string,Node*>::iterator,bool> retInsert;
	list<Node*> graph;
	Node* aNode;

	ifstream input(fileName.c_str());

	if( input.is_open()){
		string line;
		string aDep;
		size_t found;
		while (getline (input, line)){
			line = trim(line);
      if(line.empty() || line[0] == '#')
				continue;
			//Rule
			found = line.find_first_of(":");
			if(found == string::npos){
				cout << "Error parsing Makefile" << endl;
				return graph;
			}
			string nodeName = line.substr(0,found);
			nodeName = trim(nodeName);
			if (startingRule.empty()) startingRule = nodeName;
      map<string, Node*>::iterator it = nodeMap.find(nodeName);
			aNode = it == nodeMap.end() ? new Node(nodeName) : it->second;				
			aNode->setRule(true);
			//Dependencies
			istringstream iss(line.substr(found+1));
			while( iss >> aDep){
				aDep = trim(aDep);
				retInsert = nodeMap.insert(pair<string,Node*>(aDep,new Node(aDep)));
				aNode->addDependency();
				aNode->addNeeds(retInsert.first->second);
				retInsert.first->second->addResolves(aNode);
			}
			//Command
			getline(input, line);
			aNode->setCommand(trim(line));
			nodeMap.insert(pair<string,Node*>(aNode->getNodeName(),aNode));
		}
		input.close();
	}

	map<string, Node*>::iterator itNodeMap = nodeMap.find(startingRule);
	if ( itNodeMap == nodeMap.end() ){
		cout << "Error finding rule passed" << endl;
		return graph;
	}
	list<Node*> toAnalize;
	Node* n;
	toAnalize.push_back(itNodeMap->second);
	nodeMap.clear();
  while(!toAnalize.empty()){
		n = toAnalize.front(); toAnalize.pop_front();
    nodeMap.insert(pair<string, Node*>(n->getNodeName(), n));
    toAnalize.splice(toAnalize.begin(), n->getNeeds());
	}

	list<Node*> resolves;
  for(map<string,Node*>::iterator it = nodeMap.begin(); it != nodeMap.end(); it++)
		if( !it->second->isRule())
      toAnalize.push_back(it->second);
  
  while( !toAnalize.empty() ){
    n = toAnalize.front(); toAnalize.pop_front();
    n->setFinished();
    for(list<Node*>::const_iterator it = n->getResolves().begin(); it != n->getResolves().end(); it++){ 
      (*it)->addTerminal(n->getNodeName());
			(*it)->remDependency();
      if((*it)->isReady() && !(*it)->isFinished() && checkIfDepUpToDate((*it)->getNodeName(), (*it)->getTerminals()))
        toAnalize.push_back(*it);
   }    
  }

  for(map<string,Node*>::iterator it = nodeMap.begin(); it != nodeMap.end(); it++)	
	  graph.push_back(it->second);
  return graph;
}

string& trim(string& str) {
	str.erase(str.find_last_not_of(" \n\r\t")+1);
	return str.erase(0,str.find_first_not_of(" \n\r\t"));
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

pair<char*, unsigned long> readFile(std::string fileName) {
	using namespace std;
	FILE *file;
	char *content;
	unsigned long fileLen;

	//Open file
	file = fopen(fileName.c_str(), "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);
	//Allocate memory
	content=(char *)malloc(fileLen);
	if (!content)
	{
		fprintf(stderr, "Memory error!");
    	fclose(file);
		exit(EXIT_FAILURE);
	}

	//Read file contents into buffer
	fread(content, sizeof(char), fileLen, file);
	fclose(file);
	return pair<char*, unsigned long>(content, fileLen);
}

void writeFile(char* buffer, int size, std::string fileName) {
	FILE* fp;
	fp = fopen(fileName.c_str(), "w+b");
	fwrite(buffer, sizeof(char), size, fp);
	fclose(fp);
}

std::string getFormattedLS(){
	string commandLS = "ls -l | awk \'{print $";
  commandLS.append(colNameLS).append("\" \"$5}\'");
  FILE* pipe = popen(commandLS.c_str(), "r");
	char buffer[Master::FILE_NAME_SIZE];
	stringstream ss;	
	while(!feof(pipe)) {
		if(fgets(buffer, Master::FILE_NAME_SIZE, pipe) != NULL)
			ss << buffer;
	}	
	pclose(pipe);
  return ss.str(); 
}

char* getFilesAndSizes(const std::list<std::string>& fileNames){
	using namespace std;
	char* files = (char*) malloc(Master::FILE_NAME_SIZE);
	files[0] = '\0';
	if(fileNames.empty())
		return files;	

  stringstream ss;
  ss << getFormattedLS();
  string output;
	string filesToSend;
  list<string>::const_iterator it = fileNames.begin();
  while( ss >> output && it != fileNames.end()){
    if(!output.compare(*it)){
			filesToSend.append(" ").append(output + " ");
			ss >> output;
			filesToSend += output;
      it++;
    }else{
      ss >> output;
    }
	}
	return strcpy(files, filesToSend.c_str());
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
