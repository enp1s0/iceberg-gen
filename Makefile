CXX=g++
CXXFLAGS=-std=c++14 -I./src -lpng
TARGET=iceberg-gen.out

$(TARGET) : src/main.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(TARGET)
