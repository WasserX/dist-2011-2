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

	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/

	if(rank == 0) {
		list<int> listIds;
		for(int i=1; i<size; i++) {
			listIds.push_back(i);
		}
		Master* master = new Master( parseFile("./Makefile"), listIds);
		master->execute();
	}
	else {
		Slave* slave = new Slave(rank);
		slave->execute();
	}
	MPI_Finalize();  /* EXIT MPI */
	return 0;
}
