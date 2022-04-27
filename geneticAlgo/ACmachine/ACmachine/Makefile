CXX = g++
CXXFLAGS = -O0 -Wall -std=c++11
#CXXFLAGS = -O3 -march=native -Wall
INCLUDE =
TLIB =

#-----Suffix Rules---------------------------
# set up C++ suffixes and relationship between .cc and .o files

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

.cpp :
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@ -lm $(TLIB) -lg++

#-----File Dependencies----------------------

GOAL = ACmachine
SRC = main.cpp baseTM.cpp brainFuck.cpp ga.cpp chromosome.cpp


OBJ = $(addsuffix .o, $(basename $(SRC)))

$(GOAL): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(TLIB) -o $@ $(OBJ)

#-----Other stuff----------------------------
depend:
	makedepend $(SRC)

clean:
	rm -f $(OBJ)
	rm -f $(GOAL)
