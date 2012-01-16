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

	MPI_Recv(intBuffer, 1, MPI_INT, Master::ID, Master::FILE_QUANT_TAG, MPI_COMM_WORLD, &status); //commandSize
	fileCount = intBuffer[0];

	for(int i=0; i< fileCount; i++)
	{
		MPI_Recv(fileNames[i], Master::FILE_NAME_SIZE, MPI_BYTE, Master::ID, Master::FILE_NAME_TAG, MPI_COMM_WORLD, &status); //file name
		//MPI_Recv(files[i], 1024, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file
	}
}


