# Use the Variables UBUNTU or REDHAT to control the target system

TARGET = UBUNTU
CC = g++
MPICC = mpicxx
CFLAGS = -Wall -D$(TARGET)
LFLAGS =
OBJS = Node.o Master.o Slave.o utils.o
OUTPUT = distMake

$(OUTPUT): main.cpp $(OBJS)
	$(MPICC) $^ -o $@ $(CFLAGS) $(LFLAGS)

%.o: %.cpp %.h
	$(MPICC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OUTPUT) $(OBJS)
