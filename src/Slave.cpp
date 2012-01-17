/*
 * Slave.cpp
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */
		

#include "Slave.h"

Slave::Slave() {
	// TODO Auto-generated constructor stub

}

Slave::~Slave() {
	// TODO Auto-generated destructor stub
}

//void sendFinished(File file)

void Slave::receiveTask() {

	MPI_Recv(command, Master::COMMAND_SIZE, MPI_BYTE, Master::ID, Master::COMMAND_TAG, MPI_COMM_WORLD, &status); //command

	MPI_Recv(fileNames, Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::FILE_NAME_TAG, MPI_COMM_WORLD, &status); //file name
	//MPI_Recv(files[i], 1024, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file

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



