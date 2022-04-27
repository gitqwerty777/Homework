CXX = g++
#CXXFLAGS = -O0 -g -Wall
CXXFLAGS = -O2 -Wall -std=c++11
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

GOAL = SGA

SRC = ga.cpp GAmain.cpp myrand.cpp chromosome.cpp global.cpp mt19937ar.cpp


OBJ = $(addsuffix .o, $(basename $(SRC)))

$(GOAL): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(TLIB) -o $@ $(OBJ)

#-----Other stuff----------------------------
depend:
	makedepend $(SRC)

clean:
	rm -f $(OBJ)

# DO NOT DELETE

ga.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
ga.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
ga.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
ga.o: /usr/include/bits/typesizes.h /usr/include/libio.h
ga.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h
ga.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
ga.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
ga.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
ga.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ga.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ga.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
ga.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
ga.o: /usr/include/math.h /usr/include/bits/huge_val.h
ga.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h global.h
ga.o: chromosome.h myrand.h mt19937ar.h statistics.h ga.h
GAmain.o: /usr/include/math.h /usr/include/features.h
GAmain.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
GAmain.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
GAmain.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
GAmain.o: /usr/include/bits/mathcalls.h statistics.h ga.h chromosome.h
GAmain.o: myrand.h mt19937ar.h global.h
myrand.o: /usr/include/math.h /usr/include/features.h
myrand.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
myrand.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
myrand.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
myrand.o: /usr/include/bits/mathcalls.h /usr/include/time.h
myrand.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
myrand.o: /usr/include/stdlib.h /usr/include/sys/types.h
myrand.o: /usr/include/endian.h /usr/include/bits/endian.h
myrand.o: /usr/include/sys/select.h /usr/include/bits/select.h
myrand.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
myrand.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
myrand.o: /usr/include/alloca.h myrand.h mt19937ar.h
chromosome.o: /usr/include/stdio.h /usr/include/features.h
chromosome.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
chromosome.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
chromosome.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
chromosome.o: /usr/include/libio.h /usr/include/_G_config.h
chromosome.o: /usr/include/wchar.h /usr/include/bits/wchar.h
chromosome.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
chromosome.o: /usr/include/bits/sys_errlist.h global.h chromosome.h myrand.h
chromosome.o: mt19937ar.h
global.o: /usr/include/stdio.h /usr/include/features.h
global.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
global.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
global.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
global.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
global.o: /usr/include/bits/wchar.h /usr/include/gconv.h
global.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
global.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
global.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
global.o: /usr/include/bits/posix2_lim.h global.h chromosome.h myrand.h
global.o: mt19937ar.h
mt19937ar.o: /usr/include/stdio.h /usr/include/features.h
mt19937ar.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
mt19937ar.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
mt19937ar.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
mt19937ar.o: /usr/include/libio.h /usr/include/_G_config.h
mt19937ar.o: /usr/include/wchar.h /usr/include/bits/wchar.h
mt19937ar.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
mt19937ar.o: /usr/include/bits/sys_errlist.h
