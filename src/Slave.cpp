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
	//changeDir();
}

Slave::~Slave() {
	//cleanUp();
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

//void sendFinished(File file)

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
	//File_names
	string allNames = "";
	for(std::list<std::string>::iterator it = changedFiles.begin(); it != changedFiles.end(); it++){
		allNames.append(*it + " ");
	}
	char allFileNames[Master::FILE_NAME_SIZE];
	strcpy(allFileNames, allNames.substr(0, allNames.size()-1).c_str());
	MPI_Send(allFileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::RESPONSE_FILE_LIST_TAG, MPI_COMM_WORLD);
}

void Slave::receiveTask() {
	MPI_Recv(fileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::FILE_NAME_TAG, MPI_COMM_WORLD, &status); //file name
	//MPI_Recv(files[i], 1024, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file
	std::string result = "touch ";
	result += FILE_CHECKPOINT;
	system(result.c_str());
	//Execute command
	system(command);
}

void Slave::execute() {
	int flagFinish = 0;
	int flagReceived = 0;
	int finish;

	MPI_Irecv(&finish, Master::FINISH_SIZE, MPI_INT, Master::ID, 
		Master::FINISH_TAG, MPI_COMM_WORLD, reqFinished);
	
	MPI_Irecv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID, Master::COMMAND_TAG, MPI_COMM_WORLD, reqReceived);
	
	do{
		MPI_Test(reqFinished, &flagFinish, &status);
		MPI_Test(reqReceived, &flagReceived, &status);
		if(flagReceived){
			receiveTask();
			sendFinished();
			MPI_Irecv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID,
							 Master::COMMAND_TAG, MPI_COMM_WORLD, reqReceived);
			flagReceived = 0;
		}
	}while( !flagFinish );
}
