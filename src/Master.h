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
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <string.h>
#include "Node.h"
#include "utils.h"


class Master {
public:
	Master(std::vector<Node*> graph, std::list<int> resources);
	virtual ~Master() {
		for(std::vector<Node*>::iterator it = graph.begin(); it != graph.end(); it++)
			delete *it;
	}
	void execute();
	
	//Constants
	const static int ID = 0;
	const static int FINISH_SIZE = 1;
	const static int COMMAND_SIZE = 1000;
	const static int FILE_NAME_SIZE = 1000;
	const static int FILE_SIZE = 10000;
	const static int COMMAND_TAG = 100;
	const static int FILE_NAME_TAG = 101;
	const static int FILE_SEND_TAG = 102;
	const static int FINISH_TAG = 103;
	const static int RESPONSE_FILE_LIST_TAG = 200;

private:
	void updateReadyToCompute();
	std::string getExecutableFromCommand(const std::string command);
	char* getFormattedFilesToSend(const std::string& command, const std::list<std::string>& terminals);
	void sendTask(std::pair<Node*, std::list<std::string> > input, int target);
	void receiveFinished();
	std::pair<Node*, std::list<std::string> > nextNode(int id);

	const static int INF = 99999;

	std::vector<Node*> graph;
	std::list<int> resources;
	std::map<int, std::list<std::string> > filesInResource; //For each resource a list of files that already has
	std::list<Node*> readyToCompute;
	std::list<int> availableResources;
	std::map<int, Node*> computing; //Who is computing and what
	std::map<int, MPI_Request*> requests;
	std::map<int, char*> rcvBuffers;
};

#endif /* MASTER_H_ */
