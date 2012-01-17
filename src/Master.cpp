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
	} while( !readyToCompute.empty() && !computing.empty() );
}

void Master::updateReadyToCompute(){
	using namespace std;
	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if((*it)->isReady() && !(*it)->isFinished())
			readyToCompute.push_back(*it);
	}
}

void Master::sendTask(std::pair<Node*, std::list<std::string> > input, int target) {
	using namespace std;

	char command[COMMAND_SIZE];
	strcpy(command, input.first->getCommand().c_str());
	std::list<std::string> fileNames = input.second;	
	//Command
	MPI_Send(command, COMMAND_SIZE, MPI_BYTE, target, COMMAND_TAG, MPI_COMM_WORLD);
	//File_names
	string allNames = "";
	for(std::list<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); it++){
		allNames.append(*it + " ");
	}
	char allFileNames[FILE_NAME_SIZE];
	strcpy(allFileNames, allNames.substr(0, allNames.size()-1).c_str());
	MPI_Send(allFileNames, FILE_NAME_SIZE, MPI_BYTE, target, FILE_NAME_TAG, MPI_COMM_WORLD);
	//Add to computing
	computing.insert(std::pair<int, Node*>(target, input.first));
	//Remove from available
	availableResources.pop_front();
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
			cout << "I'm " << ptRequest->first << " Finished doing my tasks, these are the files I created: " 
			<< rcvBuffers.find(ptRequest->first)->second << endl;
			//Should get the files
			
			//Updating Graph
			istringstream iss(rcvBuffers.find(ptRequest->first)->second);
			string file;
			list<string> files;
			while( iss >> file)
				files.push_back(cleanWhiteSpaces(file));
			
			list<Node*> depended = mapIt->second->getResolves();
			for(list<Node*>::iterator it = depended.begin(); it != depended.end(); it++)
				(*it)->remDependency();
			
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

std::pair<Node*, std::list<std::string> > Master::nextNode(int id){
	using namespace std;
	list<string> inProcess = filesInResource.find(id)->second;
	inProcess.sort();
	list<string> other;

	unsigned int bestDistance = INF;
	Node* selected;
	for(vector<Node*>::iterator it = readyToCompute.begin(); it != readyToCompute.end(); it++)
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
