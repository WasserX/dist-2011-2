CC = g++
MPICC = mpicxx
CFLAGS = -Wall
LFLAGS =
OBJS = Node.o Master.o
OUTPUT = distMake

$(OUTPUT): main.cpp $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)

%.o: %.cpp %.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OUTPUT) $(OBJS)
