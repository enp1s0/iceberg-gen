CXX=g++
CXXFLAGS=-std=c++11 -I./src -lpng -fopenmp
TARGET=iceberg-gen.out

$(TARGET) : src/main.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(TARGET)
