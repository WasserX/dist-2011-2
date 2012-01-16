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
	char command[1000];
	int fileCount;
	char fileNames[][50];
  MPI_Status status;
	int intBuffer[1];

public:
	Slave();
	virtual ~Slave();
	//void sendFinished(File file);
	void receiveTask();
};

#endif /* SLAVE_H_ */

