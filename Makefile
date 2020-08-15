CXX=g++
CXXFLAGS=-std=c++14 -I./src
TARGET=iceberg-gen.out

$(TARGET) : src/main.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(TARGET)
