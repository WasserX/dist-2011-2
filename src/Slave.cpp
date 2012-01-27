/*
 * Slave.cpp
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */
		

#include "Slave.h"

Slave::Slave(int rank) {
	id = rank;
	changeDir();
}

Slave::~Slave() {
	cleanUp();
  #ifdef DEBUG  
  cout << "Slave " << id << " Finished" << endl;
  #endif
}

void Slave::changeDir(){
	char number[10];
	sprintf(number,"%d",id);
	string action = "cd /tmp && mkdir -p ";
	system(action.append(number).c_str());
	action = "/tmp/";
	action.append(number);
	chdir(action.c_str());
}

void Slave::cleanUp(){
	char number[10];
	sprintf(number,"%d",id);
	chdir("../");
	string action = "rm -rf ";
	system(action.append(number).c_str());
}

std::map<std::string, std::string> Slave::getChangedFiles(){
	string commandLS = "ls --time-style='+%H:%M:%S' -l | awk \'{print $";
  commandLS.append(colDataNameLS).append("\" \"$").append(colDataDateLS).append("}\'");
  FILE* pipe = popen(commandLS.c_str(), "r");
	char buffer[Master::FILE_NAME_SIZE];
	stringstream ss;
	std::map<std::string, std::string> tempMap;
	while(!feof(pipe)) {
		if(fgets(buffer, Master::FILE_NAME_SIZE, pipe) != NULL) {
			ss << buffer;
		}
	}
	string key, value;
	while (ss >> key) {
		ss >> value; 
		tempMap.insert(pair<string,string>(trim(key),trim(value)));
	}
	pclose(pipe);
  return tempMap; 
}

void Slave::sendFinished() {
	using namespace std;
	map<string, string> mapChangedFiles = getChangedFiles();
	map<string, string>::iterator ptFile;
  	list<string> changedFiles;
	
	for(map<string, string>::iterator it = mapChangedFiles.begin(); it != mapChangedFiles.end(); it++) {
		ptFile = mapFiles.find(it->first);
		if(ptFile != mapFiles.end()) {
			 if(ptFile->second.compare(it->second)) {
				changedFiles.push_back(it->first);
			}		
		}
		else {
			changedFiles.push_back(it->first);
		}
	}

  char* fileNames = getFilesAndSizes(changedFiles);

	//File List	
	MPI_Send(fileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::SLAVE_FILE_NAME_TAG, MPI_COMM_WORLD);
	
	//Send Files
	pair<char*, unsigned long> fileContent;
	string fileName;
	int size;
	istringstream iss(fileNames);
	
	while(iss >> fileName){
		iss >> size;
		fileContent = readFile(fileName);
		MPI_Send(fileContent.first, fileContent.second, MPI_BYTE, Master::ID	, Master::FILE_SEND_TAG, MPI_COMM_WORLD);
	}	
	
}

void Slave::receiveTask(const char command[]) {
	//Command has been received asynchronously
	char fileNames[Master::FILE_NAME_SIZE];
	MPI_Recv(fileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, 
	 Master::FILE_NAME_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Receive Terminals
	
  #ifdef DEBUG
	std::cout << "Slave " << id << " received command: " << command << endl;
  #endif

	string fileName;
	int size, receivedExec = 0;
	char* buffer;	
	istringstream iss(fileNames);

	//Treating Executable
	iss >> receivedExec;
	if(receivedExec){
		iss >> fileName >> size;
		buffer = (char *)malloc(size);
		MPI_Recv(buffer, size, MPI_BYTE, Master::ID, Master::FILE_SEND_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		writeFile(buffer, size, fileName);
		string permissionCommand = "chmod +x ";
		system(permissionCommand.append(fileName).c_str());
	}
	//Treating other files
	while(iss >> fileName){
		iss >> size;
		buffer = (char *)malloc(size);
		MPI_Recv(buffer, size, MPI_BYTE, Master::ID, Master::FILE_SEND_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		writeFile(buffer, size, fileName);
	}
	
	mapFiles = getChangedFiles();
	//Execute command
	system(command);
}

void Slave::execute() {
	int flagFinish = 0;
	int flagReceived = 0;
	int finish;
	char command[Master::COMMAND_SIZE];

	MPI_Irecv(&finish, Master::FINISH_SIZE, MPI_INT, Master::ID, 
		Master::FINISH_TAG, MPI_COMM_WORLD, &reqFinished);
	
	MPI_Irecv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID, Master::COMMAND_TAG, MPI_COMM_WORLD, &reqReceived);
	
	do{
		MPI_Test(&reqFinished, &flagFinish, MPI_STATUS_IGNORE);
		MPI_Test(&reqReceived, &flagReceived, MPI_STATUS_IGNORE);
		if(flagReceived){
			receiveTask(command);
			sendFinished();
			MPI_Irecv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID,
							 Master::COMMAND_TAG, MPI_COMM_WORLD, &reqReceived);
			flagReceived = 0;
		}
	}while( !flagFinish );
}
