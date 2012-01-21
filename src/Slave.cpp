/*
 * Slave.cpp
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */
		

#include "Slave.h"

const char* Slave::FILE_CHECKPOINT = "___FILE_CHECKPOINT___";

Slave::Slave(int rank) {
	id = rank;
	changeDir();
}

Slave::~Slave() {
	cleanUp();
}

void Slave::changeDir(){
	char number[10];
	sprintf(number,"%d",id);
	string action = "mkdir -p ";
	system(action.append(number).c_str());
	chdir(number);
}

void Slave::cleanUp(){
	char number[10];
	sprintf(number,"%d",id);
	chdir("../");
	string action = "rm -rf ";
	system(action.append(number).c_str());
}

std::list<std::string> Slave::getChangedFiles(){
	std::list<std::string> fileList;
	FILE* pipe = popen("ls -t1", "r");
	if (!pipe)
		return fileList;
	
	char buffer[Master::FILE_NAME_SIZE];
	std::stringstream ss;
	std::string output;
	while(!feof(pipe)) {
		if(fgets(buffer, Master::FILE_NAME_SIZE, pipe) != NULL)
			ss << buffer;
	}
	pclose(pipe);
	
	while(ss >> output && output != FILE_CHECKPOINT)
		fileList.push_back(output);
	return fileList;
}

void Slave::sendFinished() {
	std::list<std::string> changedFiles = getChangedFiles();
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
	 
	std::cout << "Slave " << id << " received command: " << command << endl;
	
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
		writeFile(buffer, fileName);
		string permissionCommand = "chmod +x ";
		system(permissionCommand.append(fileName).c_str());
	}
	//Treating other files
	while(iss >> fileName){
		iss >> size;
		buffer = (char *)malloc(size);
		MPI_Recv(buffer, size, MPI_BYTE, Master::ID, Master::FILE_SEND_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		cout << "Received File " << fileName << endl;
		writeFile(buffer, fileName);
	}
	
	std::string touchCommand = "touch ";
	touchCommand += FILE_CHECKPOINT;
	system(touchCommand.c_str());
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
