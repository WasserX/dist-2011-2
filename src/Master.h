/*
 * Master.h
 *  Created on: Jan 10, 2012
 *      Author: WasserX
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <vector>
#include <list>
#include <map>
#include <string>
#include "Node.h"


class Master {
public:
	Master(std::vector<Node*> graph, std::vector<int> resources);
	virtual ~Master() {
		for(std::vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++)
			delete *it;
	}

	//Getters and Setters
	inline std::vector<Node*> getGraph() { return graph; };
	inline std::vector<int> getAvailableResources() { return availableResources; };
	inline std::map<int, std::list<std::string> > getFilesInResource() { return filesInResource; };
	inline std::vector<Node*> getReadyToCompute() { return readyToCompute; };
	inline std::map<int, Node*> getComputing() { return computing; };

private:
	void updateReadyToCompute();
	std::pair<Node*, std::list<std::string> >& nextNode(int id);
	//Removes elems that are on both lists from base and returns it. PARAMETERS NEED TO BE SORTED
	std::list<std::string>& diffLists(std::list<std::string>& base, const std::list<std::string>& toCompare);

	const int INF = 99999;

	std::vector<Node*> graph;
	std::vector<int> availableResources;
	std::map<int, std::list<std::string> > filesInResource; //For each resource a list of files that already has
	std::vector<Node*> readyToCompute;
	std::map<int, Node*> computing; //Who is computing and what
};

#endif /* MASTER_H_ */
