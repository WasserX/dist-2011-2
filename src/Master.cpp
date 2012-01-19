/*
 * Master.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: wasser
 */

#include "Master.h"

Master::Master(std::vector<Node*> graph, std::list<int> resources){
	using namespace std;
	this->graph = graph;
	availableResources = resources;
	this->resources = resources;
	for(list<int>::iterator it = availableResources.begin(); it != availableResources.end(); it++)
		filesInResource.insert(pair<int, list<string> >(*it, list<string>()));

	updateReadyToCompute();
}

void Master::execute(){
	using namespace std;
	do {
		if ( !readyToCompute.empty() && !availableResources.empty() ) {
			sendTask(nextNode(availableResources.front()), availableResources.front());
		}
		else if ( !computing.empty() )
			receiveFinished();
	} while( !readyToCompute.empty() || !computing.empty() );

	/*for(list<Node*>::iterator it = readyToCompute.begin(); it != readyToCompute.end(); it++) {
		(*it)->toString();
	}
	cout << "********************************************" << endl;
	for(map<int, Node*>::iterator it = computing.begin(); it != computing.end(); it++) {
		it->second->toString();
	}*/

	int finish = 1;
	for(list<int>::iterator it = resources.begin(); it != resources.end(); it++)
		MPI_Send(&finish, FINISH_SIZE, MPI_INT, *it, FINISH_TAG, MPI_COMM_WORLD);
}

void Master::updateReadyToCompute(){
	using namespace std;
	readyToCompute.clear();
	
	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if((*it)->isReady() && !(*it)->isFinished() && !(*it)->isComputing())
			readyToCompute.push_back(*it);
	}
}

void Master::sendTask(std::pair<Node*, std::list<std::string> > input, int target) {
	using namespace std;

	//Command
	char command[COMMAND_SIZE];
	strcpy(command, input.first->getCommand().c_str());
	MPI_Send(command, COMMAND_SIZE, MPI_BYTE, target, COMMAND_TAG, MPI_COMM_WORLD);

	//Terminals: Structure: "(0|1 executableFile) (fileName)*"
	//The executable file will have his permissions changed
	char *files = getFormattedFilesToSend(command, input.second);
	MPI_Send(files, FILE_NAME_SIZE, MPI_BYTE, target, FILE_NAME_TAG, MPI_COMM_WORLD);
	cout << "I am sending task to " << target << " with command: " << command << endl;
	//Add to computing
	computing.insert(std::pair<int, Node*>(target, input.first));
	//Remove from available
	availableResources.pop_front();
	readyToCompute.remove(input.first);
	input.first->setComputing();
	
	//Waits first part of answer async
	map<int, MPI_Request*>::iterator ptRequests = requests.find(target);
	if( ptRequests == requests.end() )
		ptRequests = requests.insert(pair<int, MPI_Request*>(target, new MPI_Request())).first;
	
	map<int, char*>::iterator ptBuffers = rcvBuffers.find(target);
	if( ptBuffers == rcvBuffers.end() )
		ptBuffers = rcvBuffers.insert(pair<int, char*>(target,(char*) malloc (FILE_NAME_SIZE))).first;
	MPI_Irecv(ptBuffers->second, FILE_NAME_SIZE, MPI_BYTE, target, 
		RESPONSE_FILE_LIST_TAG, MPI_COMM_WORLD, ptRequests->second);
}

void Master::receiveFinished() {
	using namespace std;

	map<int, MPI_Request*>::iterator ptRequest;
	MPI_Status status;
	int flag = 0;
	bool once = false;
		
	for(map<int, Node*>::iterator mapIt = computing.begin(); mapIt != computing.end(); mapIt++){
		flag = false;
		ptRequest = requests.find(mapIt->first);
		if( ptRequest != requests.end() )
			MPI_Test(ptRequest->second, &flag, &status);
		if(flag){
			//Should get the files
			
			//Updating Graph
			mapIt->second->setFinished();
			istringstream iss(rcvBuffers.find(ptRequest->first)->second);
			string file;
			list<string> files;
			while( iss >> file)
				files.push_back(cleanWhiteSpaces(file));
			
			list<Node*> depended = mapIt->second->getResolves();
			list<string>::iterator termBeginIt;
			for(list<Node*>::iterator it = depended.begin(); it != depended.end(); it++){
				(*it)->remDependency();
				for(list<string>::iterator itFile = files.begin(); itFile != files.end(); itFile++)
					(*it)->addTerminal(*itFile);
			}
			
			//Updating the lists it belongs
			computing.erase(ptRequest->first);
			availableResources.push_back(ptRequest->first);
			
			once = true;
			flag= 0;
		} 
	}
	if(once)
		updateReadyToCompute();
}

char* Master::getFormattedFilesToSend(const std::string& command, const std::list<std::string>& terminals){
	string filesToSend = getExecutableFromCommand(command);
	if (!filesToSend.empty())
		filesToSend.insert(0,"1 ");
	else
		filesToSend.insert(0,"0 ");
			
	for(list<string>::const_iterator it = terminals.begin(); it != terminals.end(); it++){
		filesToSend.append(*it + " ");
	}
	char files[FILE_NAME_SIZE];
	return strcpy(files, filesToSend.substr(0, filesToSend.size()-1).c_str());
}

std::string Master::getExecutableFromCommand(const std::string command){
	struct stat statFile;
	
	std::istringstream ss(command);
	std::string exec;
	ss >> exec;
	if (stat(exec.c_str(), &statFile) == -1)
		return "";
	else
		return exec;	
}

std::pair<Node*, std::list<std::string> > Master::nextNode(int id){
	using namespace std;
	list<string> inProcess = filesInResource.find(id)->second;
	inProcess.sort();
	list<string> other;

	unsigned int bestDistance = INF;
	Node* selected;
	for(list<Node*>::iterator it = readyToCompute.begin(); it != readyToCompute.end(); it++)
	{

		if ((*it)->isFinished()) //This eliminates the terminals that are always ready
			continue;

		other = (*it)->getTerminals();
		other.sort();
		other = diffLists(other, inProcess);

		if( other.size() < bestDistance ) {
			selected = *it;
			bestDistance = other.size();
		}
	}

	other = selected->getTerminals();
	other.sort();
	pair<Node*, list<string> > toReturn(selected, diffLists(other, inProcess));
	return toReturn;
}
