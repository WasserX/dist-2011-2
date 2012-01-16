/*
 * Master.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: wasser
 */

#include "Master.h"

Master::Master(std::vector<Node*> graph, std::vector<int> resources){
	using namespace std;
	this->graph = graph;
	availableResources = resources;
	for(vector<int>::iterator it = availableResources.begin(); it != availableResources.end(); it++)
		filesInResource.insert(pair<int, list<string> >(*it, list<string>()));

	updateReadyToCompute();
}

void Master::execute(){
	using namespace std;
	bool finish = false;
	while(!finish){
		if ( !readyToCompute.empty() && computing.size() != availableResources.size() )
			;//sendTask();
		else if ( !computing.empty() )
			;//receiveTaskS();
		else if ( readyToCompute.empty() && computing.empty() )
			finish = true;
	}
}

void Master::updateReadyToCompute(){
	using namespace std;
	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if((*it)->isReady() && !(*it)->isFinished())
			readyToCompute.push_back(*it);
	}
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
