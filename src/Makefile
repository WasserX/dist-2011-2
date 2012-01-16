CC = g++
MPICC = mpicxx
CFLAGS = -Wall
LFLAGS =
OBJS = Node.o Master.o Slave.o
OUTPUT = distMake

$(OUTPUT): main.cpp $(OBJS)
	$(MPICC) $^ -o $@ $(CFLAGS) $(LFLAGS)

%.o: %.cpp %.h
	$(MPICC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OUTPUT) $(OBJS)
