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
	rank += 48;
	//Create and change to a new working directory
	std::string command = " mkdir -p " + (char)rank;
	command += " && cd " + (char)rank;
	system(command.c_str());
}

Slave::~Slave() {
	//When finished delete the working directory
	std::string command = string("cd ../ && rm -r " + (char)(id+48));
	system(command.c_str());
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

void Slave::receiveTask() {

	MPI_Recv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID, Master::COMMAND_TAG, MPI_COMM_WORLD, &status); //command

	MPI_Recv(fileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::FILE_NAME_TAG, MPI_COMM_WORLD, &status); //file name
	//MPI_Recv(files[i], 1024, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file

	//Execute command
	system(command);

}

/*void Slave::execute() {

	bool flagFinish = false;
	bool flagReceived = false;

	while( ! flagFinish || !flagReceived) {
	Test(finish); Test(Received)
	}
	(abaixo execute) mpi_irecv(finish; mpi_irecv(command);
	)depois do while) if(finish) --> finish; else receiveTask(); 
	else receiveTask(); irecv(command); laco;
}*/



