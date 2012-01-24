/*
 * Master.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: wasser
 */

#include "Master.h"

Master::Master(std::list<Node*> graph, std::list<int> resources){
	using namespace std;
	this->graph = graph;
	
	availableResources = resources;
	this->resources = resources;
	for(list<int>::iterator it = availableResources.begin(); it != availableResources.end(); it++){
		filesInResource.insert(pair<int, list<string>* >(*it, new list<string>()));
		requests.insert(pair<int, MPI_Request*>(*it, new MPI_Request()));
		rcvBuffers.insert(pair<int, char*>(*it, (char*) malloc (FILE_NAME_SIZE)));
	}

  #if defined COUNT_MESSAGES || defined COUNT_FILE_MESSAGES
  messageCounter = 0;
  #endif
	updateReadyToCompute();
}

void Master::execute(){
	using namespace std;
	
	do {
		if ( !readyToCompute.empty() && !availableResources.empty() ) {
			//If it's the last rule (all, or something) execute locally
			if(readyToCompute.size() == 1 && readyToCompute.front()->getResolves().empty()){
        #ifdef DEBUG
        cout << "Master executing task " << readyToCompute.front()->getNodeName() << endl;
        #endif
        system(readyToCompute.front()->getCommand().c_str());
				readyToCompute.front()->setFinished();
				readyToCompute.pop_front();
			}
			else		
				sendTask(nextNode(availableResources.front()), availableResources.front());
		}
		else if ( !computing.empty() )
			receiveFinished();
	} while( !readyToCompute.empty() || !computing.empty() );

	int finish = 1;
	for(list<int>::iterator it = resources.begin(); it != resources.end(); it++){
		MPI_Send(&finish, FINISH_SIZE, MPI_INT, *it, FINISH_TAG, MPI_COMM_WORLD);
    #ifdef COUNT_MESSAGES
    messageCounter++;
    #endif
  }
  #ifdef DEBUG
  cout << "Master Finished" <<endl;
  #endif

  #if defined COUNT_MESSAGES || defined COUNT_FILE_MESSAGES
  cout << "Total Messages Sent: " << messageCounter << endl;
  #endif
}

void Master::updateReadyToCompute(Node* changed){
	using namespace std;

  if(changed){
    readyToCompute.remove(changed);
    for(list<Node*>::const_iterator it = changed->getResolves().begin(); it != changed->getResolves().end(); it++)
      if((*it)->isReady() && !(*it)->isFinished())
        readyToCompute.push_back(*it);
    readyToCompute.remove(changed);
  }else{
    readyToCompute.clear();
	  for(list<Node*>::iterator it = graph.begin(); it != graph.end(); it++){
		  if((*it)->isReady() && !(*it)->isFinished() && !(*it)->isComputing())
			  readyToCompute.push_back(*it);
	  }
  }
}

void Master::sendTask(std::pair<Node*, std::list<std::string> > input, int target) {
	using namespace std;

	//Command
	char command[COMMAND_SIZE];
	strcpy(command, input.first->getCommand().c_str());
	MPI_Send(command, COMMAND_SIZE, MPI_BYTE, target, COMMAND_TAG, MPI_COMM_WORLD);
  #ifdef COUNT_MESSAGES
  messageCounter++;
  #endif

	//File List
  input.second.sort();
	char *files = getFormattedFilesToSend(target, command, input.second);
	MPI_Send(files, FILE_NAME_SIZE, MPI_BYTE, target, FILE_NAME_TAG, MPI_COMM_WORLD);
	#ifdef COUNT_MESSAGES
  messageCounter++;
  #endif

	//Send Files
	pair<char*, unsigned long> fileContent;
	string fileName, execName;
	int size, sendExec = 0;
	istringstream iss(files);
	
	iss >> sendExec;
	if(sendExec){
		iss >> fileName >> size;
    execName = fileName;
		fileContent = readFile(fileName);
		MPI_Send(fileContent.first, fileContent.second, MPI_BYTE, target, FILE_SEND_TAG, MPI_COMM_WORLD);
	  #if defined COUNT_MESSAGES || defined COUNT_FILE_MESSAGES
    messageCounter++;
    #endif
  }
	while(iss >> fileName){
		iss >> size;
		fileContent = readFile(fileName);
		MPI_Send(fileContent.first, fileContent.second, MPI_BYTE, target, FILE_SEND_TAG, MPI_COMM_WORLD);
		#if defined COUNT_MESSAGES || defined COUNT_FILE_MESSAGES
    messageCounter++;
    #endif
  }	

	//Mark files as available in resource
	//input.second will be DESTROYED
	list<string>* fRes = filesInResource.find(target)->second;
	fRes->splice(fRes->begin(), input.second);
  if ( sendExec ) fRes->push_back(execName);
	//Add to computing
	computing.insert(std::pair<int, Node*>(target, input.first));
	input.first->setComputing();
	//Remove from available
	availableResources.pop_front();
	readyToCompute.remove(input.first);
	
	//Waits first part of answer async
	MPI_Irecv(rcvBuffers.find(target)->second, FILE_NAME_SIZE, MPI_BYTE, target, 
		SLAVE_FILE_NAME_TAG, MPI_COMM_WORLD, requests.find(target)->second);
 #ifdef COUNT_MESSAGES
  messageCounter++;
  #endif
}

