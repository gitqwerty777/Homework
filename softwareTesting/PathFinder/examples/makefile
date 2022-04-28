CXXFLAG = -std=c++14 -fprofile-arcs -ftest-coverage

all: cities.cpp Node.o Dijkstra.o cities.o
	g++ $(CXXFLAG) -o cities Dijkstra.o Node.o cities.o 

image: image.o AStar.o Node.o 
g++ $(CXXFLAG) -o image AStar.o Node.o image.o -lsfml-graphics

cities.o: cities.cpp
	g++ $(CXXFLAG) -c  cities.cpp -o cities.o

Node.o: ../src/Node.cpp
	g++ $(CXXFLAG) -c  ../src/Node.cpp -o Node.o

Dijkstra.o: ../src/Dijkstra.cpp
	g++ $(CXXFLAG) -c  ../src/Dijkstra.cpp -o Dijkstra.o

images.o: image.cpp
	g++ $(CXXFLAG) -c  image.cpp -lsfml-graphics -o image.o

AStar.o: ../src/AStar.cpp
	g++ $(CXXFLAG) -c ../src/AStar.cpp -o AStar.o
