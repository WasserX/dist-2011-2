#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/time.h>

#define NPROCESSORS 6

timeval startTime;

using namespace std;

void writeFile(char* buffer, int size, std::string fileName) {
	FILE* fp;
	fp = fopen(fileName.c_str(), "w+b");
	fwrite(buffer, sizeof(char), size, fp);
	fclose(fp);
}

void start(){
	gettimeofday(&startTime, NULL);
}

double stop(){
	timeval endTime;
	long seconds, useconds;
	double duration;

	gettimeofday(&endTime, NULL);

	seconds  = endTime.tv_sec  - startTime.tv_sec;
	useconds = endTime.tv_usec - startTime.tv_usec;

	duration = seconds + useconds/1000000.0; //return in miliseconds

	return duration;
}

int main(int argc, char** argv) {

	if (argc != 4){
		cout << "Usage: " <<argv[0] <<" <outputName> <executable> <benchmark Make(1 or 0)>" <<endl;
	}
	double timeElapsed;
	int numberOfProcessors[NPROCESSORS] = {2,5,10,15,20,25};
	std::ostringstream oss;

	string fileName = argv[1];
	string execName = argv[2];
	string doMake = argv[3];

	std::string toSave = "";
	toSave.append("Name	Test	Processors	Time Elapsed\n");

	for(int j=0; j < 3; j++) {
		if(!doMake.compare("1")){
		cout << "test " << j << endl;
		cout << "make   "; 
		toSave.append("make		");
		oss.str("");
		oss << j+1;
		toSave.append(oss.str());
		toSave.append("		1		");


		start();
		system("make");
		timeElapsed = stop();
		oss.str("");
		oss << timeElapsed;
		toSave.append(oss.str());
		toSave.append("\n");
		cout << oss.str() << endl;
		system("make clean");
}
		toSave.append("distMake		");
		cout << "distMake" << endl; 
		for(int i=0; i< NPROCESSORS; i++) {			
			oss.str("");
			oss << numberOfProcessors[i];
			toSave.append(oss.str());
			cout << "Pass: " << (j+1) << " Processors: " << numberOfProcessors[i] << " Time: ";

			std::string mpiCommand = "mpirun -machinefile machines -np ";
			mpiCommand.append(oss.str() + " ");
			mpiCommand.append(execName);
			start();
			system(mpiCommand.c_str());
			timeElapsed = stop();
			oss.str("");
			oss << timeElapsed;
			toSave.append("		");
			toSave.append(oss.str());
			cout << oss.str() << endl;
			toSave.append("\n");
			system("make clean");
		}
	}

	char *buffer = (char*)malloc(toSave.size());
	strcpy(buffer, toSave.c_str());
	writeFile(buffer, toSave.size(), fileName.c_str());
	return 0;
}