void Master::receiveFinished() {
	using namespace std;

	map<int, MPI_Request*>::iterator ptRequest;
	int flag = 0;
		
	for(map<int, Node*>::iterator mapIt = computing.begin(); mapIt != computing.end(); mapIt++){
		flag = false;
		ptRequest = requests.find(mapIt->first);
		if( ptRequest != requests.end() )
			MPI_Test(ptRequest->second, &flag, MPI_STATUS_IGNORE);
		if(flag){
			
			//Getting Files
			string fileName;
			int size;
			char* buffer;
			list<string> fileNames;
			istringstream iss(rcvBuffers.find(mapIt->first)->second);
			while(iss >> fileName){
				fileNames.push_back(fileName);
        iss >> size;
				buffer = (char *)malloc(size);
				MPI_Recv(buffer, size, MPI_BYTE, mapIt->first, FILE_SEND_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				writeFile(buffer, size, fileName);
				#if defined COUNT_MESSAGES || defined COUNT_FILE_MESSAGES
    messageCounter++;
    #endif
			}
			
			//Updating Graph
			mapIt->second->setFinished();
			
			list<Node*> depended = mapIt->second->getResolves();
			list<string>::iterator termBeginIt;
			for(list<Node*>::iterator it = depended.begin(); it != depended.end(); it++){
				(*it)->remDependency();
				for(list<string>::iterator itFile = fileNames.begin(); itFile != fileNames.end(); itFile++)
					(*it)->addTerminal(*itFile);
			}
			
			//Updating the lists it belongs
			computing.erase(ptRequest->first);
			availableResources.push_back(ptRequest->first);
			
			//Updating files it contains
			list<string>* res = filesInResource.find(mapIt->first)->second;
			res->splice(res->begin(),fileNames);

			flag= 0;
      updateReadyToCompute(mapIt->second);
		} 
	}
}

//Terminals: Structure: "(0|1 executableFile) (fileName)*"
//The executable file will have his permissions changed
char* Master::getFormattedFilesToSend(int target, const std::string& command, const std::list<std::string>& terminals){
	string exec = getExecutableFromCommand(command);	
	//If executable already in resource, do not send it again	
	if( !exec.empty() ) {
		list<string>* inRes = filesInResource.find(target)->second;
		for(list<string>::const_iterator it = inRes->begin(); it != inRes->end(); it++)
			if( (*it) == exec ){
				exec = "";
				break;
			}
	}

	string filesToSend(getFilesAndSizes(terminals));
	if( !exec.empty()){
		list<string> execList;
    execList.push_back(exec);
    exec.assign(getFilesAndSizes(execList)).append(" ");
		exec.insert(0, "1 ");
	}
	else if (!filesToSend.empty())
		exec.insert(0, "0 ");
	else
		exec.insert(0, "0");
	
	filesToSend.insert(0,exec);

	char files[FILE_NAME_SIZE];
	return strcpy(files, filesToSend.c_str());
}

std::string Master::getExecutableFromCommand(const std::string command){
	struct stat statFile;
	
	std::istringstream ss(command);
	std::string exec;
	ss >> exec;

  trim(exec).erase(0,exec.find_first_not_of("./"));

	if (stat(exec.c_str(), &statFile) == -1)
		return "";
	else
		return exec;	
}

std::pair<Node*, std::list<std::string> > Master::nextNode(int id){
	using namespace std;
	list<string>* inProcess = filesInResource.find(id)->second;
	inProcess->sort();
	list<string> other;
  
  #ifdef NO_HEURISTICS
		other = readyToCompute.front()->getTerminals();
		other.sort();
		other = diffLists(other, *inProcess);
    return pair<Node*, list<string> >(readyToCompute.front(), other);
  #endif

	unsigned int bestDistance = INF;
	Node* selected;
	for(list<Node*>::iterator it = readyToCompute.begin(); it != readyToCompute.end(); it++)
	{
		other = (*it)->getTerminals();
		other.sort();
		other = diffLists(other, *inProcess);

		if( other.size() < bestDistance ) {
			selected = *it;
			bestDistance = other.size();
		}
	}

	other = selected->getTerminals();
	other.sort();
	pair<Node*, list<string> > toReturn(selected, diffLists(other, *inProcess));
	return toReturn;
}
