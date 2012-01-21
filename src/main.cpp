#include <vector>
#include <list>
#include "utils.h"
#include "Node.h"
#include "Slave.h"
#include "Master.h"
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv); /*START MPI */
	int size;
	int rank;
	string rule = "";

	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/


	//Using rule passed as starting point
	if (argc > 3)
		rule.assign(argv[3]);
	
	if(rank == 0) {
		list<int> listIds;
		for(int i=1; i<size; i++) {
			listIds.push_back(i);
		}
		Master* master = new Master( parseFile("./Makefile", rule), listIds);
		master->execute();
    delete master;
	}
	else {
		Slave* slave = new Slave(rank);
		slave->execute();
	  delete slave;
  }
	MPI_Finalize();  /* EXIT MPI */
	return 0;
}
