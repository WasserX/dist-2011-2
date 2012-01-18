/*
 * Slave.h
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */

#ifndef SLAVE_H_
#define SLAVE_H_

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include "Master.h"

class Slave {
private:
	int id;
	char command[Master::COMMAND_SIZE];
	char fileNames[Master::FILE_NAME_SIZE];
  MPI_Status status;

	const static char* FILE_CHECKPOINT;
	
	std::list<std::string> getChangedFiles();
	
public:
	Slave(int rank);
	virtual ~Slave();
	void receiveTask();
	//void execute();
};

#endif /* SLAVE_H_ */

