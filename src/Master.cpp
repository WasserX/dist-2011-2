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

void Master::updateReadyToCompute(){
	using namespace std;
	for(vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		if((*it)->isReady())
			readyToCompute.push_back(*it);
	}
}
