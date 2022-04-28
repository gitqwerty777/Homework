CXXFLAGS += -std=c++11 -pthread -O3
LDLIBS += -lGL -lglut
EXEC = MPEG1Decoder
OBJS = bitstream.o dct.o decode.o main.o struct.o vlc.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC) $(LDLIBS)

clean:
	$(RM) $(EXEC) $(OBJS)
