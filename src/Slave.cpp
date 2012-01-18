/*
 * Slave.cpp
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */
		

#include "Slave.h"

Slave::Slave(int rank) {
	id = rank;
	//Create and change to a new working directory
	std::string command = "mkdir " + id;
	command += " && cd " + id;
	system(command.c_str());
}

Slave::~Slave() {
	//When finished delete the working directory
	std::string command = string("cd ../ && rm -r " + id);
	system(command.c_str());
}

//void sendFinished(File file)

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



