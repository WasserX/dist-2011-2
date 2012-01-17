/*
 * Slave.h
 *
 *  Created on: 10 Jan 2012
 *      Author: ricardo
 */

#ifndef SLAVE_H_
#define SLAVE_H_

#include <mpi.h>
#include "Master.h"

class Slave {
private:
	char command[Master::COMMAND_SIZE];
	char fileNames[Master::FILE_NAME_SIZE];
  	MPI_Status status;

public:
	Slave();
	virtual ~Slave();
	void receiveTask();
	//void execute();
};

#endif /* SLAVE_H_ */

