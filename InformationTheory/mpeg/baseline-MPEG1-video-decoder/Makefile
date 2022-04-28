all: decoder

decoder: main.o bit_reader.o video.o video_init.o video_display.o
	g++ --std=c++11 -Wall $^ -o $@ -lm -lpthread -lX11

%.o: %.cpp
	g++ --std=c++11 -Wall -c $^

clean:
	rm -rf *.o decoder
