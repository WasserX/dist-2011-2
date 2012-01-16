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
	totalResources = resources.size();
	for(list<int>::iterator it = availableResources.begin(); it != availableResources.end(); it++)
		filesInResource.insert(pair<int, list<string> >(*it, list<string>()));

	updateReadyToCompute();
}

void Master::execute(){
	using namespace std;
	
	do {
		if ( !readyToCompute.empty() && computing.size() != totalResources ) {
			sendTask(nextNode(availableResources.front()), availableResources.front());
		}
		else if ( !computing.empty() )
				;//receiveTaskS();
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
	char command[COMMAND_SIZE];
	strcpy(command, input.first->getCommand().c_str());
	std::list<std::string> fileNames = input.second;	
	//Command
	MPI_Send(command, COMMAND_SIZE, MPI_BYTE, target, COMMAND_TAG, MPI_COMM_WORLD);
	//File Quantity
	int fileQuantity[FILE_QUANT_SIZE];
	fileQuantity[0] = fileNames.size();
	MPI_Send(fileQuantity, FILE_QUANT_SIZE, MPI_INT, target, FILE_QUANT_TAG, MPI_COMM_WORLD);
	//File_names
	char fileName[FILE_NAME_SIZE];
	for(std::list<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); it++){
		strcpy(fileName, it->c_str());
		MPI_Send(fileName, FILE_NAME_SIZE, MPI_BYTE, target, FILE_NAME_TAG, MPI_COMM_WORLD);
	}
	//Add to computing
	computing.insert(std::pair<int, Node*>(target, input.first));
	//Remove from available
	availableResources.pop_front();
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

std::list<std::string>& Master::diffLists(std::list<std::string>& base, const std::list<std::string>& toCompare){
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
