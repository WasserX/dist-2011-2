/*
 * Slave.cpp
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */
		

#include "Slave.h"

#define MASTERID 0
#define MESSAGETAG 120

Slave::Slave() {
	// TODO Auto-generated constructor stub

}

Slave::~Slave() {
	// TODO Auto-generated destructor stub
}

//void sendFinished(File file)

void Slave::receiveTask() {
	MPI_Recv(command, 1000, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //command

	MPI_Recv(intBuffer, 1, MPI_INT, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //commandSize
	fileCount = intBuffer[0];

	for(int i=0; i< fileCount; i++)
	{
		MPI_Recv(fileNames[i], 50, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file name
		//MPI_Recv(files[i], 1024, MPI_BYTE, MASTERID, MESSAGETAG, MPI_COMM_WORLD, &status); //file
	}
}


