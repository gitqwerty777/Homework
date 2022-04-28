SRC_FILES = main.cpp huffman_decode.cpp mcu_decode.cpp jpg_decode.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
TARGET = jpeg-decoder

# compile program with "make"
all: $(TARGET)

# run program with "./jpeg-decoder <IMAGE_NAME>" or "make -f README run IMAGE=<IMAGE_NAME>"
run: jpeg-decoder
	./jpeg-decoder $(IMAGE)

jpeg-decoder: $(OBJ_FILES)
	g++ -lm -std=c++11 $^ -o $@

%.o: %.cpp
	g++ -lm -std=c++11 -c $^

clean:
	rm -rf $(OBJ_FILES) $(TARGET)
